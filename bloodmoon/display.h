#include <TFT_eSPI.h>

class display {
private:
  unsigned int startX;
  unsigned int startY;
  unsigned int endX;
  unsigned int endY;
public:
  display(int startX, int startY, int endX, int endY);
  virtual void renderBoot();
  virtual void renderIconMenu();
  virtual void renderMenu();
  virtual void renderApScanMenu();
  virtual void renderStScanMenu();
  virtual void renderDeauthMenu();
  virtual void renderMonitorMenu();
  virtual void renderNRFJammer();
  void setBrightness(float brightness);
  void turnOn();
  void turnOff();
  void turn();
};