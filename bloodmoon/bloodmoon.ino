#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Menu.h"
#include "display.h"
#include "wifi.h"
#include "attack.h"
#include "menues.h"
#include "globals.h"
#include "rf24.h"
#include "settings.h"
#include "variables.h"
SPIClass mySPI(VSPI);
display dis(0, 22, 128, 112);
wifi Wifi;
nrf24 nRF24(34, 33, 16000000);
attack Attack;

void setup() {
  initAll();
  delay(200);
}

void loop() {
  dis.turn();
  activem->handleInput();
}
