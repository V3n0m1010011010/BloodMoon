#include <EasyButton.h>
class controls {
private:
  EasyButton mov;
  EasyButton sel;
  bool movWasPressed = false;
  bool selWasPressed = false;
  unsigned long pressStartTime = 0;
  const unsigned long longPressDuration = 300;
public:
  controls(int sel, int mov);
  void init();
  bool handleInput();
};