#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include <TFT_eSPI.h>
#include <EasyButton.h>
extern TFT_eSPI tft;
class Menu {
private:
  String type;
  bool menuWithIcon = false;
  std::string title;
  std::vector<std::string> sections;
  std::vector<char> icons;
  std::vector<std::function<void()>> actions;
  std::function<void()> aDown;
  std::function<void()> aUp;
  int selectedIndex = 0;
  bool animate = false;
  bool isScrollable;
  std::vector<bool> selectedList;
  Menu* subMenu = nullptr;
  Menu* parentMenu = nullptr;
  bool needsRender = false;

public:
  Menu(String type, const std::string& title, bool isScrollable, std::function<void()> aDown = nullptr, std::function<void()> aUp = nullptr, bool menuWithIcon = false);

  void addSection(const std::string& section, char icon, std::function<void()> action = nullptr);

  void setParentMenu(Menu* menu);
  Menu* getParentMenu();
  void setSubMenu(Menu* menu);
  Menu* getSubMenu();
  String getType();
  int getSelectedIndex();
  void setSelectedIndex(int index);

  bool isScroll();
  bool isMenuWithIcon();

  void setlist(std::vector<bool> list);
  std::vector<bool> getlist();

  void setTitle(std::string t);
  std::string getTitle();

  void setSection(std::string t, int i);
  std::string getSection(int i);

  char getIcon(int i);
  int getIconsSize();

  int getSectionsSize();
  void executeAction(int i);
  void setScroll(bool scroll);

  void setRenderState(bool state);
  bool getRenderState();
};

#endif
