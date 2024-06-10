//************************************//
//              RTC.cpp               //
//************************************// 

#include "RTCmod.hpp" 
#include "I2C.hpp"

//RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String DateNameFileString;

void RTCmodule::RTCInit()
{    
    #ifndef ESP8266
        while (!Serial); // wait for serial port to connect. Needed for native USB
    #endif

    if (! rtc.begin()) 
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1) delay(10);
    }

    if (rtc.lostPower()) 
    {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
}

void RTCmodule::RTCGetDateTime()
{
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
}

String RTCmodule::GetTimeString()
{
  DateTime now = rtc.now(); // Obtiene la fecha y hora actual del módulo RTC
  
  // Construye un string con el formato "HH:MM:SS"
  String hora = String(now.hour());
  String minutos = String(now.minute());
  String segundos = String(now.second());

  // Asegura que la hora, minutos y segundos tengan dos dígitos
  if (hora.length() == 1) hora = '0' + hora;
  if (minutos.length() == 1) minutos = '0' + minutos;
  if (segundos.length() == 1) segundos = '0' + segundos;

  return hora + ":" + minutos + ":" + segundos;
}

String RTCmodule::GetDateFileName() 
{
  DateTime now = rtc.now(); // Obtiene la fecha y hora actual del módulo RTC

  // Construye un string con el formato "YYMMDDHH"
  String dia = String(now.day());
  String mes = String(now.month());
  String ano = String(now.year() - 2000); // Restamos 2000 ya que el año se almacena como los últimos dos dígitos
  String hora = String(now.hour());

  // Asegura que el día, mes, año, hora, minutos y segundos tengan dos dígitos
  if (dia.length() == 1) dia = '0' + dia;
  if (mes.length() == 1) mes = '0' + mes;
  if (ano.length() == 1) ano = '0' + ano;
  if (hora.length() == 1) hora = '0' + hora;

  return "/" + ano + mes + dia + hora + ".txt";
}