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

  #define move 47
  #define select 0

  //// -----------------




  ////  nRF24l01 modul SPI pins:
  
  #define CE_PIN 34
  #define SCK_PIN 17
  #define MISO_PIN 16
  #define MOSI_PIN 18
  #define CSN_PIN 33
  
  //-------------------------

  #define backlightPin 10

#endif

#ifdef LILYGO_T_DISPLAY_S3_LCD

  ////  display settings:

  #define width 170
  #define height 320

  //// -----------------



  ////  button pins:

  #define move 14
  #define select 0

  //// -----------------




  ////  nRF24l01 modul SPI pins:
  
  #define CE_PIN -1
  #define SCK_PIN -1
  #define MISO_PIN -1
  #define MOSI_PIN -1
  #define CSN_PIN -1
  
  //-------------------------

  #define backlightPin 38

#endif


#define pwmChannel 0
#define pwmFrequency 5000

int currentIconsSet = 1;