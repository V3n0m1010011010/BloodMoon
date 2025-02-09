#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include <TFT_eSPI.h>
#include <EasyButton.h>
extern TFT_eSPI tft;
class Menu {
private:
  int id;
  bool menuWithIcon = false;
  std::string title;
  std::vector<std::string> sections;
  std::vector<char> icons;
  std::vector<std::function<void()>> actions;
  std::function<void()> aDown;
  std::function<void()> aUp;
  int selectedIndex = 0;
  int menuArray[3] = { 2, 0, 1 };
  int posX = tft.width() / 2;
  bool animate = false;
  EasyButton mov;
  EasyButton sel;
  bool movWasPressed = false;
  bool selWasPressed = false;
  unsigned long pressStartTime = 0;
  const unsigned long longPressDuration = 300;
  bool isScrollable;
  std::vector<bool> selectedList;
  Menu* subMenu = nullptr;
  Menu* parentMenu = nullptr;

public:
  Menu(double id, const std::string& title, bool isScrollable, int sel, int mov, std::function<void()> aDown = nullptr, std::function<void()> aUp = nullptr, bool menuWithIcon = false);

  void init();
  void addSection(const std::string& section, char icon, std::function<void()> action = nullptr);
  virtual void renderHead();
  virtual void renderMain();
  virtual void renderFoot();
  void renderAll();
  virtual void iconScrollAnimation(bool dir, int frames);
  bool handleInput();

  void setParentMenu(Menu* menu);
  Menu* getParentMenu();
  void setSubMenu(Menu* menu);
  Menu* getSubMenu();

  int getSelectedIndex();
  void setSelectedIndex(int index);

  void setlist(std::vector<bool> list);
  std::vector<bool> getlist();

  void setTitle(std::string t);
  std::string getTitle();

  void setSection(std::string t, int i);
  std::string getSection(int i);

  void setScroll(bool scroll);
};

#endif
