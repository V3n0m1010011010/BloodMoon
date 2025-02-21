#include <TFT_eSPI.h>
#include <array>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include "display.h"
#include "wifi.h"
#include "rf24.h"
#include "menu.h"
#include "fonts/icons.h"
#include "settings.h"
#include "globals.h"
#include "variables.h"
#include "images/images.h"

extern wifi Wifi;
extern Menu* activem;
extern TFT_eSPI tft;
extern nrf24 nRF24;

display::display(int startX, int startY, int endX, int endY)
  : startX(startX), startY(startY), endX(endX), endY(endY) {
  ledcSetup(pwmChannel, pwmFrequency, 8);
  ledcAttachPin(backlightPin, pwmChannel);
  posX = width / 2;
}
//------------------------------------------------------------------------------------------------
void display::renderBoot() {  // Render boot sequence
  tft.fillScreen(TFT_BLACK);
  tft.drawBitmap(width / 4, height / 4 - 10, ghost, 64, 64, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("BloodMoon", width / 2, 100);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Firmware", width / 2, 128 / 3);
  tft.drawString("by", width / 2, height / 2);
  tft.drawString("V3n0m1010011010", width / 2, (128 / 3) * 2);
  delay(1000);
  tft.setTextDatum(ML_DATUM);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 5);
  tft.print("nRF24l01");
  for (int i = 0; i < 3; i++) {
    tft.print(".");
    delay(300);
  }
  delay(500);
  tft.setCursor(10, 15);
  tft.println(nRF24.getInit() ? "detected" : "not detected");
  delay(1000);
  tft.unloadFont();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderHead() {
  tft.setTextFont(0);
  tft.fillRect(0, 0, 128, 22, tft.color565(161, 0, 0));
  tft.drawRect(0, 0, 128, 22, tft.color565(255, 255, 255));
  tft.fillRect(5, 0, 128 - 5 * 2, 22, tft.color565(161, 0, 0));
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(activem->getTitle().c_str(), width / 2, 11);
}
void display::renderMainWithIcon() {
  int selectedIndex = activem->getSelectedIndex();
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  if (currentIconsSet == 1) {
    tft.loadFont(customIcons1);
  } else if (currentIconsSet == 2) {
    tft.loadFont(customIcons2);
  }
  if (selectedIndex >= 0 && selectedIndex < iconSize) {
    char currentIcon = activem->getIcon(selectedIndex);
    tft.drawString(String(currentIcon), width / 2, height / 2 - 8);
    tft.unloadFont();
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.drawString(section.c_str(), width / 2, height / 2 + 27);
    tft.drawChar('<', 20, 62);
    tft.drawChar('>', 128 - 25, 62);
  }
}
void display::renderMain() {
  int selectedIndex = activem->getSelectedIndex();
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  const int maxVisibleItems = 3;
  const int maxTextWidth = 15;
  static int scrollOffset = 0;
  if (selectedIndex < scrollOffset) {
    scrollOffset = selectedIndex;
  } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, sectionSize); ++i) {
    int y = 25 + ((i - scrollOffset) * 22);
    if (i == selectedIndex) {
      tft.fillRect(5, y, width - 10, 22, tft.color565(50, 0, 0));
      tft.drawRect(5, y, width - 10, 22, tft.color565(10, 0, 0));
    } else {
      tft.fillRect(5, y, width - 10, 22, tft.color565(80, 0, 0));
      tft.drawRect(5, y, width - 10, 22, tft.color565(10, 0, 0));
    }
    tft.drawString(activem->getSection(i).substr(0, maxTextWidth).c_str(), width / 2, y + 11);
  }
  if (sectionSize > maxVisibleItems && selectedIndex < sectionSize - 1) {
    if (currentIconsSet == 1) {
      tft.loadFont(customIcons1);
    } else if (currentIconsSet == 2) {
      tft.loadFont(customIcons2);
    }
    tft.drawString(String('\x24'), width / 2 - 4, 95);
    tft.unloadFont();
    tft.setTextFont(2);
  }
}
void display::renderFoot() {
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextDatum(ML_DATUM);
  tft.fillRect(0, 112, 128, 16, tft.color565(161, 0, 0));
  tft.drawString("Sel", 5, 128 - 8);
  tft.drawString("Mov", 98, 128 - 8);
}
void display::renderAll() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  if (activem->getType() == "normal" || activem->getType() == "scan" || activem->getType() == "deauther" || activem->getType() == "jammer") {
    if (activem->isMenuWithIcon()) {
      renderMainWithIcon();
    } else {
      renderMain();
    }
  } else if (activem->getType() == "apSelect") {
    renderApSelectMenu();
  } else if (activem->getType() == "stSelect") {
    renderStSelectMenu();
  }
  renderHead();
  renderFoot();
}

