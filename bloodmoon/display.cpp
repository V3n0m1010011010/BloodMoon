#include <TFT_eSPI.h>
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
void display::renderBoot() {
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
void display::renderIconMenu() {
  setBrightness(0.3);
  activem->renderAll();
}
void display::renderMenu() {
  setBrightness(0.3);
  activem->renderAll();
}
void display::renderApScanMenu() {
  setBrightness(0.3);
  activem->renderHead();

  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.setTextSize(1);
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
    activem->handleInput();
    int y = startY + 10 + ((i - scrollOffset) * 10);
    tft.drawString(Wifi.getApList()[i].ssid, tft.width() / 2, y);
  }

  activem->renderFoot();
}
void display::renderStScanMenu() {
  activem->renderHead();

  activem->renderFoot();
}
void display::renderDeauthMenu() {
  activem->renderHead();

  activem->renderFoot();
}
void display::renderMonitorMenu() {
  activem->renderHead();

  activem->renderFoot();
}
void display::renderNRFJammer() {
  setBrightness(0.9);
  activem->renderHead();
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(1);
  tft.fillRect(startX, startY, endX, endY, TFT_BLACK);
  tft.drawString("Jamming...", startX + 10, startY + 10);
  tft.drawString("press sel to stop", startX + 10, startY + 25);
  activem->renderFoot();
}
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
  // Serial.print("turn() :: ");
  // Serial.print(lastActivityTime);
  // Serial.print(" :: ");
  // Serial.println(millis());
  if (millis() - lastActivityTime > displayFade && millis() - lastActivityTime < displayTimeOut) {
    setBrightness(0.7);
  } else if (millis() - lastActivityTime > displayTimeOut) {
    Serial.println("test---------------------------");
    setBrightness(1);
  } else {
    setBrightness(0.3);
  }
}