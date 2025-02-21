void initAll() {
  tft.init();
  Serial.begin(115200);
  mySPI.begin(17, 16, 18, 33);
  nRF24.init();
  Wifi.init();
  ctr.init();
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
  if (nRF24.getInit()) {
    mainm->setSubMenu(nrfm);
    nrfm->setSubMenu(nrfJammerm);
  }
  mainm->setSubMenu(settm);
  activem = mainm;
  dis.renderAll();
  lastActivityTime = millis();
}

//----------------------------------------------------START INIT MENUS--------------------------------------------------------------------

//----------------------------------------------------MAIN MENU--------------------------------------------------------------------

void mainMenu() {
  mainm = new Menu("normal", "Main Menu", true, nullptr, nullptr, true);
  mainm->addSection("WiFi", '\x21', []() {
    activem = wifim;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  mainm->addSection("Bluetooth", '\x22', []() {
    activem = bluem;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  if (nRF24.getInit()) {
    mainm->addSection("nRF24", '\x25', []() {
      activem = nrfm;
      activem->setSelectedIndex(0);
      dis.renderAll();
    });
  }
  mainm->addSection("Settings", '\x23', []() {
    activem = settm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}

//----------------------------------------------------END MAIN MENU--------------------------------------------------------------------


//----------------------------------------------------WIFI MENU--------------------------------------------------------------------

void wifiMenu() {
  wifim = new Menu("normal", "WiFi", true);
  wifim->addSection("Back", '\0', []() {
    activem = wifim->getParentMenu();
    dis.renderAll();
  });
  wifim->addSection("Scanning", '\0', []() {
    activem = wifiScanm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  wifim->addSection("Attack", '\0', []() {
    activem = wifiAttackm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  wifim->addSection("Gerneral", '\0', []() {
    activem = wifiGeneralm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  wifim->addSection("Sniffer", '\0', []() {
    activem = wifiSnifferm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}

void wifiScanMenu() {
  wifiScanm = new Menu("normal", "Scan", true);
  wifiScanm->addSection("Back", '\0', []() {
    activem = wifiScanm->getParentMenu();
    dis.renderAll();
  });
  wifiScanm->addSection("Scan Ap's", '\0', []() {
    activem = wifiScanApsm;
    dis.renderApScanMenu();
    Wifi.scanAps();
  });
  wifiScanm->addSection("Scan St's", '\0', []() {
    activem = wifiScanStsm;
    dis.renderStScanMenu();
    Wifi.scanSts();
  });
}
void wifiScanApMenu() {
  wifiScanApsm = new Menu("scan", "Scanning", false);
  wifiScanApsm->addSection("Back", '\0', []() {
    Wifi.stopWifiScan();
    activem = wifiScanApsm->getParentMenu();
    dis.renderAll();
  });
}
void wifiScanStMenu() {
  wifiScanStsm = new Menu("scan", "Scanning", false);
  wifiScanStsm->addSection("Back", '\0', []() {
    Wifi.stopWifiScan();
    activem = wifiScanStsm->getParentMenu();
    dis.renderAll();
  });
}
void wifiAttackMenu() {
  wifiAttackm = new Menu("normal", "Attacks", true);
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    dis.renderAll();
  });
  wifiAttackm->addSection("Deauth", '\0', []() {
    activem = wifiAttackDeauthm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}
void wifiAttackDeauthMenu() {
  wifiAttackm = new Menu("deauther", "Deauther", true);
  wifiAttackm->addSection("Back", '\0', []() {
    activem = wifiAttackm->getParentMenu();
    dis.renderAll();
  });
  wifiAttackm->addSection("Start", '\0', []() {
    if (Attack.getMode(0) == true) {
      activem->setTitle("Deauther");
      activem->setSection("Start", 1);
      dis.renderAll();
    } else {
      activem->setTitle("Deauthing");
      activem->setSection("Stop", 1);
      dis.renderAll();
      Attack.deauth();
    }
  });
}

void wifiGeneralMenu() {
  wifiGeneralm = new Menu("normal", "General", true);
  wifiGeneralm->addSection("Back", '\0', []() {
    activem = wifiGeneralm->getParentMenu();
    dis.renderAll();
  });
  wifiGeneralm->addSection("Select", '\0', []() {
    activem = wifiGeneralSelectm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}
void wifiGeneralSelectMenu() {
  wifiGeneralSelectm = new Menu("normal", "Select", true);
  wifiGeneralSelectm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectm->getParentMenu();
    dis.renderAll();
  });
  wifiGeneralSelectm->addSection("AccessPoints", '\0', []() {
    wifiGeneralSelectApMenu();
    activem = wifiGeneralSelectApm;
    activem->setSelectedIndex(0);
    dis.renderApSelectMenu();
  });
  wifiGeneralSelectm->addSection("Stations", '\0', []() {
    wifiGeneralSelectStMenu();
    activem = wifiGeneralSelectStm;
    activem->setSelectedIndex(0);
    dis.renderStSelectMenu();
  });
}

void wifiGeneralSelectApMenu() {
  wifiGeneralSelectApm = new Menu("apSelect", "AP's", true);
  wifiGeneralSelectApm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectApm->getParentMenu();
    dis.renderAll();
  });
  auto list = Wifi.getApList();
  Serial.println(list.size());
  for (int i = 0; i < list.size(); i++) {
    wifiGeneralSelectApm->addSection(list[i].ssid.c_str(), '\0');
  }
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectApm);
}


void wifiGeneralSelectStMenu() {
  wifiGeneralSelectStm = new Menu("stSelect", "ST's", true);
  wifiGeneralSelectStm->addSection("Back", '\0', []() {
    activem = wifiGeneralSelectStm->getParentMenu();
    dis.renderAll();
  });
  wifiGeneralSelectm->setSubMenu(wifiGeneralSelectStm);
}
//----------------------------------------------------END WIFI MENU--------------------------------------------------------------------


//----------------------------------------------------Bluetooth MENU--------------------------------------------------------------------

void blueMenu() {
  bluem = new Menu("normal", "BLT", true);
  bluem->addSection("Back", '\0', []() {
    activem = bluem->getParentMenu();
    dis.renderAll();
  });
  bluem->addSection("Blue-Scan", '\0', []() {
    activem = blueScanm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  bluem->addSection("Blue-Attacks", '\0', []() {
    activem = blueAttackm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  bluem->addSection("Blue-Sniffer", '\0', []() {
    activem = blueSnifferm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}

//----------------------------------------------------END Bluetooth MENU--------------------------------------------------------------------



//----------------------------------------------------nRF24 MENU--------------------------------------------------------------------

void nrfMenu() {
  if (nRF24.getInit()) {
    nrfm = new Menu("normal", "nRF24", true);
    nrfm->addSection("Back", '\0', []() {
      activem = nrfm->getParentMenu();
      dis.renderAll();
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
    nrfJammerm = new Menu("jammer", "Jammer", false);
    nrfJammerm->addSection("Back", '\0', []() {
      activem = nrfJammerm->getParentMenu();
      dis.renderAll();
      Attack.setMode(1, false);
    });
  }
}

//----------------------------------------------------END nRF24 MENU--------------------------------------------------------------------



//----------------------------------------------------Settings MENU--------------------------------------------------------------------

void settMenu() {
  settm = new Menu("normal", "Settings", true);
  settm->addSection("Back", '\0', []() {
    activem = settm->getParentMenu();
    dis.renderAll();
  });
  settm->addSection("Set UI-color", '\0', []() {
    activem = settColorm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
  settm->addSection("Set Icons-set", '\0', []() {
    activem = settIconsm;
    activem->setSelectedIndex(0);
    dis.renderAll();
  });
}

//----------------------------------------------------END Settings MENU--------------------------------------------------------------------

//----------------------------------------------------END INIT MENUS--------------------------------------------------------------------