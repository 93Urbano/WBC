#include "RTC.hpp"
#include "SD.hpp"
#include "UART.hpp"
#include "I2C.hpp"
#include "BLE.hpp"
#include "CAN.hpp"

uint8_t LEDpin = 0;
bool LEDstate = 0;

void setup() 
{
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,LOW);
  UARTInit();
  I2CInit();
  RTCInit();
  SDInit();
  BLEInit();
  CANInit();
  
}

void loop() 
{
  BLETest();
  SDTest();
  UARTTest();
  
  delay(5000);
  LEDstate = !LEDstate;
  digitalWrite(LEDpin,LEDstate);
}
