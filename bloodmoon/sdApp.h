#include <SD.h>
#include <SPI.h>
#include <algorithm>
#include <map>
#include <string>

class sdCard{
private:
  bool sdInit;

public:

  void init(int csPin, SPIClass &spi);

  void createFile(String source);
  void writeToFile(String name, const uint8_t* byte, int length, bool next);

  bool fileExists(String source);
  void deleteFile(String source);

  bool getInit();

};