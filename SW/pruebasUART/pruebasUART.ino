#define RXD2 2
#define TXD2 3

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  Serial.println("Serial 1 INIT");
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial2.println("Serial 2 INIT");
  Serial2.println("Serial Txd is on pin: "+String(TX));
  Serial2.println("Serial Rxd is on pin: "+String(RX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
}

void loop() { //Choose Serial1 or Serial2 as required
 /* while (Serial2.available()) {
    Serial2.println("hola");
  }*/
  Serial2.println("hola SOY 2");
    while (Serial.available()) {
    Serial.println("hola");
  }
  Serial.println("hola SOY 1");
}