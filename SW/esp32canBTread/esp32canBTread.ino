#include <ESP32CAN.h>
#include <CAN_config.h>
CAN_device_t CAN_cfg;

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//Cambio en libreria por
//#define   CAN_SPEED_500KBPS=50;
void setup() {
    Serial.begin(115200);
    CAN_cfg.speed=CAN_SPEED_500KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_5;
    CAN_cfg.rx_pin_id = GPIO_NUM_4;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //start CAN Module
    ESP32Can.CANInit();

    SerialBT.begin("CAN MONITOR"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
    delay(5000);
    SerialBT.print(" BT printing.. ");
}

void loop() {
    CAN_frame_t rx_frame;
    //receive next CAN frame from queue
    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

      //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std){
        //printf("New standard frame");
        printf("S: ");
        SerialBT.print("S: ");
      }
      else{
        //printf("New extended frame");
        printf("E: ");
        SerialBT.print("E: ");
      }

      if(rx_frame.FIR.B.RTR==CAN_RTR){
        printf(" RTR from 0x%08x DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        //SerialBT.print(" RTR from 0x%08x DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      }
      else{
        SerialBT.print(rx_frame.MsgID,HEX);
        SerialBT.print(" "); 
        SerialBT.print(rx_frame.FIR.B.DLC,HEX);
        SerialBT.print(" ");
        //mcp2515.readMessage(&canMsg);
        for (int i = 0; i<rx_frame.FIR.B.DLC; i++) {
          SerialBT.print(rx_frame.data.u8[i],HEX);
          SerialBT.print(" ");
        }
        SerialBT.println(" ");
        /*
        printf(" 0x%08x, %d",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          printf("%h\t", (char)rx_frame.data.u8[i]);
        }
        printf("\n");
        */
      }
    }
}
