#include "RTCmod.hpp"
#include "SDmod.hpp"
// #include "UART.hpp"
#include "I2C.hpp"
#include "BLE.hpp"
// #include "CAN.hpp"
#include "WiFiserver.h"

uint LEDpin = 0;
bool LEDstate = 0;

WiFiserver wifi1;
BLEmodule BLE1;
SDmodule SD1;
RTCmodule RTC1;
I2Cmodule I2C1;

void setup() 
{
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,LOW);
  // UARTInit();
  I2C1.I2CInit();
  RTC1.RTCInit();
  SD1.SDinit();
  BLE1.BLEInit();
  // CANInit();
  //wifi1.WiFiserverInit(); //comentada por que usa demasiada memoria, quiza tengo que pasar el html a su extension o hacer la app fuera
}

void loop() 
{
  BLE1.BLETest();
  SD1.SDTest();
  RTC1.RTCGetDateTime();
  // UARTTest();
  
  delay(5000);
  LEDstate = !LEDstate;
  digitalWrite(LEDpin,LEDstate);
}
