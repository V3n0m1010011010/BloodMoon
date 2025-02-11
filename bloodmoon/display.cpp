#include <TFT_eSPI.h>
#include <array>
#include <string>
#include <iomanip>
#include <sstream>
#include "display.h"
#include "wifi.h"
#include "rf24.h"
#include "menu.h"
#include "icons.h"
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
}
//------------------------------------------------------------------------------------------------
void display::renderBoot() {  // Render boot sequence
  tft.fillScreen(TFT_BLACK);
  tft.drawBitmap(tft.width() / 4, tft.height() / 4 - 10, ghost, 64, 64, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("BloodMoon", tft.width() / 2, 100);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Firmware", tft.width() / 2, 128 / 3);
  tft.drawString("by", tft.width() / 2, tft.height() / 2);
  tft.drawString("V3n0m1010011010", tft.width() / 2, (128 / 3) * 2);
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
  tft.drawString(activem->getTitle().c_str(), tft.width() / 2, 11);
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
    tft.drawString(String(currentIcon), tft.width() / 2, tft.height() / 2 - 8);
    tft.unloadFont();
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.drawString(section.c_str(), tft.width() / 2, tft.height() / 2 + 27);
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
      tft.fillRect(5, y, 118, 22, tft.color565(50, 0, 0));
      tft.drawRect(5, y, 118, 22, tft.color565(10, 0, 0));
    } else {
      tft.fillRect(5, y, 118, 22, tft.color565(80, 0, 0));
      tft.drawRect(5, y, 118, 22, tft.color565(10, 0, 0));
    }
    tft.drawString(activem->getSection(i).substr(0, maxTextWidth).c_str(), tft.width() / 2, y + 11);
  }
  if (sectionSize > maxVisibleItems && selectedIndex < sectionSize - 1) {
    if (currentIconsSet == 1) {
      tft.loadFont(customIcons1);
    } else if (currentIconsSet == 2) {
      tft.loadFont(customIcons2);
    }
    tft.drawString(String('\x24'), tft.width() / 2 - 4, 95);
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
    tft.fillRect(0, 22, 128, 90, TFT_BLACK);
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
      tft.drawString(String(currentIcon), posX, tft.height() / 2 - 8);
      tft.drawString(String(beforeIcon), posX - tft.width(), tft.height() / 2 - 8);
      tft.drawString(String(afterIcon), posX + tft.width(), tft.height() / 2 - 8);
      tft.unloadFont();
      tft.setTextSize(1);
      tft.setTextFont(2);
      tft.drawString(activem->getSection(indexCurrent).c_str(), posX, tft.height() / 2 + 27);
      tft.drawString(activem->getSection(indexBefore).c_str(), posX - tft.width(), tft.height() / 2 + 27);
      tft.drawString(activem->getSection(indexAfter).c_str(), posX + tft.width(), tft.height() / 2 + 27);
    }
    delay(5);
  }
  posX = tft.width() / 2;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderApScanMenu() {
  setBrightness(0.3);
  renderHead();
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
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
    tft.drawString(Wifi.getApList()[i].ssid.substring(0, maxTextWidth).c_str(), tft.width() / 2, y);
  }
  renderFoot();
}
void display::renderStScanMenu() {
  setBrightness(0.3);
  renderHead();
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
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
    tft.drawString(ss.str().c_str(), tft.width() / 2, y);
  }
  renderFoot();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void display::renderApSelectMenu() {
  renderHead();
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  int selectedIndex = activem->getSelectedIndex();
  if (selectedIndex == 0) {
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(activem->getSection(0).c_str(), posX, tft.height() / 2);
  } else if (Wifi.getFirstScan()[0]) {
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.setTextDatum(ML_DATUM);
    auto ap = Wifi.getApList()[selectedIndex];
    String ssid = ap.ssid;
    std::array<uint8_t, 6> bssid = ap.bssid;
    int channel = ap.channel;
    bool isSelected = ap.isSelected;
    tft.drawString(ssid, -(tft.width() / 2) + posX + 5, startY + 5);
  }
  renderFoot();
}
void display::renderStSelectMenu() {
}
//------------------------------------------------------------------------------------------------

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