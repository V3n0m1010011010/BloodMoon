#include <RF24.h>
#include <nRF24L01.h>
#include <algorithm>
#include <vector>
#include <string>
class nrf24 {
private:
  static nrf24* instance;
  bool radInit;
  static bool nRF24Jamming;
  std::vector<byte> hopping_channel = { 32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84, 86 };
public:
  RF24 rad;
  nrf24(int ce, int csn, long hrz);
  void init();
  bool getInit();
  void changeChannel(byte n);
  void startNRF24Jammer();
  void stopNRF24Jammer();
  static void jammer(void* param);
};
