#include <TFT_eSPI.h>
extern TFT_eSPI tft;
class display {
private:
  unsigned int startX;
  unsigned int startY;
  unsigned int endX;
  unsigned int endY;
  int posX;
public:
  display(int startX, int startY, int endX, int endY);
  virtual void renderBoot();

  virtual void renderHead();
  virtual void renderMainWithIcon();
  virtual void renderMain();
  virtual void renderFoot();
  virtual void renderAll();

  virtual void renderIconScrollAnimation(bool dir, int frames);

  virtual void renderApScanMenu();
  virtual void renderStScanMenu();
  virtual void renderApSelectMenu();
  virtual void renderStSelectMenu();
  virtual void renderDeauthMenu();
  virtual void renderMonitorMenu();
  virtual void renderNRFJammer();
  void setBrightness(float brightness);
  void turnOn();
  void turnOff();
  void turn();
};