void initAll() {
  tft.init();
  Serial.begin(115200);
  spi.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);
  ctr.init();
  nRF24.init();
  Wifi.init();
  delay(200);
  pinMode(backlightPin, OUTPUT);
  mainMenu();
  wifiMenu();
  wifiScanMenu();
  wifiScanApMenu();
  wifiScanStMenu();
  wifiAttackMenu();
  wifiAttackDeauthMenu();
  wifiGeneralMenu();
  wifiGeneralSelectMenu();
  blueMenu();
  nrfMenu();
  nrfJammerMenu();
  settMenu();
  mainm->setSubMenu(wifim);
  wifim->setSubMenu(wifiScanm);
  wifiScanm->setSubMenu(wifiScanApsm);
  wifiScanm->setSubMenu(wifiScanStsm);
  wifim->setSubMenu(wifiAttackm);
  wifiAttackm->setSubMenu(wifiAttackDeauthm);
  wifim->setSubMenu(wifiGeneralm);
  wifiGeneralm->setSubMenu(wifiGeneralSelectm);
  mainm->setSubMenu(bluem);
  mainm->setSubMenu(nrfm);
  nrfm->setSubMenu(nrfJammerm);
  mainm->setSubMenu(settm);
  dis.renderBoot();
  activem = mainm;
  activem->setRenderState(true);
  lastActivityTime = millis();
}





/*################################################################################################
+
*                              start definition for all menus 
*
##################################################################################################*/


//######################################### MAIN MENU #############################################

