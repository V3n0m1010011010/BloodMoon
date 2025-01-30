//----------------------------------------------------MAIN MENU--------------------------------------------------------------------

void mainMenu() {
  mainm = new Menu(0.0, "Main Menu", true, select, move, nullptr, nullptr, true);
  mainm->init();
  mainm->addSection("Wifi", '\x21', []() {
    activem = wifim;
    activem->setSelectedIndex(0);
    activem->render();
  });
  mainm->addSection("Bluetooth", '\x22', []() {
    activem = bluem;
    activem->setSelectedIndex(0);
    activem->render();
  });
  mainm->addSection("Settings", '\x23', []() {
    activem = settm;
    activem->setSelectedIndex(0);
    activem->render();
  });
}

//----------------------------------------------------END MAIN MENU--------------------------------------------------------------------


//----------------------------------------------------WIFI MENU--------------------------------------------------------------------

void wifiMenu() {
  wifim = new Menu(1.0, "WIFI", true, select, move);
  wifim->init();
  wifim->addSection("Back", '\0', []() {
    activem = wifim->getParentMenu();
    activem->setSelectedIndex(0);
    activem->render();
  });
}

//----------------------------------------------------END WIFI MENU--------------------------------------------------------------------


//----------------------------------------------------Bluetooth MENU--------------------------------------------------------------------

void blueMenu() {
  bluem = new Menu(2.0, "BLT", true, select, move);
  bluem->init();
  bluem->addSection("Back", '\0', []() {
    activem = bluem->getParentMenu();
    activem->setSelectedIndex(0);
    activem->render();
  });
}

//----------------------------------------------------END Bluetooth MENU--------------------------------------------------------------------


//----------------------------------------------------Settings MENU--------------------------------------------------------------------

void settMenu() {
  settm = new Menu(3.0, "Settings", true, select, move);
  settm->init();
  settm->addSection("Back", '\0' []() {
    activem = settm->getParentMenu();
    activem->setSelectedIndex(0);
    activem->render();
  });
}

//----------------------------------------------------END Settings MENU--------------------------------------------------------------------