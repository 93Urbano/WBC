#include <ESP32CAN.h>
#include <CAN_config.h>
CAN_device_t CAN_cfg;

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
const unsigned int MAX_MESSAGE_LENGTH = 12;

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
  CANtoSerialBT();
  SerialBTtoCAN();
}


void CANMessage (unsigned int ID, unsigned char DLC, unsigned char DATA1, unsigned char DATA2, unsigned char DATA3, unsigned char DATA4, unsigned char DATA5, unsigned char DATA6, unsigned char DATA7, unsigned char DATA8)
{
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_ext;
  tx_frame.MsgID = ID;
  tx_frame.FIR.B.DLC = DLC;
  tx_frame.data.u8[0] = DATA1;
  tx_frame.data.u8[1] = DATA2;
  tx_frame.data.u8[2] = DATA3;
  tx_frame.data.u8[3] = DATA4;
  tx_frame.data.u8[4] = DATA5;
  tx_frame.data.u8[5] = DATA6;
  tx_frame.data.u8[6] = DATA7;
  tx_frame.data.u8[7] = DATA8;
  ESP32Can.CANWriteFrame(&tx_frame);
}

void SerialBTtoCAN (void)
{
  if ( SerialBT.available() > 0 )
  {
    //Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;
    
    //Read the next available byte in the serial receive buffer
    char inByte = SerialBT.read();
    
    //Message coming in (check not terminating character) and guard for over message size
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
    {
      //Add the incoming byte to our message
      message[message_pos] = inByte;
      message_pos++;
    }
    //Full message received...
    else
    {
      //Add null character to string
      message[message_pos] = '\0';
      
      //Print the message (or do other things)
      Serial.println(message);

      //Sabemos cual es el mensaje, podemos enviar CAN
      //CANMessage(0x320, 4, 25, 32, 45, 56, 0, 0, 0, 0); 
      
      //Reset for the next message
      message_pos = 0;
    }
  }
}



void CANtoSerialBT (void)
{
  CAN_frame_t rx_frame;
  //receive next CAN frame from queue
  if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE)
  {
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
    }
  }
}
