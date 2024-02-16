// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define I2C_SDA 1
#define I2C_SCL 2

String DateNameFileString;

//SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK  36
#define MISO  37
#define MOSI  35
#define CS  38

SPIClass spi = SPIClass(HSPI);

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
  if (deviceConnected)
  {
    pTxCharacteristic->setValue(s);
    pTxCharacteristic->notify();
  }
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

void RTCInit()
{
  Wire.begin(I2C_SDA, I2C_SCL);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void SDInit()
{
  spi.begin(SCK, MISO, MOSI, CS);
  Serial.println("SD SETUP");

  if (!SD.begin(CS,spi,80000000)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  listDir(SD, "/", 0);
}

String GetTimeString ()
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

String GetDateFileName() {
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
    //SD
    SDInit();
    //RTC
    RTCInit();
}

void loop ()
{
  DateNameFileString = GetDateFileName();
  const char* DateNameFile = DateNameFileString.c_str();
  //Wait for message to be received
  twai_message_t message;
  if (twai_receive(&message, pdMS_TO_TICKS(100)) == ESP_OK) 
  {
    WriteString2BLE("\n");
    appendFile(SD, DateNameFile, "\n");
    if (!(message.rtr)) 
    {
      String Timeget = GetTimeString();
      std::sprintf(buffer, "%s 0x%X ", Timeget.c_str(), message.identifier);
      std::string formattedString(buffer);
      const char* cString = formattedString.c_str();
      WriteString2BLE(const_cast<char*>(cString));
      appendFile(SD, DateNameFile, const_cast<char*>(cString));
      for (int i = 0; i < message.data_length_code; i++) 
      {
          std::sprintf(buffer, "%X ", message.data[i]);
          std::string formattedString(buffer);
          const char* cString = formattedString.c_str();
          WriteString2BLE(const_cast<char*>(cString));
          appendFile(SD, DateNameFile, const_cast<char*>(cString));
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
