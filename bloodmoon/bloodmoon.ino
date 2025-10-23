#include <Arduino.h>
#include <SPI.h>
#include "globals.h"
#include "settings.h"
SPIClass spi(VSPI);
TFT_eSPI tft = TFT_eSPI();
display dis(0, 22, width, height - 16);
controls ctr(select_btn, move_btn);
nrf24 nRF24(CE_PIN, CSN_PIN, 16000000);
sdCard sd;
wifi Wifi;
Menu menu;

void setup() {
  tft.init();
  Serial.begin(115200);
  spi.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
  ctr.init();
  nRF24.init();
  sd.init(sd_CS_PIN, spi);
  Wifi.init();
  menu.init();
  delay(200);
  pinMode(backlightPin, OUTPUT);
  dis.renderBoot();
  menu.activem.needsRender = true;
  lastActivityTime = millis();
  delay(200);
}

void loop() {
  dis.turn();
  if(menu.activem.needsRender){
    dis.renderAll();
  }
  ctr.handleInput();
}
