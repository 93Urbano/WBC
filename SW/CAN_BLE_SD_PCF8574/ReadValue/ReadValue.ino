#include <Wire.h>

const int pcfAddress = 0x38;

void setup()
{
  Wire.begin();
  Serial.begin(115200);
}

void loop()
{
  short channel = 1;
  byte value = 0;

  // Leer dato de canal 'channel'
  Wire.requestFrom(pcfAddress, 1 << channel);
  if (Wire.available())
  {
    value = Wire.read();
    // Mostrar el valor por puerto serie
    Serial.println(value);
  }
  Wire.endTransmission();


  delay(500);
}