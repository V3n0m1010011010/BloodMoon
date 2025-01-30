void init() {
  tft.init();
  Serial.begin(115200);
  mainMenu();
  wifiMenu();
  blueMenu();
  settMenu();
  mainm->setSubMenu(wifim);
  mainm->setSubMenu(bluem);
  mainm->setSubMenu(settm);
  activem = mainm;
  activem->render();
}