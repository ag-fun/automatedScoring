/*
* Author - Marcel Schulz (alias Schullebernd)
* *************************************************************
* See further library details on https://github.com/Schullebernd/SBNetwork
* *************************************************************
* This example is a client with a BME280 temperatur, humidity and barometer sensor on I2C.
* It sends the measured values every 20 seconds to the master.
* To setup the master device for this example, open the UniversalSensorMaster example and run it on a second device.
* **************************************************************
* Step 1 - Prepare your device like in the example sketch GettingStartedClient
*
* Step 2 - Connect the BME280 sensor to your arduino/wemos device
* Connect		Arduino --> BME280
*           ------------------ 
*                A4 --> SDA
*                A5 --> SCL
*               3V3 --> VCC
*               GND --> GND
*             empty --> CSB
*             empty --> SD0
* The I2C Address of BME280 usually is 0x76 --> Put this into  bme.begin(0x76) at setup function. If this is not working try 0x77.
*
* Step 3 - Download and include the Sparkfun BME280 library.
* https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
* 
* Step 4 - Run the project
* Connect the Arduino via USB to the PC, select the right board and COM interface in the tools menu and run the project.
*/

#include <SBNetwork_config.h>
#include <SBNetwork.h>


// Type in here the mac address of the device.
// This must be unique within your complete network otherwise the network will not work.
//SBMacAddress deviceMac(0x01, 0x02, 0x03, 0x04, 0x05);
SBMacAddress deviceMac(0x05, 0x04, 0x03, 0x02, 0x01);

// Create a new network device with Arduino and set the _ce and _cs pin
// The first argument defines the type of the device. false=master and true=client device
//SBNetwork networkDevice(false, D2, D8);
SBNetwork networkDevice(true, 6, 7);

// time variables
uint32_t wait = 2000;
uint32_t lastWait = wait;


void setup() {

	Serial.begin(19200);
	Serial.println(F("*** PRESS 'N' to reset the device"));

	// Initialize the network device
	networkDevice.initialize(deviceMac);


}

void loop() {

	if (Serial.available())
	{
		char c = toupper(Serial.read());
		if (c == 'N') {
			networkDevice.resetData();
		}
	}

	// Call this in the loop() function to maintain the network device
	networkDevice.update();

	// Send a message to the master every 4 seconds
	if (lastWait < millis()) {
		float field = 1;
		Serial.print("Field: ");
		Serial.println(field);

		float score_a = 1;
		Serial.print("score 1: ");
		Serial.println(score_a);

		float score_b = 10;
		Serial.print("score_b: ");
		Serial.println(score_b);


   
		byte message[10 + (3*4)]; // the first 10 bytes containing the type of the sensor. The 3*4 bytes are needed for the values. float has a length of 32 bits = 4 bytes and we need 3 of them.
		strcpy((char*)message, "sensor");
		memcpy((void*)(message + 10), &field, sizeof(float));
		memcpy((void*)(message + 10 + 4), &score_a, sizeof(float));
		memcpy((void*)(message + 10 + 4 + 4), &score_b, sizeof(float));
		networkDevice.sendToDevice(networkDevice.NetworkDevice.MasterMAC, message, 10 + (3*4));
		lastWait += wait;
	}

} // Loop
