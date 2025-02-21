#include <WiFi.h>
#include <algorithm>
#include <vector>
#include <string>

class attack {
private:
  bool nRF24Jamming = false;
  bool deauthing = false;
  std::vector<byte> hopping_channel = { 32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84, 86 };
public:
  void deauth();
  void nRF24Jammer();
  void setMode(int mode, bool status);
  bool getMode(int mode);
};