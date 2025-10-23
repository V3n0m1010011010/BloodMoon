#include <SD.h>
#include <SPI.h>

#include "globals.h"

void sdCard::init(int csPin, SPIClass &spi){
  Serial.println("sd initilization...");
  sdInit = false;
  if (SD.begin(csPin, spi)) {
    sdInit = true;
    Serial.println("sd init success");
  }else{
      Serial.println("sd init failed");
  }
}

void sdCard::createFile(String source){
  File f = SD.open(source, FILE_WRITE);
  if (f) {
    f.close();
  }
}

void sdCard::writeToFile(String dest, const uint8_t* byte, int length, bool next){
  File f = SD.open(dest, FILE_APPEND);
  if(f){
    f.write(byte, length);
    f.flush();
    if(!next)f.close();
  }
}

bool sdCard::fileExists(String source){
  return SD.exists(source);
}

void sdCard::deleteFile(String source){
  SD.remove(source);
}

bool sdCard::getInit(){
  return sdInit;
}