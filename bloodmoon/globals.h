#pragma once
#include <TFT_eSPI.h>
#include "Menu.h"
#include "display.h"
#include "controls.h"
#include "wifiApp.h"
#include "rf24App.h"
#include "sdApp.h"
extern TFT_eSPI tft;
extern display dis;
extern controls ctr;
extern wifi Wifi;
extern nrf24 nRF24;
extern sdCard sd;
extern Menu menu;
static TaskHandle_t backgroundTask = NULL;

unsigned long lastActivityTime = 0;
const unsigned long displayFade = 10000;
const unsigned long displayTimeOut = 20000;
bool displayNormal = true;