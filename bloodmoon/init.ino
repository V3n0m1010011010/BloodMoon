void init() {
  tft.init();
  Serial.begin(115200);
  mainMenu();
  mainm->setSubMenu(scanm);
  activem = mainm;
  activem->render();
}