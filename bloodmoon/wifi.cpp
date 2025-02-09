#include <WiFi.h>
#include "esp_wifi.h"
#include "wifi.h"
#include "Menu.h"
#include "display.h"
extern Menu* activem;
extern display dis;
void wifi::init() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}
void wifi::scanAps() {
  scanAp = true;
  firstApScan = true;
  apList.clear();
  WiFi.mode(WIFI_STA);
  WiFi.scanNetworks(true, true);
  uint32_t lastScanMillis = millis();
  bool scanInProgress = false;
  while (scanAp) {
    int scanStatus = WiFi.scanComplete();
    if (scanStatus >= 0) {
      bool listUpdated = false;
      for (int i = 0; i < scanStatus; ++i) {
        String ssid = WiFi.SSID(i);
        const uint8_t* bssid = WiFi.BSSID(i);
        bool exists = false;
        for (const auto& ap : apList) {
          if (ap.ssid == ssid && memcmp(ap.bssid.data(), bssid, 6) == 0) {
            exists = true;
            break;
          }
        }
        if (!exists) {
          std::array<uint8_t, 6> bssidArray;
          memcpy(bssidArray.data(), bssid, 6);
          apList.push_back({ssid, bssidArray});
          listUpdated = true;
        }
      }
      if (listUpdated) {
        dis.renderApScanMenu();
      }
      WiFi.scanDelete();
      scanInProgress = false;
    }

    // Neuen Scan alle 3 Sekunden starten (nur wenn nicht aktiv)
    if (!scanInProgress && (millis() - lastScanMillis > 3000)) {
      WiFi.scanNetworks(true, true);
      scanInProgress = true;
      lastScanMillis = millis();
    }

    // Non-blocking Input-Check alle 50ms
    static uint32_t lastInputCheck = 0;
    if (millis() - lastInputCheck > 50) {
      if (activem->handleInput()) {
        // Bei User-Input sofort reagieren
        break;
      }
      lastInputCheck = millis();
    }

    // Kurze Pause um andere Tasks zu ermöglichen
    delay(10);
  }

  // Cleanup
  WiFi.scanDelete();
  scanAp = false;
}
void wifi::scanSts() {
}
void wifi::sniffer(uint8_t* buf, uint16_t len) {
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
std::vector<AccessPoint> wifi::getApList() {
  return apList;
}
void wifi::clearApList(){
  apList.clear();
}
void wifi::setApScanStatus(bool status) {
  scanAp = status;
}