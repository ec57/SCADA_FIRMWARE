# SCADA_FIRWARE
repo for capstone project: SCADA module

SETUP:  Enter 'https://dl.espressif.com/dl/package_esp32_index.json' into the “Additional Board Manager URLs” field in Arduino IDE File > Preferences. In Arduino IDE: Tools > Board > Boards Manager… Search for ESP32 and press install button for the “ESP32 by Espressif Systems“ . For more information see: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/ .

LIBRARIES:  Place the following libraries in the Arduino IDE 'Libraries' folder:
'Libraries' folder
  -Adafruit_INA219
    -Adafruit_INA219.h
    -Adafruit_INA219.cpp
  -AuxillaryVoltageArray
    -AuxillaryVoltageArray.h
    -AuxillaryVoltageArray.cpp
  -DigitalPorts
    -DigitalPorts.h
    -DigitalPorts.cpp
  -FaBoRTC_PCF2129
    -FaBoRTC_PCF2129.h
    -FaBoRTC_PCF2129.cpp

CODE:  Place the 'SCADA_WORKBENCH_2.ino' file in a folder named 'SCADA_WORKBENCH_2' inside the Arduino folder in 'Documents' folder
'Documents' folder
  -Arduino
    -SCADA_WORKBENCH_2
      -SCADA_WORKBENCH_2.ino
