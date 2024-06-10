//************************************//
//              SD.hpp                //
//************************************// 

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Arduino.h>

#define SCK  36
#define MISO  37
#define MOSI  35
#define CS  38

class SDmodule
{
    private: 
        static void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
        static void createDir(fs::FS &fs, const char * path);
        static void removeDir(fs::FS &fs, const char * path);
        static void readFile(fs::FS &fs, const char * path);
        static void writeFile(fs::FS &fs, const char * path, const char * message);
        static void appendFile(fs::FS &fs, const char * path, const char * message);
        static void renameFile(fs::FS &fs, const char * path1, const char * path2);
        static void deleteFile(fs::FS &fs, const char * path);
        static void testFileIO(fs::FS &fs, const char * path);
    public:
        static void SDinit();
        static void SDTest();
};