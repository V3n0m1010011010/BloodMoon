#include <RF24.h>
#include <nRF24L01.h>
class nrf24 {
private:
  bool radInit;
public:
  RF24 rad;
  nrf24(int ce, int csn, long hrz);
  void init();
  void changeChannel(byte n);
  bool getInit();
};
