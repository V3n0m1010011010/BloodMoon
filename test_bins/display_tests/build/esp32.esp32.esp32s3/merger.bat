@echo off
setlocal enabledelayedexpansion

:: ##############################################
:: ### KONFIGURATION - ANPASSEN FALLS NÖTIG  ###
:: ##############################################

set ESPTOOL_PATH="C:\Users\Crazy\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe"
set BOOT_APP0="C:\Users\Crazy\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.10\tools\partitions\boot_app0.bin"

:: ##############################################
:: ### AB HIER NICHTS MEHR ÄNDERN! ###
:: ##############################################

:: Aktueller Ordner des Scripts
set SCRIPT_DIR=%~dp0

:: Alle .ino.bin Dateien im Verzeichnis durchgehen
for %%F in ("%SCRIPT_DIR%*.ino.bin") do (
    set "FIRMWARE=%%F"
    set "NAME=%%~nF"
    
    :: ".ino" aus dem Namen entfernen
    set "BASE_NAME=!NAME:.ino=!"

    set "BOOTLOADER=%SCRIPT_DIR%!BASE_NAME!.ino.bootloader.bin"
    set "PARTITIONS=%SCRIPT_DIR%!BASE_NAME!.ino.partitions.bin"
    set "OUTPUT_FILE=%SCRIPT_DIR%!BASE_NAME!.bin"

    echo Merging firmware components for !BASE_NAME!...
    %ESPTOOL_PATH% --chip esp32s3 merge_bin -o "!OUTPUT_FILE!" ^
      0x0 "!BOOTLOADER!" ^
      0x8000 "!PARTITIONS!" ^
      0xe000 "%BOOT_APP0%" ^
      0x10000 "!FIRMWARE!"

    if errorlevel 1 (
        echo Fehler beim Mergen von !BASE_NAME!! Überprüfe:
        echo - Existieren alle benötigten Dateien im Ordner?
        echo - Stimmt der Pfad zu boot_app0.bin?
        pause
        exit /b 1
    )

    echo Fertig! Gemergte Firmware: !OUTPUT_FILE!
)

timeout /t 1 /nobreak >nul
exit
