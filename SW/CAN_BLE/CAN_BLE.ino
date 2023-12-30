//CAN
#include "driver/gpio.h"
#include "driver/twai.h"

//concatenacion de cadenas
#include <iostream>
#include <cstdio>  

//BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
unsigned char LED=0;
bool ledState=0;
char Str1[20];
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
char buffer[100];

//BLE
class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer) 
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) 
    {
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks 
{
    void onWrite(BLECharacteristic *pCharacteristic) 
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) 
        {
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);
            Serial.println();
            Serial.println("*********");
        }
    }
};

void WriteString2BLE(char *s)
{
    pTxCharacteristic->setValue(s);
    pTxCharacteristic->notify();
}

void CANInit()
{
    //CAN
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_46, GPIO_NUM_3, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) 
    {
        printf("Driver installed\n");
    } 
    else 
    {
        printf("Failed to install driver\n");
        return;
    }
    //Start TWAI driver
    if (twai_start() == ESP_OK) 
    {
        printf("Driver started\n");
    } 
    else 
    {
        printf("Failed to start driver\n");
        return;
    }
}

void BLEInit()
{
    // BLE
    BLEDevice::init("UART Service");
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                        );
    pTxCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID_RX,
                                            BLECharacteristic::PROPERTY_WRITE
                                            );
    pRxCharacteristic->setCallbacks(new MyCallbacks());
    // Start the service
    pService->start();
    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
    pTxCharacteristic->setValue("Hello World");
}

void setup()
{
    //CAN
    CANInit();
    //SERIAL
    Serial.begin(115200);
    //LED
    pinMode(LED, OUTPUT);
    //BLE
    BLEInit();
}

void loop ()
{
    //Wait for message to be received
    twai_message_t message;
    if (twai_receive(&message, pdMS_TO_TICKS(100)) == ESP_OK) 
    {
        printf("Message received\n");
    }

    //Process received message
    if (message.extd) 
    {
        printf("Message is in Extended Format\n");
    } 
    else 
    {
        printf("Message is in Standard Format\n");
    }
  
    if (deviceConnected) 
    {
        WriteString2BLE("\n");
        std::sprintf(buffer, "Rx 2 0x%X ", message.identifier);
        std::string formattedString(buffer);
        const char* cString = formattedString.c_str();
        WriteString2BLE(const_cast<char*>(cString));
        if (!(message.rtr)) 
        {
            for (int i = 0; i < message.data_length_code; i++) 
            {
                std::sprintf(buffer, "%X ", message.data[i]);
                std::string formattedString(buffer);
                const char* cString = formattedString.c_str();
                WriteString2BLE(const_cast<char*>(cString));
            }
            digitalWrite(LED, 1);
        }
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) 
    {
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        digitalWrite(LED, 0);
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) 
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