void display::renderIconScrollAnimation(bool dir, int frames) {
  int selectedIndex = activem->getSelectedIndex();
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < frames; i++) {
    int dx = dir ? 128 / frames : -(128 / frames);
    posX -= dx;
    tft.fillRect(0, startY, endX, endY - startY, TFT_BLACK);
    tft.setTextSize(3);
    if (currentIconsSet == 1) {
      tft.loadFont(customIcons1);
    } else if (currentIconsSet == 2) {
      tft.loadFont(customIcons2);
    }
    if (selectedIndex >= 0 && selectedIndex < iconSize) {
      int indexBefore = (selectedIndex - 1 < 0) ? iconSize - 1 : selectedIndex - 1;
      int indexCurrent = selectedIndex;
      int indexAfter = (selectedIndex + 1 > iconSize - 1) ? 0 : selectedIndex + 1;
      char beforeIcon = activem->getIcon(indexBefore);
      char currentIcon = activem->getIcon(selectedIndex);
      char afterIcon = activem->getIcon(indexAfter);
      tft.setTextWrap(false);
      tft.drawString(String(currentIcon), posX, height / 2 - 8);
      tft.drawString(String(beforeIcon), posX - width, height / 2 - 8);
      tft.drawString(String(afterIcon), posX + width, height / 2 - 8);
      tft.unloadFont();
      tft.setTextSize(1);
      tft.setTextFont(2);
      tft.drawString(activem->getSection(indexCurrent).c_str(), posX, height / 2 + 27);
      tft.drawString(activem->getSection(indexBefore).c_str(), posX - width, height / 2 + 27);
      tft.drawString(activem->getSection(indexAfter).c_str(), posX + width, height / 2 + 27);
    }
    delay(5);
  }
  posX = width / 2;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderApScanMenu() {
  setBrightness(0.3);
  renderHead();
  tft.fillRect(startX, startY, endX, endY - startY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  const int maxVisibleItems = 8;
  const int maxTextWidth = 15;
  static int scrollOffset = 0;
  int selectedIndex = Wifi.getApList().size();
  if (selectedIndex < scrollOffset) {
    scrollOffset = selectedIndex;
  } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, (int)Wifi.getApList().size()); ++i) {
    int y = startY + 10 + ((i - scrollOffset) * 10);
    tft.drawString(Wifi.getApList()[i].ssid.substring(0, maxTextWidth).c_str(), width / 2, y);
  }
  renderFoot();
}
void display::renderStScanMenu() {
  setBrightness(0.3);
  renderHead();
  tft.fillRect(startX, startY, endX, endY - startY, TFT_BLACK);
  tft.setTextSize(1);
  const int maxVisibleItems = 8;
  const int maxTextWidth = 15;
  static int scrollOffset = 0;
  int selectedIndex = Wifi.getStList().size();
  if (selectedIndex < scrollOffset) {
    scrollOffset = selectedIndex;
  } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, (int)Wifi.getStList().size()); ++i) {
    int y = startY + 10 + ((i - scrollOffset) * 10);
    std::array<uint8_t, 6> mac = Wifi.getStList()[i].mac;
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    for (size_t i = 0; i < mac.size(); ++i) {
      ss << std::setw(2) << static_cast<int>(mac[i]);
      if (i < mac.size() - 1) {
        ss << ":";
      }
    }
    tft.setTextDatum(MC_DATUM);
    tft.drawString(ss.str().c_str(), width / 2, y);
  }
  renderFoot();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderApSelectMenu() {
  renderHead();
  int selectedIndex = activem->getSelectedIndex();
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(2);
  if (selectedIndex == 0) {
    tft.fillRoundRect(20, height / 2 - 11, 88, 22, 5, tft.color565(80, 0, 0));
    tft.setTextDatum(MC_DATUM);
    tft.drawString(activem->getSection(0).c_str(), posX, height / 2);
  } else if (Wifi.getFirstScan()[0]) {
    const int maxTextWidth = 13;
    tft.fillRoundRect(startX + 5, startY + 5, endX - 10, endY - startY - 10, 5, tft.color565(80, 0, 0));
    tft.drawRoundRect(startX + 5, startY + 5, endX - 10, endY - startY - 10, 5, TFT_BLACK);
    tft.setTextDatum(ML_DATUM);
    auto ap = Wifi.getApList()[selectedIndex - 1];
    String ssid = ap.ssid;
    std::array<uint8_t, 6> bssid = ap.bssid;
    std::stringstream bssidString;
    bssidString << std::hex << std::uppercase << std::setfill('0');
    for (size_t i = 0; i < bssid.size(); ++i) {
      bssidString << std::setw(2) << static_cast<int>(bssid[i]);
      if (i < bssid.size() - 1) {
        bssidString << ":";
      }
    }
    int channel = ap.channel;
    bool isSelected = ap.isSelected;
    if (ssid.length() > maxTextWidth) {
      ssid = ssid.substring(0, maxTextWidth);
      ssid = ssid + "...";
    }
    tft.drawString(ssid.c_str(), -(width / 2) + posX + 10, startY + 15);
    tft.drawString(bssidString.str().c_str(), -(width / 2) + posX + 10, startY + 30);
  }
  renderFoot();
}
void display::renderStSelectMenu() {
}
//------------------------------------------------------------------------------------------------

