#include <TFT_eSPI.h>
#include <array>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include "display.h"
#include "controls.h"
#include "wifi.h"
#include "rf24.h"
#include "menu.h"
#include "fonts/icons.h"
#include "settings.h"
#include "globals.h"
#include "variables.h"
#include "images/images.h"



extern TFT_eSPI tft;
extern Menu* activem;
extern controls ctr;
extern wifi Wifi;
extern nrf24 nRF24;



display::display(int startX, int startY, int endX, int endY)
  : startX(startX), startY(startY), endX(endX), endY(endY) {
  ledcSetup(pwmChannel, pwmFrequency, 8);
  ledcAttachPin(backlightPin, pwmChannel);
  posX = width / 2;
  #ifdef LILYGO_T_QT_PRO
    tft.setRotation(0);
  #elif defined(LILYGO_T_DISPLAY_S3_LCD)
    tft.setRotation(0);
  #endif
}





//------------------------------------------------------------------------------------------------
//                              Render boot sequence
//------------------------------------------------------------------------------------------------

void display::renderBoot() {
  int bootFrames = (int)map(height, 128, 320, 15, 30);
  for (int i = 0; i < bootFrames; i++) {
    int dy = -(height / bootFrames) * i;
    tft.fillScreen(TFT_BLACK);
    tft.drawBitmap(width / 2 - 32, height / 2 - 32 - 10 + dy, ghost, 64, 64, TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("BloodMoon", width / 2, height / 2 + 40 + dy);
    tft.drawString("Firmware", width / 2, height / 2 - 20 + height + dy);
    tft.drawString("by", width / 2, height / 2 + height + dy);
    tft.drawString("V3n0m1010011010", width / 2, height / 2 + 20 + height + dy);
    if (i == 0) delay(2000);
    else delay(20);
  }
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Firmware", width / 2, height / 2 - 20);
  tft.drawString("by", width / 2, height / 2);
  tft.drawString("V3n0m1010011010", width / 2, height / 2 + 20);
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
//                                   Render default menu head
//------------------------------------------------------------------------------------------------

void display::renderHead() {
  tft.setTextFont(0);
  tft.fillRect(0, 0, width, 22, tft.color565(161, 0, 0));
  tft.drawRect(0, 0, width, 22, tft.color565(255, 255, 255));
  tft.fillRect(5, 0, width - 5 * 2, 22, tft.color565(161, 0, 0));
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(activem->getTitle().c_str(), width / 2, 11);
}

//------------------------------------------------------------------------------------------------







//------------------------------------------------------------------------------------------------
//                                  Render default Main menu part
//------------------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------------------






//------------------------------------------------------------------------------------------------
//                              Render main menu with icons
//------------------------------------------------------------------------------------------------

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
    // tft.drawChar('<', 20, 62);
    // tft.setTextDatum(MR_DATUM);
    tft.drawString("<", width / 5, height / 2 + 8);
    // tft.setTextDatum(ML_DATUM);
    tft.drawString(">", (width / 5) * 4, height / 2 + 8);
    // tft.drawChar('>', 128 - 25, 62);
  }
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                              Render default menu foot
//------------------------------------------------------------------------------------------------

void display::renderFoot() {
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextDatum(ML_DATUM);
  tft.fillRect(0, height - 16, width, 16, tft.color565(161, 0, 0));
  tft.drawString("Sel", 5, height - 8);
  tft.drawString("Mov", width - 30, height - 8);
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                    Render all menu parts according to menu type
//------------------------------------------------------------------------------------------------

void display::renderAll() {
  activem->setRenderState(false);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  renderHead();
  renderFoot();
  String type = activem->getType();
  if ("normal") {
    if (lastSelectedIndex == activem->getSelectedIndex()) {
      if (activem->isMenuWithIcon()) {
        renderMainWithIcon();
      } else {
        renderMain();
      }
    } else {
      if (activem->isMenuWithIcon()) {
        renderIconScrollAnimation(scrollDirection, 20);
      }
      lastSelectedIndex = activem->getSelectedIndex();
      activem->setRenderState(true);
    }
  } else if (type == "scanAp") {
    renderApScanMenu();
  } else if (type == "scanSt") {
    renderStScanMenu();
  } else if (type == "jammer") {
    renderNRFJammer();
  } else if (type == "apSelect") {
    renderApSelectMenu();
  } else if (type == "stSelect") {
    renderStSelectMenu();
  }
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                        Render Scroll animation for menu with icons
//------------------------------------------------------------------------------------------------

void display::renderIconScrollAnimation(bool dir, int frames) {
  int selectedIndex = lastSelectedIndex;
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < frames; i++) {
    int dx = dir ? width / frames : -(width / frames);
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
//                              Render scan sequenze for aps
//------------------------------------------------------------------------------------------------

void display::renderApScanMenu() {
  setBrightness(0.3);
  tft.fillRect(startX, startY, endX, endY - startY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(1);
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
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                              Render scan sequenze for sts
//------------------------------------------------------------------------------------------------

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
//                              Render select menu for aps
//------------------------------------------------------------------------------------------------

void display::renderApSelectMenu() {
  int selectedIndex = activem->getSelectedIndex();
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(2);
  renderHead();
  renderFoot();
  if (selectedIndex == 0) {
    tft.fillRoundRect(20, height / 2 - 11, 88, 22, 5, tft.color565(80, 0, 0));
    tft.setTextDatum(MC_DATUM);
    tft.drawString(activem->getSection(0).c_str(), posX, height / 2);
  } else if (Wifi.getFirstScan()[0]) {
    const int maxTextWidth = 6;
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
    tft.fillRoundRect(startX + 5, startY + 5, endX - 10, endY - startY - 10, 5, tft.color565(80, 0, 0));
    tft.fillRoundRect(startX + 10, startY + 10, endX - 20, 20, 5, tft.color565(40, 0, 0));
    tft.setTextDatum(ML_DATUM);
    tft.drawString("SSID:", 18, startY + 20);
    tft.drawString(ssid.c_str(), 55, startY + 20);
    while (!ctr.handleInput()) {
    }
    // tft.drawString(bssidString.str().c_str(), -(width / 2) + posX + 10, startY + 30);
  }
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                              Render ap select scroll animation
//------------------------------------------------------------------------------------------------

void display::renderApSelectMenuScrollAni() {
  static double lastRenderTime = 0;
  const double renderDelay = 300;
  double now = millis();
  int indexSSID = 0;
  if (now - lastRenderTime > renderDelay) {
    lastRenderTime = now;
    // tft.fillRoundRect(startX + 10, startY + 10, endX - 20, 20, 5, tft.color565(40, 0, 0));
    // tft.setTextDatum(ML_DATUM);
    // tft.drawString("SSID:", 18, startY + 20);
    // tft.drawString(ssid.c_str(), 55, startY + 20);
  }
}
//------------------------------------------------------------------------------------------------






//------------------------------------------------------------------------------------------------
//                              Render select menu for sts
//------------------------------------------------------------------------------------------------

void display::renderStSelectMenu() {
}

//------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------
//                          Render scroll animation for wifi select menu
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
    int dx = dir ? width / frames : -(width / frames);
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
          tft.fillCircle(xPos - boxWidth / 2, height / 2, 20, tft.color565(80, 0, 0));
        }
      }
    }
    delay(30);
  }
  int pos = 5;
  // for (int j = 0; j < 5; j++) {
  //   tft.fillRect(0, startY, endX, endY - startY, TFT_BLACK);
  //   tft.drawRoundRect()
  // }
  if (selectedIndex != 0) {
    int startRadius = 20;
    int framesAfter = 10;
    int endWidth = 118, endHeight = endY - startY - 10, endCornerRadius = 5;
    for (int i = 0; i < framesAfter; i++) {
      float progress = (float)i / (framesAfter - 1);
      int widthRect = (int)map(i, 0, framesAfter, 20, endWidth);
      int heightRect = (int)map(i, 0, framesAfter, 20, endHeight);
      int cornerRadius = (int)map(i, 0, framesAfter, 20, 5);
      int x = width / 2 - widthRect / 2;
      int y = height / 2 - heightRect / 2;
      tft.fillRect(0, startY, endX, endY - startY, TFT_BLACK);
      tft.drawRoundRect(x, y, width, height, cornerRadius, tft.color565(80, 0, 0));
      delay(100);
    }
  }
  posX = width / 2;
}

//------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------
//                              Render deauthing sequenze
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

void display::renderNRFError() {
  tft.fillRoundRect(width / 2 - 60, height / 2 - 10, 60 * 2, 20, 5, tft.color565(80, 0, 0));
  tft.setTextDatum(MC_DATUM);
  tft.drawString("nRF24 not found", width / 2, height / 2);
  delay(1000);
  renderAll();
}

//------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------
//                              Render packet-monitor
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
  ledcAttachPin(backlightPin, pwmChannel);
  ledcWrite(0, 0.3);
}
void display::turnOff() {
  ledcDetachPin(backlightPin);
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);
}
void display::turn() {
  if (millis() - lastActivityTime > displayFade && millis() - lastActivityTime < displayTimeOut) {
    setBrightness(0.8);
  } else if (millis() - lastActivityTime > displayTimeOut) {
    displayNormal = false;
    // setBrightness(1);
    turnOff();
  } else {
    turnOn();
  }
}


String display::stringShifter(const String& input, int offset, int maxWidth) {
  if (input.length() <= maxWidth) return input;
  String result;
  int effectiveLength = input.length() + 3;
  int virtualPos = offset % effectiveLength;
  if (virtualPos <= input.length()) {
    int endPos = virtualPos + maxWidth;
    if (endPos > input.length()) {
      result = input.substring(virtualPos);
      result += "...";
      result = result.substring(0, maxWidth);
    } else {
      result = input.substring(virtualPos, endPos);
      if (endPos < input.length()) result += "...";
    }
  } else {
    result = input.substring(0, maxWidth);
  }
  return result;
}