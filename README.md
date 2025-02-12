# BloodMoon – ESP32-S3 Wireless Toolkit

BloodMoon is a custom firmware for the LilyGO T-QT Pro (ESP32-S3), inspired by Marauder. It provides wireless scanning and attack functionalities for 2.4GHz networks.

<br>

### Features:
#
- *Wi-Fi Scanner* – Detects nearby access points or Stations and displays essential network information.

- *2.4GHz Jamming* – Uses an nRF24L01 module to disrupt 2.4GHz wireless signals.



<br>

### Hardware Requirements:
#
- *LilyGO T-QT Pro* (ESP32-S3)

- *nRF24L01 module* (for jamming functionality) Pins can be modified in `settings.h`

<br>

### Software Requirements:
#
- *board-manager: `esp32` version 2.0.10*
  
- *`TFT_eSPI` version 2.5.4* (for t-qt pro copy `User_Setup.h` file in TFT_eSPI folder)

- *`EasyButton` version 2.0.3*

- *`RF24` version 1.4.9*

<br>

### To install via Arduino IDE:
#
- With any text editor, open `C:\Users\<USERNAME>\AppDate\Local\Arduino15\packages\esp32\hardware\esp32\2.0.3\platform.txt`
- Add `-w` to the following compiler setting for esp32s3
  - `build.extra_flags.esp32s3`
- Add `-zmuldefs` to the following compiler setting for esp32s3
  - `compiler.c.elf.libs.esp32s3`

<br>


### In development:
#
- [ ] display scanned APs/STs - info (SSID, BSSID, Channel)
- [ ] Multiselecting scanned APs/STs
- [ ] Deauthentification-attack
- [ ] Beacon-spamming

<br>

> [!Note]
> This project is still in development, and more features will be added over time. Contributions and feedback are welcome! 

>[!Caution]
> This firmware is intended for educational and research purposes only. I do not take any responsibility for misuse or illegal activities involving this software.
