//************************************//
//              I2C.hpp               //
//************************************// 

#include "Wire.h"
#include <Arduino.h>

#define I2C_SDA 1
#define I2C_SCL 2

class I2Cmodule
{
    private:
        static void I2CScan();
    public:
        static void I2CInit();
};