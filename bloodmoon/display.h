#include <TFT_eSPI.h>
extern TFT_eSPI tft;
class display {
private:
  unsigned int startX;
  unsigned int startY;
  unsigned int endX;
  unsigned int endY;
  int posX;
  bool hasApSelectFirstRendered = false;
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
  virtual void renderApSelectMenuScrollAni();
  virtual void renderStSelectMenu();
  virtual void renderWifiSelectScrollAnimation(bool dir, int frames);
  virtual void renderDeauthMenu();
  virtual void renderMonitorMenu();
  virtual void renderNRFJammer();
  virtual void renderNRFError();
  void setBrightness(float brightness);
  void turnOn();
  void turnOff();
  void turn();
  String stringShifter(const String& input, int offset, int maxWidth);
};