/*###################################################################
*                        Settings file
*
*            Choose the board by comment/uncomment
###################################################################*/

//#define LILYGO_T_QT_PRO
#define LILYGO_T_DISPLAY_S3_LCD

//###################################################################








/*###################################################################
*                        Board configurations
*
###################################################################*/



#ifdef LILYGO_T_QT_PRO

  ////  display settings:

  #define width 128
  #define height 128

  //// -----------------



  ////  button pins:

  #define move_btn 47
  #define select_btn 0

  //// -----------------




  ////  nRF24l01 modul SPI pins:
  
  #define CE_PIN 34
  #define SCK_PIN 17
  #define MISO_PIN 16
  #define MOSI_PIN 18
  #define CSN_PIN 33
  
  //-------------------------

  ////  micro sd card modul defines:

  #define sd_SCK_PIN -1
  #define sd_MISO_PIN -1
  #define sd_MOSI_PIN -1
  #define sd_CS_PIN -1
  
  #define PCAP_PATH "/handshake_once.pcap"
  
  //-------------------------

  #define backlightPin 10

#endif

#ifdef LILYGO_T_DISPLAY_S3_LCD

  ////  display settings:

  #define width 170
  #define height 320

  //// -----------------



  ////  button pins:

  #define move_btn 14
  #define select_btn 0

  //// -----------------




  ////  nRF24l01 modul SPI pins:
  
  #define CE_PIN 1
  #define SCK_PIN 12
  #define MISO_PIN 13
  #define MOSI_PIN 11
  #define CSN_PIN 2

  ////  micro sd card modul defines:

  #define sd_SCK_PIN 12
  #define sd_MISO_PIN 13
  #define sd_MOSI_PIN 11
  #define sd_CS_PIN 10

  #define PCAP_PATH "/handshake_once.pcap"
  
  //-------------------------

  #define backlightPin 38

#endif


#define pwmChannel 0
#define pwmFrequency 5000

int currentIconsSet = 1;