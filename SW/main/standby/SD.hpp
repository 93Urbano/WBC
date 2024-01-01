//************************************//
//              SD.hpp                //
//************************************// 

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK  36
#define MISO  37
#define MOSI  35
#define CS  38

SPIClass spi = SPIClass(HSPI);