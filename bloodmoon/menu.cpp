#include <EasyButton.h>
#include <TFT_eSPI.h>
#include "menu.h"
#include "settings.h"
#include "globals.h"
#include "variables.h"
#include "display.h"
extern Menu* activem;
extern TFT_eSPI tft;
extern display dis;
Menu::Menu(String type, const std::string& title, bool isScrollable, std::function<void()> aDown, std::function<void()> aUp, bool menuWithIcon)
  : type(type), menuWithIcon(menuWithIcon), title(title), isScrollable(isScrollable), aDown(aDown), aUp(aUp) {}

void Menu::addSection(const std::string& section, char icon, std::function<void()> action) {
  sections.push_back(section);
  actions.push_back(action);
  icons.push_back(icon);
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
String Menu::getType(){
  return type;
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
bool Menu::isScroll() {
  return isScrollable;
}
bool Menu::isMenuWithIcon() {
  return menuWithIcon;
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
int Menu::getSectionsSize() {
  return sections.size();
}
char Menu::getIcon(int i){
  return icons[i];
}
int Menu::getIconsSize(){
  return icons.size();
}
void Menu::executeAction(int i) {
  actions[i]();
}
void Menu::setScroll(bool scroll) {
  isScrollable = scroll;
}