#include <WiFi.h>
#include "esp_wifi.h"
#include <algorithm>
#include <vector>
#include <string>
#include <mutex>


struct AccessPoint {
  String ssid;
  std::array<uint8_t, 6> bssid;
  int channel;
  bool isSelected;
};
struct Station {
  std::array<uint8_t, 6> mac;
  std::array<uint8_t, 6> bssid;
  int rssi;
  int channel;
  bool isSelected;
};
class wifi {
private:
  std::mutex apMutex;
  std::vector<AccessPoint> apList;
  std::mutex stMutex;
  std::vector<Station> stList;
  static wifi* instance;
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
  int curChannel = 1;
  unsigned long maxVal = 0;
  double multiplicator = 0.0;
  unsigned int val[128];
  int currentChannel = 1;
  esp_timer_handle_t channelHopperTimer;
  static constexpr int MAX_CHANNEL = 13;
  static void channelHopCallback(void* arg) {
    wifi* self = static_cast<wifi*>(arg);
    self->currentChannel++;
    if (self->currentChannel > MAX_CHANNEL) {
      self->currentChannel = 1;
    }
    esp_wifi_set_channel(self->currentChannel, WIFI_SECOND_CHAN_NONE);
  }
public:
  uint8_t deauthPacket[26] = {
    /*  0 - 1  */ 0xC0, 0x00,
    /*  2 - 3  */ 0x00, 0x00,
    /*  4 - 9  */ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    /* 10 - 15 */ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    /* 16 - 21 */ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
    /* 22 - 23 */ 0x00, 0x01,
    /* 24 - 25 */ 0x01, 0x00
  };
  void init();
  void scanAps();
  void scanSts();
  void stopWifiScan();
  static void beaconCallbackAp(void* buf, wifi_promiscuous_pkt_type_t type);
  static void beaconCallbackSt(void* buf, wifi_promiscuous_pkt_type_t type);
  void packetSniffer(uint8_t* buf, uint16_t len);
  double getMultiplicator(long maxRow, long LineVal);
  void packetMonitor();
  uint8_t* createDeauthPacket(uint8_t* bssid);
  std::vector<AccessPoint> getApList();
  std::vector<Station> getStList();
  void setScanStatus(int i, bool status);
  std::vector<bool> getFirstScan();
};