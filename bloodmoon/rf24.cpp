#include <RF24.h>
#include <nRF24L01.h>
#include "rf24.h"
nrf24::nrf24(int ce, int csn, long hrz)
  : rad(ce, csn, hrz) {}
void nrf24::init() {
  Serial.println("nRF initilization...");
  radInit = false;
  if (rad.begin()) {
    radInit = true;
    Serial.println("nRF init success");
    rad.printDetails();
    rad.setDataRate(RF24_2MBPS);
    rad.setPALevel(RF24_PA_MAX, true);
    rad.stopListening();
  }
}
void nrf24::changeChannel(byte n) {
  Serial.print("Jamming, Channel: ");
  Serial.println(n);
  rad.setChannel(n);
}
bool nrf24::getInit(){
  return radInit;
}