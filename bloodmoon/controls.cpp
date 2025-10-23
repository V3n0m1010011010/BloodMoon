#include <EasyButton.h>
#include "globals.h"

controls::controls(int sel, int mov)
  : mov(mov), sel(sel) {}


void controls::init() {
  mov.begin();
  sel.begin();
}


bool controls::handleInput() {
  bool inputDetected = false;
  
  mov.read();

  mov.onPressed([&]() {
    inputDetected = true;
    lastActivityTime = millis();
    dis.turn();
    dis.isSelectRendering = false;
    if (displayNormal) {
      if (menu.isScroll()) {
        menu.setSelectedIndex((menu.getSelectedIndex() + 1) % menu.getSectionCount());
        dis.scrollDirection = true;
        menu.setRenderState(true);
      }
    } else {
      dis.turnOn();
      displayNormal = true;
    }
  });

  mov.onPressedFor(longPressDuration, [&]() {
    inputDetected = true;
    lastActivityTime = millis();
    dis.turn();
    dis.isSelectRendering = false;
    if (displayNormal) {
      if (menu.isScroll()) {
        menu.setSelectedIndex((menu.getSelectedIndex() - 1 + menu.getSectionCount()) % menu.getSectionCount());
        dis.scrollDirection = false;
        menu.setRenderState(true);
      }
    } else {
      dis.turnOn();
      displayNormal = true;
    }
  });


  sel.read();
  if (sel.wasPressed()) {
    selWasPressed = true;
  }
  if (sel.isReleased() && selWasPressed) {
    inputDetected = true;
    lastActivityTime = millis();
    dis.turn();
    dis.isSelectRendering = false;
    selWasPressed = false;
    if (displayNormal) {
      menu.executeAction(menu.getSelectedIndex());
    } else {
      dis.turnOn();
      displayNormal = true;
    }
    dis.lastSelectedIndex = menu.getSelectedIndex();
  }
  return inputDetected;
}