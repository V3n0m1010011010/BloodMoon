void initAll() {
  tft.init();
  Serial.begin(115200);
  mySPI.begin(17, 16, 18, 33);
  nRF24.init();
  delay(200);
  dis.renderBoot();
  pinMode(10, OUTPUT);
  ledcSetup(pwmChannel, pwmFrequency, 8);
  ledcAttachPin(backlightPin, pwmChannel);
  mainMenu();
  wifiMenu();
  wifiScanMenu();
  wifiScanApMenu();
  wifiScanStMenu();
  wifiAttackMenu();
  wifiAttackDeauthMenu();
  wifiGeneralMenu();
  wifiGeneralSelectMenu();
  wifiGeneralSelectApMenu();
  wifiGeneralSelectStMenu();
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
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectApm);
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectStm);
  mainm->setSubMenu(bluem);
  if (nRF24.getInit()) {
    mainm->setSubMenu(nrfm);
    nrfm->setSubMenu(nrfJammerm);
  }
  mainm->setSubMenu(settm);
  activem = mainm;
  dis.renderIconMenu();
  lastActivityTime = millis();
}

//----------------------------------------------------START INIT MENUS--------------------------------------------------------------------

//----------------------------------------------------MAIN MENU--------------------------------------------------------------------

void mainMenu() {
  mainm = new Menu(0.0, "Main Menu", true, select, move, nullptr, nullptr, true);
  mainm->init();
  mainm->addSection("WiFi", '\x21', []() {
    activem = wifim;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  mainm->addSection("Bluetooth", '\x22', []() {
    activem = bluem;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  if (nRF24.getInit()) {
    mainm->addSection("nRF24", '\x25', []() {
      activem = nrfm;
      activem->setSelectedIndex(0);
      dis.renderMenu();
    });
  }
  mainm->addSection("Settings", '\x23', []() {
    activem = settm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}

//----------------------------------------------------END MAIN MENU--------------------------------------------------------------------


//----------------------------------------------------WIFI MENU--------------------------------------------------------------------

void wifiMenu() {
  wifim = new Menu(1.0, "WiFi", true, select, move);
  wifim->init();
  wifim->addSection("Back", '\0', []() {
    activem = wifim->getParentMenu();
    dis.renderMenu();
  });
  wifim->addSection("Scanning", '\0', []() {
    activem = wifiScanm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  wifim->addSection("Attack", '\0', []() {
    activem = wifiAttackm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  wifim->addSection("Gerneral", '\0', []() {
    activem = wifiGeneralm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  wifim->addSection("Sniffer", '\0', []() {
    activem = wifiSnifferm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}

void wifiScanMenu() {
  wifiScanm = new Menu(1.0, "Scan", true, select, move);
  wifiScanm->init();
  wifiScanm->addSection("Back", '\0', []() {
    activem = wifiScanm->getParentMenu();
    dis.renderMenu();
  });
  wifiScanm->addSection("Scan Ap's", '\0', []() {
    activem = wifiScanApsm;
    dis.renderApScanMenu();
    Wifi.scanAps();
  });
  wifiScanm->addSection("Scan St's", '\0', []() {
    activem = wifiScanStsm;
    dis.renderStScanMenu();
  });
}
void wifiScanApMenu() {
  wifiScanApsm = new Menu(1.0, "Scanning", false, select, move);
  wifiScanApsm->init();
  wifiScanApsm->addSection("Back", '\0', []() {
    Wifi.setApScanStatus(false);
    Wifi.clearApList();
    Serial.println("SelPressed");
    activem = wifiScanApsm->getParentMenu();
    dis.renderMenu();
  });
}
void wifiScanStMenu() {
  wifiScanStsm = new Menu(1.0, "Scanning", false, select, move);
  wifiScanStsm->init();
  wifiScanStsm->addSection("Back", '\0', []() {
    activem = wifiScanStsm->getParentMenu();
    dis.renderMenu();
  });
}
void wifiAttackMenu() {
  wifiAttackm = new Menu(0.0, "Attacks", true, select, move);
  wifiAttackm->init();
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    dis.renderMenu();
  });
  wifiAttackm->addSection("Deauth", '\0', []() {
    activem = wifiAttackDeauthm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}
void wifiAttackDeauthMenu() {
  wifiAttackm = new Menu(0.0, "Deauther", true, select, move);
  wifiAttackm->init();
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    dis.renderMenu();
  });
  wifiAttackm->addSection("Start", '\0', []() {
    if (Attack.getMode(0) == true) {
      activem->setTitle("Deauther");
      activem->setSection("Start", 1);
      dis.renderMenu();
    } else {
      activem->setTitle("Deauthing");
      activem->setSection("Stop", 1);
      dis.renderMenu();
      Attack.deauth();
    }
  });
}

void wifiGeneralMenu() {
  wifiGeneralm = new Menu(0.0, "General", true, select, move);
  wifiGeneralm->init();
  wifiGeneralm->addSection("Back", '\0', []() {
    activem = wifiGeneralm->getParentMenu();
    dis.renderMenu();
  });
  wifiGeneralm->addSection("Select", '\0', []() {
    activem = wifiGeneralSelectm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}
void wifiGeneralSelectMenu() {
  wifiGeneralSelectm = new Menu(0.0, "Select", true, select, move);
  wifiGeneralSelectm->init();
  wifiGeneralSelectm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectm->getParentMenu();
    dis.renderMenu();
  });
  wifiGeneralSelectm->addSection("AccessPoints", '\0', []() {
    activem = wifiGeneralSelectApm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  wifiGeneralSelectm->addSection("Stations", '\0', []() {
    activem = wifiGeneralSelectStm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}
void wifiGeneralSelectApMenu() {
  wifiGeneralSelectApm = new Menu(0.0, "General", true, select, move);
  wifiGeneralSelectApm->init();
  wifiGeneralSelectApm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectApm->getParentMenu();
    dis.renderMenu();
  });
}
void wifiGeneralSelectStMenu() {
  wifiGeneralSelectApm = new Menu(0.0, "General", true, select, move);
  wifiGeneralSelectApm->init();
  wifiGeneralSelectApm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectApm->getParentMenu();
    dis.renderMenu();
  });
}
//----------------------------------------------------END WIFI MENU--------------------------------------------------------------------


//----------------------------------------------------Bluetooth MENU--------------------------------------------------------------------

void blueMenu() {
  bluem = new Menu(2.0, "BLT", true, select, move);
  bluem->init();
  bluem->addSection("Back", '\0', []() {
    activem = bluem->getParentMenu();
    dis.renderMenu();
  });
  bluem->addSection("Blue-Scan", '\0', []() {
    activem = blueScanm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  bluem->addSection("Blue-Attacks", '\0', []() {
    activem = blueAttackm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  bluem->addSection("Blue-Sniffer", '\0', []() {
    activem = blueSnifferm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}

//----------------------------------------------------END Bluetooth MENU--------------------------------------------------------------------



//----------------------------------------------------nRF24 MENU--------------------------------------------------------------------

void nrfMenu() {
  if (nRF24.getInit()) {
    nrfm = new Menu(3.0, "nRF24", true, select, move);
    nrfm->init();
    nrfm->addSection("Back", '\0', []() {
      activem = nrfm->getParentMenu();
      dis.renderMenu();
    });
    nrfm->addSection("Jammer", '\0', []() {
      activem = nrfJammerm;
      nRF24.init();
      activem->setSelectedIndex(0);
      dis.renderNRFJammer();
      Attack.nRF24Jammer();
    });
  }
}
void nrfJammerMenu() {
  if (nRF24.getInit()) {
    nrfJammerm = new Menu(3.0, "Jammer", false, select, move);
    nrfJammerm->init();
    nrfJammerm->addSection("Back", '\0', []() {
      activem = nrfJammerm->getParentMenu();
      dis.renderMenu();
      Attack.setMode(1, false);
    });
  }
}

//----------------------------------------------------END nRF24 MENU--------------------------------------------------------------------



//----------------------------------------------------Settings MENU--------------------------------------------------------------------

void settMenu() {
  settm = new Menu(3.0, "Settings", true, select, move);
  settm->init();
  settm->addSection("Back", '\0', []() {
    activem = settm->getParentMenu();
    dis.renderMenu();
  });
  settm->addSection("Set UI-color", '\0', []() {
    activem = settColorm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
  settm->addSection("Set Icons-set", '\0', []() {
    activem = settIconsm;
    activem->setSelectedIndex(0);
    dis.renderMenu();
  });
}

//----------------------------------------------------END Settings MENU--------------------------------------------------------------------

//----------------------------------------------------END INIT MENUS--------------------------------------------------------------------