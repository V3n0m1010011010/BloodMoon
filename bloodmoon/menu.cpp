#include <TFT_eSPI.h>
#include "settings.h"
#include "globals.h"

Menu::Menu()
{}

void Menu::init(){

  // ######################################### MAIN MENU #############################################

  setMinValues(&mainm, "normal", "Main Menu", true, true);
  mainm.sections.push_back(MenuSection{"Wifi", '\x21', [this]()
                                       {
                                         activem = wifim;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  mainm.sections.push_back(MenuSection{"Bluetooth", '\x22', [this]()
                                       {
                                         activem = bluem;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  mainm.sections.push_back(MenuSection{"nRF24", '\x25', [this]()
                                       {
                                         if (nRF24.getInit())
                                         {
                                           activem = nrfm;
                                           activem.selectedIndex = 0;
                                           activem.needsRender = true;
                                         }
                                         else
                                         {
                                           dis.renderNRFError();
                                         }
                                       }});
  mainm.sections.push_back(MenuSection{"Settings", '\x23', [this]()
                                       {
                                         activem = settm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});

  // ##################################### END MAIN MENU ##########################################

  // ######################################### WIFI MENUS #########################################

  setMinValues(&wifim, "normal", "WiFi", true, false);
  wifim.sections.push_back(MenuSection{"Back", '\0', [this]()
                                       {
                                         activem = mainm;
                                         activem.needsRender = true;
                                       }});
  wifim.sections.push_back(MenuSection{"Scanning", '\0', [this]()
                                       {
                                         activem = wifiScanm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  wifim.sections.push_back(MenuSection{"General", '\0', [this]()
                                       {
                                         activem = wifiGeneralm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  wifim.sections.push_back(MenuSection{"Attack", '\0', [this]()
                                       {
                                         activem = wifiAttackm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  wifim.sections.push_back(MenuSection{"Sniffer", '\0', [this]()
                                       {
                                         activem = wifiSnifferm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});

  // ##################################################################################################

  // ######################################### WiFi scan menu #########################################

  setMinValues(&wifiScanm, "normal", "Scan", true, false);
  wifiScanm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                           {
                                             activem = wifim;
                                             activem.needsRender = true;
                                           }});
  wifiScanm.sections.push_back(MenuSection{"Scan Ap's", '\0', [this]()
                                           {
                                             activem = wifiScanApsm;
                                             activem.needsRender = true;
                                             Wifi.scanAps();
                                           }});
  wifiScanm.sections.push_back(MenuSection{"Scan St's", '\0', [this]()
                                           {
                                             activem = wifiScanStsm;
                                             activem.needsRender = true;
                                             Wifi.scanSts();
                                           }});

  // #################################################################################################

  // ######################################### WiFi Scan menus #######################################

  setMinValues(&wifiScanApsm, "scanAp", "Scanning", false, false);
  wifiScanApsm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                              {
                                                activem = wifiScanm;
                                                Wifi.stopWifiScan();
                                                activem.needsRender = true;
                                              }});
  setMinValues(&wifiScanStsm, "scanSt", "Scanning", false, false);
  wifiScanStsm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                              {
                                                activem = wifiScanm;
                                                Wifi.stopWifiScan();
                                                activem.needsRender = true;
                                              }});

  // #################################################################################################

  // ################################## wifi general menu #############################################

  setMinValues(&wifiGeneralm, "normal", "General", true, false);
  wifiGeneralm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                              {
                                                activem = wifim;
                                                activem.selectedIndex = 0;
                                                activem.needsRender = true;
                                              }});
  wifiGeneralm.sections.push_back(MenuSection{"Select", '\0', [this]()
                                              {
                                                activem = wifiGeneralSelectm;
                                                activem.selectedIndex = 0;
                                                activem.needsRender = true;
                                              }});

  // #################################################################################################

  // ################################## wifi select menu #############################################

  setMinValues(&wifiGeneralSelectm, "normal", "Select", true, false);
  wifiGeneralSelectm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                    {
                                                      activem = wifiGeneralm;
                                                      activem.selectedIndex = 0;
                                                      activem.needsRender = true;
                                                    }});
  wifiGeneralSelectm.sections.push_back(MenuSection{"AccessPoints", '\0', [this]()
                                                    {
                                                      updateScannedNetworkMenus();
                                                      activem = wifiGeneralSelectApm;
                                                      activem.selectedIndex = 0;
                                                      activem.needsRender = true;
                                                    }});
  wifiGeneralSelectm.sections.push_back(MenuSection{"Stations", '\0', [this]()
                                                    {
                                                      updateScannedNetworkMenus();
                                                      activem = wifiGeneralSelectStm;
                                                      activem.selectedIndex = 0;
                                                      activem.needsRender = true;
                                                    }});

  // #################################################################################################

  // ################################### wifi ap/st select ###########################################

  setMinValues(&wifiGeneralSelectApm, "normal", "AP's", true, false);
  wifiGeneralSelectApm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                      {
                                                        activem = wifiGeneralSelectm;
                                                        activem.needsRender = true;
                                                      }});

  setMinValues(&wifiGeneralSelectStm, "normal", "ST's", true, false);
  wifiGeneralSelectApm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                      {
                                                        activem = wifiGeneralSelectm;
                                                        activem.needsRender = true;
                                                      }});

  // #################################################################################################

  // ######################################### aps/sts menu ##########################################

  setMinValues(&wifiGeneralSelectAp, "apSelect", "Select", true, false);
  wifiGeneralSelectAp.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                     {
                                                       activem = wifiGeneralSelectApm;
                                                       activem.needsRender = true;
                                                     }});
  wifiGeneralSelectAp.sections.push_back(MenuSection{"info1", '\0', nullptr});
  wifiGeneralSelectAp.sections.push_back(MenuSection{"info2", '\0', nullptr});

  // #################################### wifi attack menu ###########################################

  setMinValues(&wifiAttackm, "normal", "Attacks", true, false);
  wifiAttackm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                             {
                                               activem = wifim;
                                               activem.selectedIndex = 0;
                                               activem.needsRender = true;
                                             }});
  wifiAttackm.sections.push_back(MenuSection{"Deauth", '\0', [this]()
                                             {
                                               activem = wifiAttackDeauthm;
                                               activem.selectedIndex = 0;
                                               activem.needsRender = true;
                                             }});
  wifiAttackm.sections.push_back(MenuSection{"WPA Capture", '\0', [this]()
                                             {
                                               activem = wifiAttackHandshakem;
                                               activem.selectedIndex = 0;
                                               activem.needsRender = true;
                                             }});

  // #################################################################################################

  // ###################################### deauth menu ##############################################

  setMinValues(&wifiAttackDeauthm, "deauther", "Deauther", true, false);
  wifiAttackDeauthm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                   {
                                                     activem = wifiAttackm;
                                                     activem.selectedIndex = 0;
                                                     activem.needsRender = true;
                                                   }});

  wifiAttackDeauthm.sections.push_back(MenuSection{"Start", '\0', [this]()
                                                   {
                                                     if (!Wifi.isDeauthing())
                                                     {
                                                       activem.isScrollable = false;
                                                       activem.title = "Deauthing";
                                                       if (activem.sections.size() > 1)
                                                         activem.sections[1].name = "Stop";
                                                       activem.needsRender = true;
                                                       Wifi.startDeauth();
                                                     }
                                                     else
                                                     {
                                                       activem.isScrollable = true;
                                                       activem.title = "Deauther";
                                                       if (activem.sections.size() > 1)
                                                         activem.sections[1].name = "Start";
                                                       activem.needsRender = true;
                                                       Wifi.stopDeauth();
                                                     }
                                                   }});

  // #################################################################################################

  // ###################################### handshake menu ###########################################

  setMinValues(&wifiAttackHandshakem, "normal", "WPA Capture", true, false);
  wifiAttackHandshakem.sections.push_back(MenuSection{"Back", '\0', [this]()
                                                   {
                                                     activem = wifiAttackm;
                                                     activem.selectedIndex = 0;
                                                     activem.needsRender = true;
                                                   }});
  wifiAttackHandshakem.sections.push_back(MenuSection{"Start", '\0', [this]()
                                                   {
                                                     if (!Wifi.isWPACapturing())
                                                     {
                                                       activem.isScrollable = false;
                                                       activem.title = "Capturing...";
                                                       if (activem.sections.size() > 1)
                                                       activem.sections[1].name = "Stop";
                                                       activem.needsRender = true;
                                                       Wifi.startWPACapture();
                                                     }
                                                     else
                                                     {
                                                       activem.isScrollable = true;
                                                       activem.title = "WPA Capture";
                                                       if (activem.sections.size() > 1)
                                                       activem.sections[1].name = "Start";
                                                       activem.needsRender = true;
                                                       Wifi.stopWPACapture();
                                                     }
                                                   }});

  // #################################################################################################

  // ################################### Bluetooth MENU ##############################################

  setMinValues(&bluem, "normal", "BLT", true, false);
  bluem.sections.push_back(MenuSection{"Back", '\0', [this]()
                                       {
                                         activem = mainm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  bluem.sections.push_back(MenuSection{"Blue-Scan", '\0', [this]()
                                       {
                                         activem = blueScanm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  bluem.sections.push_back(MenuSection{"Blue-Attacks", '\0', [this]()
                                       {
                                         activem = blueAttackm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  bluem.sections.push_back(MenuSection{"Blue-Sniffer", '\0', [this]()
                                       {
                                         activem = blueSnifferm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});

  // #################################################################################################

  // ########################################## nRF24 MENU ###########################################
  if (nRF24.getInit())
  {

    setMinValues(&nrfm, "normal", "nRF24", true, false);
    nrfm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                        {
                                          activem = mainm;
                                          activem.selectedIndex = 0;
                                          activem.needsRender = true;
                                        }});
    nrfm.sections.push_back(MenuSection{"Jammer", '\0', [this]()
                                        {
                                          activem = nrfJammerm;
                                          activem.selectedIndex = 0;
                                          activem.needsRender = true;
                                          nRF24.startNRF24Jammer();
                                        }});

    setMinValues(&nrfJammerm, "jammer", "Jammer", false, false);
    nrfJammerm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                              {
                                                nRF24.stopNRF24Jammer();
                                                activem = nrfm;
                                                activem.selectedIndex = 0;
                                                activem.needsRender = true;
                                              }});
  }
  // ###############################################################################################

  // ######################################## Settings MENU ########################################

  setMinValues(&settm, "normal", "Settings", true, false);
  settm.sections.push_back(MenuSection{"Back", '\0', [this]()
                                       {
                                         activem = mainm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  settm.sections.push_back(MenuSection{"Set UI-color", '\0', [this]()
                                       {
                                         activem = settColorm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  settm.sections.push_back(MenuSection{"Set Icons-set", '\0', [this]()
                                       {
                                         activem = settIconsm;
                                         activem.selectedIndex = 0;
                                         activem.needsRender = true;
                                       }});
  settm.sections.push_back(MenuSection{"Reboot", '\0', [this]()
                                       {
                                         esp_restart();
                                       }});

  // ###############################################################################################

  menu.activem = mainm;
}

String Menu::getType()
{
  return activem.type;
}
int Menu::getSelectedIndex()
{
  return activem.selectedIndex;
}
void Menu::setSelectedIndex(int index)
{
  if (index >= 0 && index < activem.sections.size())
  {
    activem.selectedIndex = index;
  }
  else
  {
    activem.selectedIndex = 0;
  }
}
bool Menu::isScroll()
{
  return activem.isScrollable;
}
bool Menu::isMenuWithIcon()
{
  return activem.menuWithIcon;
}
void Menu::setTitle(std::string t)
{
  activem.title = t;
}
std::string Menu::getTitle()
{
  return activem.title;
}
std::string Menu::getSection(int i)
{
  return activem.sections[i].name;
}
int Menu::getSectionCount()
{
  return activem.sections.size();
}
char Menu::getIcon(int i)
{
  return activem.sections[i].icon;
}
int Menu::getIconCount()
{
  return activem.sections.size();
}
void Menu::executeAction(int i)
{
  if (activem.sections[i].action != nullptr)
    activem.sections[i].action();
}
void Menu::setScroll(bool scroll)
{
  activem.isScrollable = scroll;
}
void Menu::setRenderState(bool state)
{
  activem.needsRender = state;
}
bool Menu::getRenderState()
{
  return activem.needsRender;
}

void Menu::setMinValues(menuData *menu, String type, const std::string &title, bool isScrollable, bool menuWithIcon)
{
  menu->type = type;
  menu->title = title;
  menu->isScrollable = isScrollable;
  menu->menuWithIcon = menuWithIcon;
}

void Menu::updateScannedNetworkMenus()
{
  wifiGeneralSelectApm.sections.erase(wifiGeneralSelectApm.sections.begin() + 1, wifiGeneralSelectApm.sections.end());
  auto list = Wifi.getApList();
  for (int i = 0; i < list.size(); i++)
  {
    wifiGeneralSelectApm.sections.push_back(MenuSection{std::string(list[i].ssid.c_str()), '\0', [this, i]()
                                                        {
                                                          wifiGeneralSelectApMenu(i);
                                                          activem = wifiGeneralSelectAp;
                                                          activem.selectedIndex = 0;
                                                          activem.needsRender = true;
                                                        }});
  }
}
void Menu::wifiGeneralSelectApMenu(int i)
{
  dis.selectedAp = i;
  wifiGeneralSelectApm.sections.erase(wifiGeneralSelectApm.sections.begin() + 3, wifiGeneralSelectApm.sections.end());
  wifiGeneralSelectAp.sections.push_back(MenuSection{"selDeauth", '\0', [this, i]()
                                                     {
                                                       auto ap = Wifi.getApList()[i];
                                                       Wifi.setApSelectings(i, 0, !ap.isDeauthSelected);
                                                       activem.needsRender = true;
                                                     }});
  wifiGeneralSelectAp.sections.push_back(MenuSection{"selWPACapture", '\0', [this, i]()
                                                     {
                                                       auto ap = Wifi.getApList()[i];
                                                       Wifi.setApSelectings(i, 2, !ap.isWPACaptureSelected);
                                                       activem.needsRender = true;
                                                     }});
  // wifiGeneralSelectAp->addSection("selBecClone", '\0', [i](){
  //   auto ap = Wifi.getApList()[i];
  //   Wifi.setApSelectings(i, 1, !ap.isBecCloneSelected);
  //   activem->setRenderState(true);
  // });
}