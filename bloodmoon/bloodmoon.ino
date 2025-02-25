#include <Arduino.h>
#include <esp_system.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <vector>
#include "Menu.h"
#include "display.h"
#include "controls.h"
#include "wifi.h"
#include "attack.h"
#include "menues.h"
#include "globals.h"
#include "rf24.h"
#include "settings.h"
#include "variables.h"
SPIClass mySPI(VSPI);
display dis(0, 22, 128, 112);
controls ctr(select, move);
wifi Wifi;
nrf24 nRF24(34, 33, 16000000);
attack Attack;

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
