
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "time.h"
#include <Wire.h>
#include <FaBoRTC_PCF2129.h>
#include <Adafruit_INA219.h>
#include <AuxillaryVoltageArray.h>
#include <DigitalPorts.h>
#include <FS.h>
#include <SD.h>
#include <sd_diskio.h>
#include <sd_defines.h>
#include <SPI.h>

#define I2C_SDA 17 // sda pin
#define I2C_SCL 16 // scl pin

// create objects for each power monitor 
Adafruit_INA219 ina219_0(0x40); //U1:GND,GND //UX:A2,A1
Adafruit_INA219 ina219_1(0x41); //U2:VCC,GND
Adafruit_INA219 ina219_2(0x42); //U3:SDA,GND
Adafruit_INA219 ina219_3(0x43); //U4:SCL,GND

// object to represent the 8 auxillary voltage terminals
AuxillaryVoltageArray auxillaryVoltageArray;

// object to represent the 8 digial ports
DigitalPorts digitalPorts;

// realtime clock 
FaBoRTC_PCF2129 faboRTC;
const char* ntpServer = "pool.ntp.org"; // server to get internet time
const long  gmtOffset_sec = -3600 * 8;    // -8 hours offset for Pacific Standard Time
const int   daylightOffset_sec = 3600;
const char* ssid = "Linksys20977";      // Enter your WIFI network name!
const char* password = "dsfg9t2j7a";    // Enter your WIFI network ID!

//// bluetooth low energy
//BLEServer* pServer = NULL;
//BLECharacteristic* pCharacteristic = NULL;
//BLECharacteristic* pCharacteristic_2 = NULL;
//bool deviceConnected;
//bool oldDeviceConnected;
//std::string tx_string;
//char buffer[50];    // buffer for string to TX via BLE characteristic.
//#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
//#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"
//#define CHARACTERISTIC_2_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a9"
//
//class MyServerCallbacks : public BLEServerCallbacks {
//    void onConnect(BLEServer* pServer) {
//        deviceConnected = true;
//    }
//
//    void onDisconnect(BLEServer* pServer) {
//        deviceConnected = false;
//    }
//};

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   SETUP    //////////////////////////////////////////////////////////
void setup() {
    /// SERIAL ///
    Serial.begin(115200);
    ///// I2C bus ///
    Wire.begin(I2C_SDA, I2C_SCL, 100000);
    /// DIGITAL PORTS ///
    digitalPorts.writeData(0x0);    // turn all digital ports off to start
    /// begin the INA219 power monitors ///
    ina219_0.begin();
    ina219_1.begin();
    ina219_2.begin();
    ina219_3.begin();
    ///  RTC ///
    Serial.println("Checking I2C device...");
    if (faboRTC.searchDevice()) {
        Serial.println("configuring FaBo RTC I2C Brick");
        faboRTC.configure();
    }
    else {
        Serial.println("device not found");
        while (1);
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////   Bluetooth Low Energy Initialization  ////////////////////////
    //// Create the BLE Device
    //BLEDevice::init("ESP32");

    //// Create the BLE Server
    //pServer = BLEDevice::createServer();
    //pServer->setCallbacks(new MyServerCallbacks());

    //// Create the BLE Service
    //BLEService* pService = pServer->createService(SERVICE_UUID);

    //// Create a BLE Characteristic
    //pCharacteristic = pService->createCharacteristic(
    //    CHARACTERISTIC_UUID,
    //    BLECharacteristic::PROPERTY_READ |
    //    BLECharacteristic::PROPERTY_WRITE |
    //    BLECharacteristic::PROPERTY_NOTIFY |
    //    BLECharacteristic::PROPERTY_INDICATE
    //);

    //// Create another BLE Characteristic
    //pCharacteristic_2 = pService->createCharacteristic(
    //    CHARACTERISTIC_UUID,
    //    BLECharacteristic::PROPERTY_READ |
    //    BLECharacteristic::PROPERTY_WRITE |
    //    BLECharacteristic::PROPERTY_NOTIFY |
    //    BLECharacteristic::PROPERTY_INDICATE
    //);

    //// Create a BLE Descriptor
    //pCharacteristic->addDescriptor(new BLE2902());

    //// Start the service
    //pService->start();

    //// Start advertising
    //BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    //pAdvertising->addServiceUUID(SERVICE_UUID);
    //pAdvertising->setScanResponse(false);
    //pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    //BLEDevice::startAdvertising();
    //Serial.println("Waiting a client connection to notify...");

    ///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////   RTC: Get Internet Time  ////////////////////////////////
    //////connect to WiFi
    //Serial.printf("Connecting to %s ", ssid);
    //WiFi.begin(ssid, password);
    //while (WiFi.status() != WL_CONNECTED) {
    //    delay(500);
    //    Serial.print(".");
    //}
    //Serial.println(" CONNECTED");

    ////init and get the time
    ////configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    ////printLocalTime();

    ////disconnect WiFi as it's no longer needed
    //WiFi.disconnect(true);
    //WiFi.mode(WIFI_OFF);

    //struct tm timeinfo;
    //getLocalTime(&timeinfo);

    ////// Year and month must be set manually ///
    ////faboRTC.setDate(timeinfo.tm_year, timeinfo.tm_mon, .... does not work

    // faboRTC.setDate(2020, 3, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    // faboRTC.setDate(2020, 3, 13, 4, 28, 0);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////   SD Card Initialization  /////////////////////////////////////
    if (!SD.begin()) {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    }
    else {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// generate report of the setup phase
// write report to SD card


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   LOOP     /////////////////////////////////////////////////////////
void loop() {

    // read the auxillary voltage array
    auxillaryVoltageArray.readAuxVoltage();
    // read the four power monitors
    ina219_0.readDevice();
    ina219_1.readDevice();
    ina219_2.readDevice();
    ina219_3.readDevice();
    DateTime now = faboRTC.now();

    // print status of SCADA board
    Serial.print("Time: ");
    Serial.println(now.toString());
    Serial.print("P0:");
    Serial.print(ina219_0.toString());
    Serial.print( "\tAuxVoltage:");
    Serial.println(auxillaryVoltageArray.toString());
    Serial.print("P1:");
    Serial.print(ina219_1.toString());
    Serial.print("\tDigitalPorts:");
    Serial.println(digitalPorts.data);
    Serial.print("P2:");
    Serial.println(ina219_2.toString());
    Serial.print("P3:");
    Serial.println(ina219_3.toString());
    Serial.println();



    // main delay  //
    delay(500);

    ///////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////   Bluetooth Low Energy    ////////////////////////////////

    // notify changed value
    //if (deviceConnected) {
    //    tx_string = sprintf(buffer, "%f", ina219_0.getBusVoltage_V());   // make string for transmission
    //    pCharacteristic->setValue(buffer);
    //    pCharacteristic->notify();
    //    delay(100); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    //}
    //// disconnecting
    //if (!deviceConnected && oldDeviceConnected) {
    //    delay(500); // give the bluetooth stack the chance to get things ready
    //    pServer->startAdvertising(); // restart advertising
    //    Serial.println("start advertising");
    //    oldDeviceConnected = deviceConnected;
    //}
    //// connecting
    //if (deviceConnected && !oldDeviceConnected) {
    //    // do stuff here on connecting
    //    oldDeviceConnected = deviceConnected;
    //}

}
