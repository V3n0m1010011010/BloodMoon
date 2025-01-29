#include "functions.h"
uint16_t getCenter(const char* string) {
  extern TFT_eSPI tft;
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  // tft.getTextBounds(string, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t center = (128 - textWidth) / 2;
  return center;
}