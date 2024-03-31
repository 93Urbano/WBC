//************************************//
//              BLE.cpp               //
//************************************// 

#include "BLE.hpp"

bool BLEmodule::deviceConnected = false;
bool BLEmodule::oldDeviceConnected = false;
uint8_t BLEmodule::txValue = 0;
unsigned char BLEmodule::LED = 0;
bool BLEmodule::ledState = 0;
char BLEmodule::Str1[20];

BLEServer* BLEmodule::pServer = nullptr;
BLECharacteristic* BLEmodule::pTxCharacteristic = nullptr;

class MyServerCallbacks: public BLEServerCallbacks 
{
	void onConnect(BLEServer* pServer) 
	{
		BLEmodule::deviceConnected = true;
	};

	void onDisconnect(BLEServer* pServer) 
	{
		BLEmodule::deviceConnected = false;
	}
};

class MyCallbacks: public BLECharacteristicCallbacks {
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

void BLEmodule::WriteString2BLE(char *s)
{
	pTxCharacteristic->setValue(s);
	pTxCharacteristic->notify();
}

void BLEmodule::BLEInit()
{
	// Create the BLE Device
	BLEDevice::init("UART Service");

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX,BLECharacteristic::PROPERTY_NOTIFY);
						
	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX,BLECharacteristic::PROPERTY_WRITE);

	pRxCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
	pTxCharacteristic->setValue("Hello World");
}

void BLEmodule::BLETest()
{
    if (deviceConnected) 
    {
        WriteString2BLE("PEPE EL DE LOS PALOTES\n");
        delay(500); // bluetooth stack will go into congestion, if too many packets are sent
        digitalWrite(LED, 1);
        delay(500); // bluetooth stack will go into congestion, if too many packets are sent
        digitalWrite(LED, 0);
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) 
    {
        delay(500); // give the bluetooth stack the chance to get things ready
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