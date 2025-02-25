#include <EasyButton.h>
#include "controls.h"
#include "menu.h"
#include "display.h"
#include "variables.h"
extern Menu* activem;
extern display dis;
controls::controls(int sel, int mov)
  : mov(mov), sel(sel) {}
void controls::init() {
  mov.begin();
  sel.begin();
}
bool controls::handleInput() {
  bool inputDetected = false;
  mov.read();
  if (mov.isPressed() && pressStartTime == 0) {
    pressStartTime = millis();
    movWasPressed = true;
  }
  if (mov.isReleased() && movWasPressed) {
    inputDetected = true;
    lastActivityTime = millis();
    unsigned long pressDuration = millis() - pressStartTime;
    dis.turn();
    if (displayNormal) {
      if (pressDuration < longPressDuration) {
        if (activem->isScroll()) {
          if (activem->isMenuWithIcon()) dis.renderIconScrollAnimation(true, 20);
          if(activem->getSectionsSize() > 1 && activem->getType() == "apSelect") dis.renderWifiSelectScrollAnimation(true, 8);
          activem->setSelectedIndex((activem->getSelectedIndex() + 1) % activem->getSectionsSize());
          activem->setRenderState(true);
        }
      } else {
        if (activem->isScroll()) {
          if (activem->isMenuWithIcon()) dis.renderIconScrollAnimation(false, 20);
          if(activem->getSectionsSize() > 1 && activem->getType() == "apSelect") dis.renderWifiSelectScrollAnimation(false, 8);
          activem->setSelectedIndex((activem->getSelectedIndex() - 1 + activem->getSectionsSize()) % activem->getSectionsSize());
          activem->setRenderState(true);
        }
      }
    } else {
      dis.turnOn();
      displayNormal = true;
    }
    pressStartTime = 0;
    movWasPressed = false;
  }
  sel.read();
  if (sel.wasPressed()) {
    selWasPressed = true;
  }
  if (sel.isReleased() && selWasPressed) {
    inputDetected = true;
    lastActivityTime = millis();
    dis.turn();
    selWasPressed = false;
    if (displayNormal) {
      activem->executeAction(activem->getSelectedIndex());
    } else {
      dis.turnOn();
      displayNormal = true;
    }
  }
  Serial.println(activem->getType());
  return inputDetected;
}