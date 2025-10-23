#pragma once
#include <WiFi.h>
#include "esp_wifi.h"
#include <algorithm>
#include <vector>
#include <string>
#include <mutex>

struct AccessPoint
{
  String ssid;
  std::array<uint8_t, 6> bssid;
  int channel;
  int rssi;
  String encryption;
  bool isDeauthSelected;
  bool isBecCloneSelected;
  bool isWPACaptureSelected;
};
struct Station
{
  std::array<uint8_t, 6> mac;
  std::array<uint8_t, 6> bssid;
  int rssi;
  int channel;
  bool isDeauthSelected;
  bool isBecCloneSelected;
};

uint8_t deauthPacket[26] = {
    /*  0 - 1  */ 0xC0, 0x00,
    /*  2 - 3  */ 0x00, 0x00,
    /*  4 - 9  */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    /* 10 - 15 */ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    /* 16 - 21 */ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    /* 22 - 23 */ 0x00, 0x01,
    /* 24 - 25 */ 0x01, 0x00};

struct pcap_hdr_t
{
  uint32_t magic_number = 0xa1b2c3d4;
  uint16_t version_major = 2;
  uint16_t version_minor = 4;
  int32_t thiszone = 0;
  uint32_t sigfigs = 0;
  uint32_t snaplen = 65535;
  uint32_t network = 105;
} pcap_hdr;
struct pcaprec_hdr_t
{
  uint32_t ts_sec;
  uint32_t ts_usec;
  uint32_t incl_len;
  uint32_t orig_len;
};

class wifi
{
private:
  static wifi *instance;
  int currentChannel = 1;

  std::mutex apMutex;
  std::vector<AccessPoint> apList;
  std::mutex stMutex;
  std::vector<Station> stList;
  bool firstApScan = false;
  bool firstStScan = false;
  bool scanAp = false;
  bool scanSt = false;

  unsigned long maxCh = 13;
  unsigned long packetRate = 5;
  unsigned long prevTime = 0;
  unsigned long curTime = 0;
  unsigned long pkts = 0;
  unsigned long no_deauths = 0;
  unsigned long deauths = 0;
  unsigned long maxVal = 0;
  double multiplicator = 0.0;
  unsigned int val[128];

  
  static bool deauthing;
  static bool beaconCloning;
  static bool WPACapturing;

  AccessPoint WPACaptureTarget;
  bool WPACaptureTargetSet = false;
  bool WPAKeys[4] = {false, false, false, false};
  String WPACaptureSDLocation = "/";


  esp_timer_handle_t channelHopperTimer;
  static constexpr int MAX_CHANNEL = 13;
  static void channelHopCallback(void *arg)
  {
    wifi *self = static_cast<wifi *>(arg);
    self->currentChannel++;
    if (self->currentChannel > MAX_CHANNEL)
    {
      self->currentChannel = 1;
    }
    esp_wifi_set_channel(self->currentChannel, WIFI_SECOND_CHAN_NONE);
  }

public:
  void init();

  void scanAps();
  void scanSts();
  void stopWifiScan();
  static void beaconCallbackAp(void *buf, wifi_promiscuous_pkt_type_t type);
  static void beaconCallbackSt(void *buf, wifi_promiscuous_pkt_type_t type);

  void packetSniffer(uint8_t *buf, uint16_t len);
  double getMultiplicator(long maxRow, long LineVal);
  void packetMonitor();

  void startDeauth();
  void stopDeauth();
  static void deauthAttack(void *param);
  void sendDeauthPaket(int ch, uint8_t* bssid);
  bool isDeauthing();
  uint8_t *createDeauthPacket(uint8_t *bssid);


  void startWPACapture();
  void stopWPACapture();
  bool isWPACapturing();
  static void WPACapture(void *param);
  static void wpa_packet_sniffer(void *buf, wifi_promiscuous_pkt_type_t type);



  bool macEqual(const uint8_t *a, const uint8_t *b);
  int getHdrLen(const uint8_t *data, int len);
  bool isEAPOLPacket(const uint8_t *data, int len);
  bool extractReplay(const uint8_t *data, int len, uint8_t replay[8]);
  int getKeyType(const uint8_t *data, int len);

  void setApSelectings(int id, int mode, bool state);
  std::vector<AccessPoint> getApList();
  std::vector<Station> getStList();
  int getApDeauthSelects();
  int getStDeauthSelects();
  std::vector<bool> getFirstScan();
};