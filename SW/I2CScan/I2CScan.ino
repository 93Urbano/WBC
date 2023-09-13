#include "Wire.h"

#define I2C_SDA 1
#define I2C_SCL 2


 
void setup(){ 
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.begin(115200);
} 
 
void loop(){
    byte error, address;
    int nDevices;
 
    Serial.println("Scanning...");
 
    nDevices = 0;
    for(address = 8; address < 127; address++ ){
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
 
        if (error == 0){
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" !");
 
            nDevices++;
        }
        else if (error == 4) {
            Serial.print("Unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address,HEX);
        } 
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
 
    delay(5000);
}