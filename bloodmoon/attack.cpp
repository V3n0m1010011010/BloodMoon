#include <TFT_eSPI.h>
#include "attack.h"
#include "globals.h"
#include "rf24.h"
#include "Menu.h"
#include "display.h"
#include "wifi.h"
#include "controls.h"
extern nrf24 nRF24;
extern display dis;
extern controls ctr;
extern wifi Wifi;

void attack::deauth() {
  deauthing = true;
  while(deauthing){

  }
}
void attack::nRF24Jammer() {
  nRF24Jamming = true;
  nRF24.rad.startConstCarrier(RF24_PA_MAX, 45);
  while (nRF24Jamming) {
    for (int j = 0; j < sizeof(hopping_channel); j++) {
      nRF24.changeChannel(hopping_channel[j]);
      dis.turn();
      if (ctr.handleInput()) {
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