void mainMenu() {
  mainm = new Menu("normal", "Main Menu", true, nullptr, nullptr, true);
  mainm->addSection("WiFi", '\x21', []() {
    activem = wifim;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  mainm->addSection("Bluetooth", '\x22', []() {
    activem = bluem;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  mainm->addSection("nRF24", '\x25', []() {
    if (nRF24.getInit()) {
      activem = nrfm;
      activem->setSelectedIndex(0);
      activem->setRenderState(true);
    } else {
      dis.renderNRFError();
    }
  });
  mainm->addSection("Settings", '\x23', []() {
    activem = settm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}

//##################################### END MAIN MENU #########################################






//######################################### WIFI MENU #########################################

void wifiMenu() {
  wifim = new Menu("normal", "WiFi", true);
  wifim->addSection("Back", '\0', []() {
    activem = wifim->getParentMenu();
    activem->setRenderState(true);
  });
  wifim->addSection("Scanning", '\0', []() {
    activem = wifiScanm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  wifim->addSection("Attack", '\0', []() {
    activem = wifiAttackm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  wifim->addSection("Gerneral", '\0', []() {
    activem = wifiGeneralm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  wifim->addSection("Sniffer", '\0', []() {
    activem = wifiSnifferm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}

//##################################################################################################




//######################################### WiFi scan menu #########################################
void wifiScanMenu() {
  wifiScanm = new Menu("normal", "Scan", true);
  wifiScanm->addSection("Back", '\0', []() {
    activem = wifiScanm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiScanm->addSection("Scan Ap's", '\0', []() {
    activem = wifiScanApsm;
    activem->setRenderState(true);
    Wifi.scanAps();
  });
  wifiScanm->addSection("Scan St's", '\0', []() {
    activem = wifiScanStsm;
    activem->setRenderState(true);
    Wifi.scanSts();
  });
}
//#################################################################################################


//######################################### Scan menus ############################################
void wifiScanApMenu() {
  wifiScanApsm = new Menu("scanAp", "Scanning", false);
  wifiScanApsm->addSection("Back", '\0', []() {
    activem = wifiScanApsm->getParentMenu();
    Wifi.stopWifiScan();
    activem->setRenderState(true);
  });
}
void wifiScanStMenu() {
  wifiScanStsm = new Menu("scanSt", "Scanning", false);
  wifiScanStsm->addSection("Back", '\0', []() {
    activem = wifiScanStsm->getParentMenu();
    Wifi.stopWifiScan();
    activem->setRenderState(true);
  });
}
//########################################################################################






//#################################### wifi attack menu ##################################
void wifiAttackMenu() {
  wifiAttackm = new Menu("normal", "Attacks", true);
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiAttackm->addSection("Deauth", '\0', []() {
    activem = wifiAttackDeauthm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}
//########################################################################################



//###################################### deauth menu #####################################
void wifiAttackDeauthMenu() {
  wifiAttackm = new Menu("deauther", "Deauther", true);
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiAttackm->addSection("Start", '\0', []() {
    if (Attack.getMode(0) == true) {
      activem->setTitle("Deauther");
      activem->setSection("Start", 1);
      activem->setRenderState(true);
    } else {
      activem->setTitle("Deauthing");
      activem->setSection("Stop", 1);
      activem->setRenderState(true);
      Attack.deauth();
    }
  });
}
//#######################################################################################







//################################## wifi general menu ##################################

void wifiGeneralMenu() {
  wifiGeneralm = new Menu("normal", "General", true);
  wifiGeneralm->addSection("Back", '\0', []() {
    activem = wifiGeneralm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiGeneralm->addSection("Select", '\0', []() {
    activem = wifiGeneralSelectm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}
//######################################################################################



//################################## wifi select menu ##################################
void wifiGeneralSelectMenu() {
  wifiGeneralSelectm = new Menu("normal", "Select", true);
  wifiGeneralSelectm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiGeneralSelectm->addSection("AccessPoints", '\0', []() {
    wifiGeneralSelectApMenu();
    activem = wifiGeneralSelectApm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  wifiGeneralSelectm->addSection("Stations", '\0', []() {
    wifiGeneralSelectStMenu();
    activem = wifiGeneralSelectStm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}
//#######################################################################################


//################################### wifi ap select ####################################
void wifiGeneralSelectApMenu() {
  wifiGeneralSelectApm = new Menu("apSelect", "AP's", true);
  wifiGeneralSelectApm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectApm->getParentMenu();
    activem->setRenderState(true);
  });
  auto list = Wifi.getApList();
  Serial.println(list.size());
  for (int i = 0; i < list.size(); i++) {
    wifiGeneralSelectApm->addSection(list[i].ssid.c_str(), '\0');
  }
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectApm);
}
//#######################################################################################


//#################################### st select menu ###################################
void wifiGeneralSelectStMenu() {
  wifiGeneralSelectStm = new Menu("stSelect", "ST's", true);
  wifiGeneralSelectStm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectStm->getParentMenu();
    activem->setRenderState(true);
  });
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectStm);
}
//#######################################################################################






//################################### Bluetooth MENU ####################################

void blueMenu() {
  bluem = new Menu("normal", "BLT", true);
  bluem->addSection("Back", '\0', []() {
    activem = bluem->getParentMenu();
    activem->setRenderState(true);
  });
  bluem->addSection("Blue-Scan", '\0', []() {
    activem = blueScanm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  bluem->addSection("Blue-Attacks", '\0', []() {
    activem = blueAttackm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  bluem->addSection("Blue-Sniffer", '\0', []() {
    activem = blueSnifferm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
}

//#################################################################################################








//########################################## nRF24 MENU ###########################################

void nrfMenu() {
  nrfm = new Menu("normal", "nRF24", true);
  nrfm->addSection("Back", '\0', []() {
    activem = nrfm->getParentMenu();
    activem->setRenderState(true);
  });
  nrfm->addSection("Jammer", '\0', []() {
    activem = nrfJammerm;
    activem->setSelectedIndex(0);
    dis.renderNRFJammer();
    Attack.nRF24Jammer();
  });
}
void nrfJammerMenu() {
  if (nRF24.getInit()) {
    nrfJammerm = new Menu("jammer", "Jammer", false);
    nrfJammerm->addSection("Back", '\0', []() {
      activem = nrfJammerm->getParentMenu();
      Attack.setMode(1, false);
      activem->setRenderState(true);
    });
  }
}

//###############################################################################################



//######################################## Settings MENU ########################################

void settMenu() {
  settm = new Menu("normal", "Settings", true);
  settm->addSection("Back", '\0', []() {
    activem = settm->getParentMenu();
    activem->setRenderState(true);
  });
  settm->addSection("Set UI-color", '\0', []() {
    activem = settColorm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  settm->addSection("Set Icons-set", '\0', []() {
    activem = settIconsm;
    activem->setSelectedIndex(0);
    activem->setRenderState(true);
  });
  settm->addSection("Reboot", '\0', []() {
    esp_restart();
  });
}

//###############################################################################################





/*####################################################################################################################################################################
*
*
*
####################################################################################################################################################################*/