void display::renderWifiSelectScrollAnimation(bool dir, int frames) {
  int selectedIndex = activem->getSelectedIndex();
  int sectionSize = activem->getSectionsSize();
  const int boxWidth = 88;
  const int boxHeight = 22;
  const int yCenter = height / 2;
  const int maxTextWidth = 13;
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  for (int i = 0; i < frames; i++) {
    int dx = dir ? 128 / frames : -(128 / frames);
    posX -= dx;
    tft.fillRect(0, startY, endX, endY - startY, TFT_BLACK);
    if (selectedIndex >= 0 && selectedIndex < sectionSize) {
      int indexes[3] = {
        (selectedIndex - 1 < 0) ? sectionSize - 1 : selectedIndex - 1,
        selectedIndex,
        (selectedIndex + 1 > sectionSize - 1) ? 0 : selectedIndex + 1
      };
      for (int j = 0; j < 3; j++) {
        int currentIndex = indexes[j];
        int xPos = posX + (j - 1) * width;
        if (currentIndex == 0) {
          tft.fillRoundRect(xPos - boxWidth / 2, yCenter - 11, boxWidth, boxHeight, 5, tft.color565(80, 0, 0));
          tft.drawString(activem->getSection(0).c_str(), xPos, yCenter);
        } else if (Wifi.getFirstScan()[0]) {
          auto ap = Wifi.getApList()[currentIndex - 1];
          String ssid = ap.ssid;
          std::array<uint8_t, 6> bssid = ap.bssid;
          std::stringstream bssidString;
          bssidString << std::hex << std::uppercase << std::setfill('0');
          for (size_t i = 0; i < bssid.size(); ++i) {
            bssidString << std::setw(2) << static_cast<int>(bssid[i]);
            if (i < bssid.size() - 1) {
              bssidString << ":";
            }
          }
          int channel = ap.channel;
          bool isSelected = ap.isSelected;
          if (ssid.length() > maxTextWidth) {
            ssid = ssid.substring(0, maxTextWidth) + "...";
          }
          tft.fillRoundRect(xPos - boxWidth / 2 + 5, startY + 5, endX - 10, endY - startY - 10, 5, tft.color565(80, 0, 0));
          tft.drawRoundRect(xPos - boxWidth / 2 + 5, startY + 5, endX - 10, endY - startY - 10, 5, TFT_BLACK);
          tft.setTextDatum(ML_DATUM);
          tft.drawString(ssid.c_str(), xPos - boxWidth / 2 + 10, startY + 15);
          tft.drawString(bssidString.str().c_str(), xPos - boxWidth / 2 + 10, startY + 30);
        }
      }
    }
    delay(30);
  }
  posX = width / 2;
}

//------------------------------------------------------------------------------------------------
void display::renderDeauthMenu() {
  renderHead();

  renderFoot();
}
void display::renderNRFJammer() {
  setBrightness(0.9);
  renderHead();
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(1);
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.drawString("Jamming...", startX + 10, startY + 10);
  tft.drawString("press sel to stop", startX + 10, startY + 25);
  renderFoot();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderMonitorMenu() {
  renderHead();

  renderFoot();
}
//------------------------------------------------------------------------------------------------


void display::setBrightness(float brightness) {
  long x = brightness * 255;
  ledcWrite(0, x);
}
void display::turnOn() {
  digitalWrite(10, HIGH);
}
void display::turnOff() {
  digitalWrite(10, LOW);
}
void display::turn() {
  if (millis() - lastActivityTime > displayFade && millis() - lastActivityTime < displayTimeOut) {
    setBrightness(0.8);
  } else if (millis() - lastActivityTime > displayTimeOut) {
    displayNormal = false;
    setBrightness(1);
  } else {
    setBrightness(0.3);
  }
}