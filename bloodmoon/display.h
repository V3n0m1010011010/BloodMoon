#include <TFT_eSPI.h>
#include <map>
extern TFT_eSPI tft;

struct stringM{
  String text;
  unsigned int shiftIndex = 0;
};
std::map<String, int> fontMaxChars = {
    {"1", 17},
    {"2", 13},
    {"4", 10}
};

class display {
private:
  unsigned int startX;
  unsigned int startY;
  unsigned int endX;
  unsigned int endY;
  int posX;

  bool hasApSelectFirstRendered = false;

  unsigned long shiftTimer = 1000;
  unsigned long lastShift = 0;

public:

  bool isSelectRendering = false;
  int lastSelectedIndex = 0;
  bool scrollDirection;
  unsigned int selectedAp;


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

  virtual void renderAp();
  virtual void renderApScrollAnimation(bool dir, int frames);
  virtual void drawApSections(TFT_eSprite &sprite, int index, int x, int y);

  virtual void renderStSelectMenu();

  virtual void renderDeauther();
  virtual void renderDeautherScrollAnimation(bool dir, int frames);
  virtual void drawDeautherSections(TFT_eSprite &sprite, int index, int x);

  virtual void renderMonitorMenu();

  virtual void renderNRFJammer();
  virtual void renderNRFError();

  void setBrightness(float brightness);
  void turnOn();
  void turnOff();
  void turn();

  String stringShifter(const String& input, int index, int maxWidth);
  String macToString(std::array<uint8_t, 6> mac);
  int maxItems(int itemHeight);
  String subString(const String& input, int maxLength);
  int getY(int screenHeight, int start, int rows, int fontHeight, bool isSplit, bool isSub, int rowIndex);
  int getMaxChars(const String& font);
};