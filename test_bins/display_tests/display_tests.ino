#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
void setup() {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Display test", 5, 5);
}
void loop() {
}
