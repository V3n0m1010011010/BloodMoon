#include <WiFi.h>
#include <algorithm>
#include <vector>
#include <string>


struct AccessPoint {
  String ssid;
  std::array<uint8_t, 6> bssid;
};
struct Station {
  String ssid;
  std::array<uint8_t, 6> bssid;
  int rssid;
};
class wifi {
private:
  std::vector<AccessPoint> apList;
  std::vector<bool> selectedApList;
  std::vector<Station> stList;
  std::vector<bool> selectedStList;
  bool firstApScan = false;
  bool scanAp = false;
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

public:
  void init();
  void scanAps();
  void scanSts();
  void sniffer(uint8_t* buf, uint16_t len);
  double getMultiplicator(long maxRow, long LineVal);
  void packetMonitor();
  std::vector<AccessPoint> getApList();
  void clearApList();
  void setApScanStatus(bool status);
};