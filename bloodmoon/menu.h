#pragma once
#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include <TFT_eSPI.h>

struct MenuSection
{
  std::string name;
  char icon = 0;
  std::function<void()> action = nullptr;

  MenuSection(const std::string& n, char i = 0, std::function<void()> a = nullptr)
        : name(n), icon(i), action(a) {}
};

struct menuData
{
  String type;
  bool menuWithIcon = false;
  std::string title;
  std::vector<MenuSection> sections;
  int selectedIndex = 0;
  bool isScrollable;
  menuData *parentMenu = nullptr;
  bool needsRender = false;
};

class Menu
{
private:
  menuData mainm;

  menuData wifim;
  menuData wifiScanm;
  menuData wifiScanApsm;
  menuData wifiScanStsm;

  menuData wifiGeneralm;
  menuData wifiGeneralSelectm;
  menuData wifiGeneralSelectApm;
  menuData wifiGeneralSelectAp;
  menuData wifiGeneralSelectStm;

  menuData wifiAttackm;
  menuData wifiAttackDeauthm;
  menuData wifiAttackHandshakem;

  menuData wifiSnifferm;


  menuData nrfm;
  menuData nrfJammerm;

  menuData bluem;
  menuData blueScanm;
  menuData blueAttackm;
  menuData blueSnifferm;

  menuData settm;
  menuData settColorm;
  menuData settIconsm;

public:
  menuData activem;

  Menu();
  void init();

  String getType();
  int getSelectedIndex();
  void setSelectedIndex(int index);

  bool isScroll();
  bool isMenuWithIcon();

  void setTitle(std::string t);
  std::string getTitle();

  std::string getSection(int i);

  char getIcon(int i);
  int getIconCount();

  int getSectionCount();
  void executeAction(int i);
  void setScroll(bool scroll);

  void setRenderState(bool state);
  bool getRenderState();

  void setMinValues(menuData *menu, String type, const std::string &title, bool isScrollable, bool menuWithIcon);

  void updateScannedNetworkMenus();
  void wifiGeneralSelectApMenu(int i);
};

#endif
