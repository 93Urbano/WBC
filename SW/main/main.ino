// #include "RTC.hpp"
#include "SDmod.hpp"
// #include "UART.hpp"
// #include "I2C.hpp"
#include "BLE.hpp"
// #include "CAN.hpp"
#include "WiFiserver.h"

uint LEDpin = 0;
bool LEDstate = 0;

WiFiserver wifi1;
BLEmodule BLE1;
SDmodule SD1;

void setup() 
{
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,LOW);
  // UARTInit();
  // I2CInit();
  // RTCInit();
  SD1.SDinit();
  BLE1.BLEInit();
  // CANInit();
  wifi1.WiFiserverInit();
}

void loop() 
{
  BLE1.BLETest();
  SD1.SDTest();
  // UARTTest();
  
  delay(5000);
  LEDstate = !LEDstate;
  digitalWrite(LEDpin,LEDstate);
}
