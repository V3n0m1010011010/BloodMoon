#include <TFT_eSPI.h>
#include <EasyButton.h>
#include "menu.h"
#include "icons.h"
#include "variables.h"
extern Menu* activem;
extern TFT_eSPI tft;

Menu::Menu(double id, const std::string& title, bool isScrollable, int sel, int mov, std::function<void()> aDown, std::function<void()> aUp, bool menuWithIcon)
  : id(id), menuWithIcon(menuWithIcon), title(title), isScrollable(isScrollable), mov(mov), sel(sel), aDown(aDown), aUp(aUp) {}

void Menu::init() {
  mov.begin();
  sel.begin();
}

void Menu::addSection(const std::string& section, char icon, std::function<void()> action) {
  sections.push_back(section);
  actions.push_back(action);
  icons.push_back(icon);
}

void Menu::render() {
  tft.setTextFont(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(0, 0, 128, 22, tft.color565(161, 0, 0));
  tft.drawRect(0, 0, 128, 22, tft.color565(255, 255, 255));
  tft.fillRect(5, 0, 128 - 5 * 2, 22, tft.color565(161, 0, 0));
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(title.c_str(), tft.width() / 2, 11);
  if (menuWithIcon) {
    tft.setTextSize(3);
    if (currentIconsSet == 1) {
      tft.loadFont(customIcons1);
    } else if (currentIconsSet == 2) {
      tft.loadFont(customIcons2);
    }
    if (selectedIndex >= 0 && selectedIndex < icons.size()) {
      char currentIcon = icons[selectedIndex];
      tft.drawString(String(currentIcon), tft.width() / 2, tft.height() / 2 - 8);
      tft.unloadFont();
      tft.setTextSize(1);
      tft.setTextFont(2);
      tft.drawString(sections[selectedIndex].c_str(), tft.width() / 2, tft.height() / 2 + 27);
      tft.drawChar('<', 20, 62);
      tft.drawChar('>', 128 - 25, 62);
    }
  }
  tft.setTextDatum(ML_DATUM);
  tft.fillRect(0, 112, 128, 16, tft.color565(161, 0, 0));
  tft.drawString("Sel", 5, 128 - 8);
  tft.drawString("Mov", 98, 128 - 8);
}


void Menu::scrollAnimation(bool dir, int frames) {
  tft.setTextDatum(MC_DATUM);
  for (int i = 0; i < frames; i++) {
    int dx = dir ? 128 / frames : -(128 / frames);
    posX -= dx;
    tft.fillRect(0, 22, 128, 90, TFT_BLACK);
    tft.setTextSize(3);
    if (currentIconsSet == 1) {
      tft.loadFont(customIcons1);
    } else if (currentIconsSet == 2) {
      tft.loadFont(customIcons2);
    }
    if (selectedIndex >= 0 && selectedIndex < icons.size()) {
      int indexBefore = (selectedIndex - 1 < 0) ? icons.size() - 1 : selectedIndex - 1;
      int indexCurrent = selectedIndex;
      int indexAfter = (selectedIndex + 1 > icons.size() - 1) ? 0 : selectedIndex + 1;
      char beforeIcon = icons[indexBefore];
      char currentIcon = icons[selectedIndex];
      char afterIcon = icons[indexAfter];
      tft.setTextWrap(false);
      tft.drawString(String(currentIcon), posX, tft.height() / 2 - 8);
      tft.drawString(String(beforeIcon), posX - tft.width(), tft.height() / 2 - 8);
      tft.drawString(String(afterIcon), posX + tft.width(), tft.height() / 2 - 8);
      tft.unloadFont();
      tft.setTextSize(1);
      tft.setTextFont(2);
      tft.drawString(sections[indexCurrent].c_str(), posX, tft.height() / 2 + 27);
      tft.drawString(sections[indexBefore].c_str(), posX - tft.width(), tft.height() / 2 + 27);
      tft.drawString(sections[indexAfter].c_str(), posX + tft.width(), tft.height() / 2 + 27);
    }
    delay(5);
  }
  posX = tft.width() / 2;
}

void Menu::handleInput() {
  // mov.read();
  // sel.read();
  // if (mov.onPressedFor(1000) && isScrollable) {
  //   scrollAnimation(false, 30);
  //   selectedIndex = (selectedIndex - 1 + sections.size()) % sections.size();
  //   render();
  // } else if (mov.wasPressed() && isScrollable) {
  //   scrollAnimation(true, 30);
  //   selectedIndex = (selectedIndex + 1) % sections.size();
  //   render();
  // }
  // if (sel.wasPressed()) {
  //   actions[selectedIndex]();
  // }
  mov.read();
  sel.read();
  if (mov.isPressed() && pressStartTime == 0) {
    pressStartTime = millis();
    movWasPressed = true;
  }
  if (mov.isReleased() && movWasPressed) {
    unsigned long pressDuration = millis() - pressStartTime;
    if (pressDuration < longPressDuration) {
      if (isScrollable) {
        scrollAnimation(true, 30);
        selectedIndex = (selectedIndex + 1) % sections.size();
        render();
      }
    } else {
      if (isScrollable) {
        scrollAnimation(false, 30);
        selectedIndex = (selectedIndex - 1 + sections.size()) % sections.size();
        render();
      }
    }
    pressStartTime = 0;
    movWasPressed = false;
  }
  if (sel.wasPressed()) {
    actions[selectedIndex]();
  }
}

void Menu::setSubMenu(Menu* menu) {
  subMenu = menu;
  if (menu) {
    menu->setParentMenu(this);
  }
}
void Menu::setParentMenu(Menu* menu) {
  parentMenu = menu;
}
Menu* Menu::getSubMenu() {
  return subMenu;
}

Menu* Menu::getParentMenu() {
  return parentMenu;
}

int Menu::getSelectedIndex() {
  return selectedIndex;
}
void Menu::setSelectedIndex(int index) {
  if (index >= 0 && index < sections.size()) {
    selectedIndex = index;
  } else {
    selectedIndex = 0;
  }
}
void Menu::setlist(std::vector<bool> list) {
  selectedList = list;
}
std::vector<bool> Menu::getlist() {
  return selectedList;
}
void Menu::setTitle(std::string t) {
  title = t;
}
std::string Menu::getTitle() {
  return title;
}
void Menu::setSection(std::string t, int i) {
  sections[i] = t;
}
std::string Menu::getSection(int i) {
  return sections[i];
}
void Menu::setScroll(bool scroll) {
  isScrollable = scroll;
}