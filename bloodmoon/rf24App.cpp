#include <TFT_eSPI.h>
#include "globals.h"
#include <RF24.h>
#include <nRF24L01.h>
#include "rf24App.h"
#include "Menu.h"
#include "display.h"
#include "controls.h"

extern controls ctr;
extern display dis;


nrf24* nrf24::instance = nullptr;
bool nrf24::nRF24Jamming = false;

nrf24::nrf24(int ce, int csn, long hrz)
  : rad(ce, csn, hrz) {}
void nrf24::init() {
  Serial.println("nRF initilization...");
  radInit = false;
  if (rad.begin()) {
    instance = this;
    radInit = true;
    Serial.println("nRF init success");
    rad.printDetails();
    rad.setDataRate(RF24_2MBPS);
    rad.setPALevel(RF24_PA_MAX, true);
    rad.stopListening();
  }
}
bool nrf24::getInit(){
  return radInit;
}

void nrf24::changeChannel(byte n) {
  Serial.print("Jamming, Channel: ");
  Serial.println(n);
  rad.setChannel(n); 
}

void nrf24::startNRF24Jammer(){
  if (backgroundTask == NULL) {
    nRF24Jamming = true;
    xTaskCreatePinnedToCore(
      jammer,
      "DeauthTask",
      4096,
      NULL,
      1,
      &backgroundTask,
      1
    );
  }
}
void nrf24::stopNRF24Jammer(){
  nRF24Jamming = false;
}

void nrf24::jammer(void* param){
  instance->rad.startConstCarrier(RF24_PA_MAX, 45);
  while (nRF24Jamming) {
    for (int j = 0; j < sizeof(hopping_channel); j++) {
      instance->changeChannel(instance->hopping_channel[j]);
    }
  }
}