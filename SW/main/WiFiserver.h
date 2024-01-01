//************************************//
//         WiFiserver.h               //
//************************************// 
#ifndef WIFISERVER_H
#define WIFISERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

class WiFiserver
{
  private:
    static const char* ssid;
    static const char* password;

  public:
    static int rpmValue;
    static int tpsPercentage;
    static int brakePercentage;
    static int ClutchState;
    static uint CANfrequency;
    static bool CANactive;
    static uint UARTBauds;
    static bool UARTactive;
    static bool CANShow;
    static bool UARTShow;
    static bool RTCShow;
    static bool CANSave;
    static bool UARTSave;
    static bool RTCSave;

    static uint ledPin;

    static void WiFiserverInit();
    static void handleRoot(AsyncWebServerRequest *request);
    static void handleDataLog(AsyncWebServerRequest *request);
    static void handleConfig(AsyncWebServerRequest *request);
    static void handleData(AsyncWebServerRequest *request);
    static void handleControl(AsyncWebServerRequest *request);
    static void handleCANControl(AsyncWebServerRequest *request);
    static void handleUARTControl(AsyncWebServerRequest *request);
    static void handleBTControl(AsyncWebServerRequest *request);
    static void handleDATAControl(AsyncWebServerRequest *request);
    static void handleDATETIME(AsyncWebServerRequest *request);
};

#endif