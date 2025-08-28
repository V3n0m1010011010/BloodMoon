#include <Arduino.h>
#include <esp_system.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <vector>
#include "Menu.h"
#include "display.h"
#include "controls.h"
#include "wifiApp.h"
#include "menues.h"
#include "globals.h"
#include "rf24App.h"
#include "settings.h"
#include "variables.h"
SPIClass spi(VSPI);
display dis(0, 22, width, height - 16);
controls ctr(select_btn, move_btn);
wifi Wifi;
nrf24 nRF24(CE_PIN, CSN_PIN, 16000000);

void setup() {
  initAll();
  delay(200);
}

void loop() {
  dis.turn();
  if(activem->getRenderState()){
    dis.renderAll();
  }
  ctr.handleInput();
}
