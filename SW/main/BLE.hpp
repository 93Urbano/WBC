//************************************//
//              BLE.hpp               //
//************************************// 

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

class BLEmodule
{
    private:
        static BLEServer *pServer;
        static BLECharacteristic * pTxCharacteristic;
    public:
        static bool deviceConnected;
        static bool oldDeviceConnected;
        static uint8_t txValue;
        static unsigned char LED;
        static bool ledState;
        static char Str1[20];

        static void WriteString2BLE(char *s);
        static void BLEInit();
        static void BLETest();
};


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

