void mainMenu() {
  mainm = new Menu(0.0, "Main Menu", true, select, move);
  mainm->init();
  mainm->addSection("Wifi", '\x21', []() {
    activem = scanm;
    activem->setSelectedIndex(0);
    activem->render();
  });
  mainm->addSection("Bluetooth", '\x22', []() {
    activem = scanm;
    activem->setSelectedIndex(0);
    activem->render();
  });
  mainm->addSection("Settings", '\x23', []() {
    activem = scanm;
    activem->setSelectedIndex(0);
    activem->render();
  });
}