#include <TFT_eSPI.h>
#include <array>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include "display.h"
#include "controls.h"
#include "wifiApp.h"
#include "rf24App.h"
#include "menu.h"
#include "fonts/icons.h"
#include "settings.h"
#include "globals.h"
#include "variables.h"
#include "images/images.h"

extern TFT_eSPI tft;
extern Menu *activem;
extern controls ctr;
extern wifi Wifi;
extern nrf24 nRF24;

display::display(int startX, int startY, int endX, int endY)
    : startX(startX), startY(startY), endX(endX), endY(endY)
{
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

void display::renderBoot()
{
  int bootFrames = (int)map(height, 128, 320, 15, 30);
  for (int i = 0; i < bootFrames; i++)
  {
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
    if (i == 0)
      delay(2000);
    else
      delay(20);
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
  for (int i = 0; i < 3; i++)
  {
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

void display::renderHead()
{
  tft.setTextFont(0);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.fillRect(0, 0, width, 22, tft.color565(161, 0, 0));
  tft.drawRect(0, 0, width, 22, tft.color565(255, 255, 255));
  tft.fillRect(5, 0, width - 5 * 2, 22, tft.color565(161, 0, 0));
  tft.drawString(activem->getTitle().c_str(), width / 2, 11);
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                                  Render default Main menu part
//------------------------------------------------------------------------------------------------

void display::renderMain()
{
  int selectedIndex = activem->getSelectedIndex();
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  const int maxVisibleItems = maxItems(25);
  Serial.println(maxVisibleItems);
  const int maxTextWidth = getMaxChars("2");
  static int scrollOffset = 0;
  if (selectedIndex < scrollOffset)
  {
    scrollOffset = selectedIndex;
  }
  else if (selectedIndex >= scrollOffset + maxVisibleItems)
  {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, sectionSize); ++i)
  {
    int y = 25 + ((i - scrollOffset) * 22);
    if (i == selectedIndex)
    {
      tft.fillRect(5, y, width - 10, 22, tft.color565(50, 0, 0));
      tft.drawRect(5, y, width - 10, 22, tft.color565(10, 0, 0));
    }
    else
    {
      tft.fillRect(5, y, width - 10, 22, tft.color565(80, 0, 0));
      tft.drawRect(5, y, width - 10, 22, tft.color565(10, 0, 0));
    }
    tft.drawString(activem->getSection(i).substr(0, maxTextWidth).c_str(), width / 2, y + 11);
  }
  if (sectionSize > maxVisibleItems && selectedIndex < sectionSize - 1)
  {
    tft.loadFont(customIcons1);
    tft.drawString(String('\x24'), width / 2, endY - 13);
    tft.unloadFont();
    tft.setTextFont(2);
  }
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render main menu with icons
//------------------------------------------------------------------------------------------------

void display::renderMainWithIcon()
{
  int selectedIndex = activem->getSelectedIndex();
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  if (selectedIndex >= 0 && selectedIndex < iconSize)
  {
    char currentIcon = activem->getIcon(selectedIndex);
    tft.loadFont(customIcons1);
    tft.drawString(String(currentIcon), width / 2, height / 2 - 8);
    tft.unloadFont();
    tft.setTextSize(1);
    tft.setTextFont(2);
    tft.drawString(section.c_str(), width / 2, height / 2 + 27);
    tft.drawString("<", width / 5, height / 2 + 8);
    tft.drawString(">", (width / 5) * 4, height / 2 + 8);
  }
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render default menu foot
//------------------------------------------------------------------------------------------------

void display::renderFoot()
{
  int selectedIndex = activem->getSelectedIndex();
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.fillRect(0, height - 16, width, 16, tft.color565(161, 0, 0));
  tft.setTextDatum(ML_DATUM);
  tft.drawString("Sel", 5, height - 8);
  tft.drawString(">", width / 2 + 5, height - 8);

  tft.setTextDatum(MR_DATUM);
  tft.drawString("Mov", width - 5, height - 8);
  tft.drawString("<", width / 2 - 5, height - 8);
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                    Render all menu parts according to menu type
//------------------------------------------------------------------------------------------------

void display::renderAll()
{
  tft.fillScreen(TFT_BLACK);
  activem->setRenderState(false);
  tft.setTextColor(TFT_WHITE);
  renderHead();
  renderFoot();

  String type = activem->getType();
  if (type == "normal")
  {
    if (lastSelectedIndex == activem->getSelectedIndex())
    {
      if (activem->isMenuWithIcon())
      {
        renderMainWithIcon();
      }
      else
      {
        renderMain();
      }
    }
    else
    {
      if (activem->isMenuWithIcon())
      {
        renderIconScrollAnimation(scrollDirection, 20);
      }
      lastSelectedIndex = activem->getSelectedIndex();
      activem->setRenderState(true);
    }
  }
  else if (type == "scanAp")
  {
    renderApScanMenu();
  }
  else if (type == "scanSt")
  {
    renderStScanMenu();
  }
  else if (type == "jammer")
  {
    renderNRFJammer();
  }
  else if (type == "apSelect")
  {
    if (lastSelectedIndex == activem->getSelectedIndex())
    {
      renderAp();
    }
    else
    {
      if ((lastSelectedIndex == 3 && activem->getSelectedIndex() == 4) ||
          (lastSelectedIndex == 4 && activem->getSelectedIndex() == 3))
      {
        renderAp();
      }
      else
      {
        renderApScrollAnimation(scrollDirection, 10);
      }
      lastSelectedIndex = activem->getSelectedIndex();
      activem->setRenderState(true);
    }
  }
  else if (type == "stSelect")
  {
    renderStSelectMenu();
  }
  else if (type == "deauther")
  {
    if (lastSelectedIndex == activem->getSelectedIndex())
    {
      renderDeauther();
    }
    else
    {
      renderDeautherScrollAnimation(scrollDirection, 10);
      lastSelectedIndex = activem->getSelectedIndex();
      activem->setRenderState(true);
    }
  }
  // tft.drawLine(width / 2, 0, width / 2, height, tft.color565(255,255,255));
  // tft.drawLine(0, height / 2, width, height / 2, tft.color565(255,255,255));
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                        Render Scroll animation for menu with icons
//------------------------------------------------------------------------------------------------

void display::renderIconScrollAnimation(bool dir, int frames)
{
  posX = width / 2;
  int selectedIndex = lastSelectedIndex;
  std::string section = activem->getSection(selectedIndex);
  int sectionSize = activem->getSectionsSize();
  int iconSize = activem->getIconsSize();
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < frames; i++)
  {
    int dx = dir ? width / frames : -(width / frames);
    posX -= dx;
    tft.fillRect(0, startY, endX, endY - startY, TFT_BLACK);
    if (selectedIndex >= 0 && selectedIndex < iconSize)
    {
      int indexBefore = (selectedIndex - 1 < 0) ? iconSize - 1 : selectedIndex - 1;
      int indexCurrent = selectedIndex;
      int indexAfter = (selectedIndex + 1 > iconSize - 1) ? 0 : selectedIndex + 1;
      char beforeIcon = activem->getIcon(indexBefore);
      char currentIcon = activem->getIcon(selectedIndex);
      char afterIcon = activem->getIcon(indexAfter);
      tft.setTextSize(3);
      tft.loadFont(customIcons1);
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
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render scan sequenze for aps
//------------------------------------------------------------------------------------------------

void display::renderApScanMenu()
{
  setBrightness(0.3);
  tft.fillRect(startX, startY, endX, endY - startY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(1);
  tft.setTextDatum(MC_DATUM);
  const int maxVisibleItems = maxItems(10) - 1;
  const int maxTextWidth = 15;
  static int scrollOffset = 0;
  int selectedIndex = Wifi.getApList().size();
  if (selectedIndex < scrollOffset)
  {
    scrollOffset = selectedIndex;
  }
  else if (selectedIndex >= scrollOffset + maxVisibleItems)
  {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, (int)Wifi.getApList().size()); ++i)
  {
    int y = startY + 10 + ((i - scrollOffset) * 10);
    tft.drawString(Wifi.getApList()[i].ssid.substring(0, maxTextWidth).c_str(), width / 2, y);
  }
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render scan sequenze for sts
//------------------------------------------------------------------------------------------------

void display::renderStScanMenu()
{
  setBrightness(0.3);
  tft.fillRect(startX, startY, endX, endY - startY, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(1);
  tft.setTextDatum(MC_DATUM);
  const int maxVisibleItems = maxItems(10) - 1;
  const int maxTextWidth = 15;
  static int scrollOffset = 0;
  int selectedIndex = Wifi.getStList().size();
  if (selectedIndex < scrollOffset)
  {
    scrollOffset = selectedIndex;
  }
  else if (selectedIndex >= scrollOffset + maxVisibleItems)
  {
    scrollOffset = selectedIndex - maxVisibleItems + 1;
  }
  for (int i = scrollOffset; i < std::min(scrollOffset + maxVisibleItems, (int)Wifi.getStList().size()); ++i)
  {
    int y = startY + 10 + ((i - scrollOffset) * 10);
    std::array<uint8_t, 6> mac = Wifi.getStList()[i].mac;
    tft.drawString(macToString(mac), width / 2, y);
  }
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render select menu for aps
//------------------------------------------------------------------------------------------------

void display::renderAp()
{
  int selectedIndex = activem->getSelectedIndex();
  TFT_eSprite sprite = TFT_eSprite(&tft);
  sprite.createSprite(endX - startX, endY - startY);
  sprite.fillSprite(TFT_BLACK);
  drawApSections(sprite, selectedIndex, 0, 0);
  tft.startWrite();
  sprite.pushSprite(startX, startY);
  tft.endWrite();
  sprite.deleteSprite();
}

void display::renderApScrollAnimation(bool dir, int frames)
{
  int oldIndex = lastSelectedIndex;
  int newIndex = activem->getSelectedIndex();
  if ((oldIndex == 3 && newIndex == 4) || (oldIndex == 4 && newIndex == 3))
  {
    renderAp();
    return;
  }

  TFT_eSprite sprite = TFT_eSprite(&tft);
  sprite.createSprite(endX - startX, endY - startY);

  int startXOld = 0;
  int startXNew = dir ? width : -width;

  for (int i = 0; i < frames; i++)
  {
    int dx = dir ? (width / frames) : -(width / frames);
    startXOld -= dx;
    startXNew -= dx;

    sprite.fillSprite(TFT_BLACK);
    drawApSections(sprite, oldIndex, startXOld, 0);
    drawApSections(sprite, newIndex, startXNew, 0);
    tft.startWrite();
    sprite.pushSprite(startX, startY);
    tft.endWrite();

    delay(10);
  }
  sprite.deleteSprite();
  renderAp();
}

void display::drawApSections(TFT_eSprite &sprite, int index, int x, int y)
{
  sprite.setTextWrap(false);
  int w = endX - startX;
  int h = endY - startY;

  sprite.setTextColor(TFT_WHITE);
  if (index == 0)
  {
    sprite.setTextSize(1);
    sprite.setTextFont(2);
    sprite.setTextDatum(MC_DATUM);
    sprite.fillRoundRect(x + 20, h / 2 - 11, 88, 22, 5, tft.color565(80, 0, 0));
    sprite.drawString(activem->getSection(0).c_str(), x + w / 2, h / 2);
  }
  else if (Wifi.getFirstScan()[0])
  {
    sprite.setTextSize(1);
    sprite.setTextFont(1);
    sprite.fillRoundRect(x + 3, 3, w - 5, h - 6, 5, tft.color565(80, 0, 0));
    int maxChars = getMaxChars("1");
    auto ap = Wifi.getApList()[selectedAp];
    String ssid = ap.ssid;
    String bssid = macToString(ap.bssid);
    int channel = ap.channel;
    int rssi = ap.rssi;
    String encryption = ap.encryption;
    bool isDeauthSelected = ap.isDeauthSelected;
    bool isBecCloneSelected = ap.isBecCloneSelected;

    int windowHeight = h - 14;
    int fontH = tft.fontHeight();

    sprite.fillRoundRect(x + 7, y + 7, w - 13, windowHeight, 5, tft.color565(40, 0, 0));
    sprite.setTextDatum(ML_DATUM);
    if (index == 1)
    {
      sprite.drawString("SSID: ", x + 13, y + getY(windowHeight, 7, 2, 0, true, false, 0));
      sprite.drawString(subString(ssid, getMaxChars("1")), x + 13, y + getY(windowHeight, 7, 2, tft.fontHeight(), true, true, 0));
      sprite.drawLine(x + 7, y + 7 + (windowHeight / 2), x + w - 7, y + 7 + (windowHeight / 2), TFT_WHITE);
      sprite.drawString("BSSID: ", x + 13, y + getY(windowHeight, 7, 2, 0, true, false, 1));
      sprite.drawString(bssid, x + 13, y + getY(windowHeight, 7, 2, tft.fontHeight(), true, true, 1));
    }
    else if (index == 2)
    {
      sprite.drawString("CH: " + String(channel), x + 13, y + getY(windowHeight, 7, 3, tft.fontHeight(), false, false, 0));
      sprite.drawLine(x + 7, y + 7 + (windowHeight / 3), x + w - 7, y + 7 + (windowHeight / 3), TFT_WHITE);
      sprite.drawString("RSSI: " + String(rssi), x + 13, y + getY(windowHeight, 7, 3, tft.fontHeight(), false, false, 1));
      sprite.drawLine(x + 7, y + 7 + (windowHeight / 3) * 2, x + w - 7, y + 7 + (windowHeight / 3) * 2, TFT_WHITE);
      sprite.drawString("WPA: " + encryption, x + 13, y + getY(windowHeight, 7, 3, tft.fontHeight(), false, false, 2));
    }
    else if (index == 3 || index == 4)
    {
      sprite.drawString("deauthing:", x + startX + 13, getY(windowHeight, 7, 2, 0, false, false, 0));
      if (isDeauthSelected)
      {
        sprite.loadFont(customIcons1);
        sprite.setTextDatum(MC_DATUM);
        int fontHc = sprite.fontHeight();
        sprite.drawString(")", x + w - 34, getY(windowHeight, 7, 2, 0, false, false, 0));
        sprite.unloadFont();
        sprite.setTextDatum(ML_DATUM);
      }
      else
      {
        sprite.loadFont(customIcons1);
        sprite.setTextDatum(MC_DATUM);
        int fontHc = sprite.fontHeight();
        sprite.drawString("(", x + endX - 34, getY(windowHeight, 7, 2, 0, false, false, 0));
        sprite.unloadFont();
        sprite.setTextDatum(ML_DATUM);
      }
      sprite.drawLine(x + startX + 7, (h / 2), x + endX - 7, (h / 2), tft.color565(255, 255, 255));

      sprite.setTextColor(tft.color565(50, 50, 50));
      sprite.drawString("beaconclone:", x + startX + 13, getY(windowHeight, 7, 2, 0, false, false, 1));
      sprite.setTextColor(TFT_WHITE);
    }
  }
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render select menu for sts
//------------------------------------------------------------------------------------------------

void display::renderStSelectMenu()
{
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render deauthing sequenze
//------------------------------------------------------------------------------------------------

void display::renderDeauther()
{
  int selectedIndex = activem->getSelectedIndex();
  TFT_eSprite sprite = TFT_eSprite(&tft);
  sprite.createSprite(endX - startX, endY - startY);
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextColor(TFT_WHITE);
  drawDeautherSections(sprite, selectedIndex, 0);
  tft.startWrite();
  sprite.pushSprite(startX, startY);
  tft.endWrite();
  sprite.deleteSprite();
}

void display::drawDeautherSections(TFT_eSprite &sprite, int index, int x)
{
  sprite.setTextWrap(false);
  int w = endX - startX;
  int h = endY - startY;
  sprite.setTextColor(TFT_WHITE);
  // sprite.fillRect(startX, startY, endX, endY, TFT_BLACK);

  if (index == 0)
  {
    sprite.setTextSize(1);
    sprite.setTextFont(2);
    sprite.setTextDatum(MC_DATUM);
    sprite.fillRoundRect(x + 20, h / 2 - 11, 88, 22, 5, tft.color565(80, 0, 0));
    sprite.drawString(activem->getSection(0).c_str(), x + w / 2, h / 2);
  }
  else
  {
    int windowHeight = endY - startY - 14;
    sprite.fillRoundRect(x + 3, 3, w - 5, h - 6, 5, tft.color565(80, 0, 0));

    if (!Wifi.isDeauthing())
    {
      
      int aps = Wifi.getApDeauthSelects();
      int sts = 0;
      sprite.fillRoundRect(x + 7, 7, w - 13, windowHeight / 2, 5, tft.color565(40, 0, 0));
      sprite.setTextSize(1);
      sprite.setTextFont(1);
      sprite.setTextDatum(ML_DATUM);
      sprite.drawString("Aps selected: " + String(aps), x + 13, getY(windowHeight, 7, 2, 0, true, false, 0));
      sprite.drawString("Sts selected: " + String(sts), x + 13, getY(windowHeight, 7, 2, 0, true, true, 0));
      sprite.setTextSize(1);
      sprite.setTextFont(2);
      sprite.setTextDatum(MC_DATUM);
      int textWidth = sprite.textWidth("Start");
      int textHeight = sprite.fontHeight();
      int boxX = w / 2 - textWidth / 2 - 5;
      sprite.fillRoundRect(x + boxX, getY(windowHeight, 7, 2, 0, false, false, 1) - textHeight / 2 - 3, textWidth + 10, textHeight + 6, 5, tft.color565(40, 0, 0));
      sprite.drawString("Start", x + w / 2, getY(windowHeight, 7, 2, 0, false, false, 1));
    }
    else
    {
      sprite.setTextSize(1);
      sprite.setTextFont(2);
      sprite.setTextDatum(MC_DATUM);
      int textWidth = sprite.textWidth("Stop");
      int textHeight = sprite.fontHeight();
      int boxX = w / 2 - textWidth / 2 - 5;
      sprite.fillRoundRect(x + boxX, getY(windowHeight, 7, 2, 0, false, false, 1) - textHeight / 2 - 3, textWidth + 10, textHeight + 6, 5, tft.color565(40, 0, 0));
      sprite.drawString("Stop", x + w / 2, getY(windowHeight, 7, 2, 0, false, false, 1));
      
      sprite.setTextColor(TFT_BLACK);
      sprite.loadFont(customIcons1);
      int textW = sprite.textWidth("'");
      sprite.drawString("'", x + w / 2 + 3, getY(windowHeight, 7, 2, 0, false, false, 0) + 3);
      sprite.setTextColor(TFT_WHITE);
      sprite.drawString("'", x + w / 2, getY(windowHeight, 7, 2, 0, false, false, 0));
      sprite.unloadFont();
    }
  }
}

void display::renderDeautherScrollAnimation(bool dir, int frames)
{
  int oldIndex = lastSelectedIndex;
  int newIndex = activem->getSelectedIndex();

  TFT_eSprite sprite = TFT_eSprite(&tft);
  sprite.createSprite(endX - startX, endY - startY);

  int startXOld = 0;
  int startXNew = dir ? width : -width;

  for (int i = 0; i < frames; i++)
  {
    int dx = dir ? (width / frames) : -(width / frames);
    startXOld -= dx;
    startXNew -= dx;

    sprite.fillSprite(TFT_BLACK);
    sprite.setTextColor(TFT_WHITE);
    drawDeautherSections(sprite, oldIndex, startXOld);
    drawDeautherSections(sprite, newIndex, startXNew);
    tft.startWrite();
    sprite.pushSprite(startX, startY);
    tft.endWrite();

    delay(10);
  }
  sprite.deleteSprite();
  renderDeauther();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render Jamming sequenze
//------------------------------------------------------------------------------------------------

void display::renderNRFJammer()
{
  setBrightness(0.9);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.drawString("Jamming...", startX + 10, startY + 10);
  tft.drawString("press sel to stop", startX + 10, startY + 25);
}

void display::renderNRFError()
{
  tft.fillRoundRect(width / 2 - 60, height / 2 - 10, 60 * 2, 20, 5, tft.color565(80, 0, 0));
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.drawString("nRF24 not found", width / 2, height / 2);
  delay(1000);
  renderAll();
}

//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
//                              Render packet-monitor
//------------------------------------------------------------------------------------------------

void display::renderMonitorMenu()
{
}

//------------------------------------------------------------------------------------------------

void display::setBrightness(float brightness)
{
  long x = brightness * 255;
  ledcWrite(0, x);
}
void display::turnOn()
{
  ledcAttachPin(backlightPin, pwmChannel);
  ledcWrite(0, 0.3);
}
void display::turnOff()
{
  ledcDetachPin(backlightPin);
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);
}
void display::turn()
{
  if (millis() - lastActivityTime > displayFade && millis() - lastActivityTime < displayTimeOut)
  {
    setBrightness(0.8);
  }
  else if (millis() - lastActivityTime > displayTimeOut)
  {
    displayNormal = false;
    // setBrightness(1);
    turnOff();
  }
  else
  {
    turnOn();
  }
}

String display::stringShifter(const String &input, int index, int maxWidth)
{
  if (input.length() <= maxWidth)
    return input;
  int virtualPos = index % input.length();
  String result;
  if (virtualPos + maxWidth <= input.length())
  {
    result = input.substring(virtualPos, virtualPos + maxWidth);
  }
  else
  {
    result = input.substring(virtualPos);
    result += input.substring(0, (virtualPos + maxWidth) % input.length());
  }
  return result;
}

int display::maxItems(int itemHeight)
{
  return (int)((endY - startY) / itemHeight);
}

String display::macToString(std::array<uint8_t, 6> mac)
{
  std::stringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0');
  for (size_t i = 0; i < mac.size(); ++i)
  {
    ss << std::setw(2) << static_cast<int>(mac[i]);
    if (i < mac.size() - 1)
    {
      ss << ":";
    }
  }
  return ss.str().c_str();
}

String display::subString(const String &input, int maxLength)
{
  if (input.length() <= maxLength)
    return input;
  return input.substring(0, maxLength - 3) + "...";
}

int display::getY(int screenHeight, int start, int rows, int fontHeight, bool isSplit, bool isSub, int rowIndex)
{
  float blockHeight = (float)screenHeight / rows;
  float blockTop = blockHeight * rowIndex;
  float padding = 0;
  if (isSplit)
  {
    if (isSub)
    {
      padding = blockHeight / 2.0f + 7;
    }
    else
    {
      padding = blockHeight / 2.0f - 7 - fontHeight;
    }
  }
  else
  {
    padding = blockHeight / 2.0f;
  }
  return (int)round(start + blockTop + padding);
}
int display::getMaxChars(const String &font)
{
  auto it = fontMaxChars.find(font);
  if (it != fontMaxChars.end())
  {
    return it->second;
  }
  return 0;
}