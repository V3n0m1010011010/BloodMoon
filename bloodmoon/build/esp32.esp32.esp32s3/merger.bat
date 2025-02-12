@echo off
setlocal enabledelayedexpansion

:: ##############################################
:: ### KONFIGURATION - ANPASSEN FALLS NÖTIG ###
:: ##############################################

set ESPTOOL_PATH="C:\Users\Crazy\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe"
set BOOT_APP0="C:\Users\Crazy\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.10\tools\partitions\boot_app0.bin"

:: ##############################################
:: ### AB HIER NICHTS MEHR ÄNDERN! ###
:: ##############################################

:: Aktueller Ordner des Scripts
set SCRIPT_DIR=%~dp0

:: Pfade zu den kompilierten Dateien (im gleichen Ordner wie das Script)
set BOOTLOADER="%SCRIPT_DIR%bloodmoon.ino.bootloader.bin"
set PARTITIONS="%SCRIPT_DIR%bloodmoon.ino.partitions.bin"
set FIRMWARE="%SCRIPT_DIR%bloodmoon.ino.bin"

:: Output-Datei im aktuellen Ordner
set OUTPUT_FILE="%SCRIPT_DIR%bloodmoon.bin"

echo Merging firmware components...
%ESPTOOL_PATH% --chip esp32s3 merge_bin -o %OUTPUT_FILE% ^
  0x0 %BOOTLOADER% ^
  0x8000 %PARTITIONS% ^
  0xe000 %BOOT_APP0% ^
  0x10000 %FIRMWARE%

if errorlevel 1 (
    echo Fehler beim Mergen! Überprüfe:
    echo - Existieren alle Dateien im Ordner?
    echo - Stimmt der Pfad zu boot_app0.bin (ggf. Version in Zeile 6 anpassen)?
    pause
    exit /b 1
)

echo Fertig! Gemergte Firmware: %OUTPUT_FILE%
pause