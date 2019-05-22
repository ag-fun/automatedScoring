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

#define US1_echoPin     7       // Ultra Sonic Echo Pin, Sensor 1
#define US1_trigPin     8       // Ultra Sonic Trigger Pin, Sensor 1
#define US2_echoPin     9       // Ultra Sonic Echo Pin, Sensor 2       // Change!
#define US2_trigPin     10       // Ultra Sonic Trigger Pin, Sensor 2    // Change!

static long ScoreDist[15] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140}; // in cm;


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

  // =================== Init Sensors / Fan ====================
  // Ultra Sonic Range
  // Sensor 1
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  // Sensor 2
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);
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

		float score_a = ReadScore(1);
		Serial.print("score 1: ");
		Serial.println(score_a);

		float score_b = ReadScore(2);
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

float ReadScore(uint8_t team) {
  //________________________________________________________________________________________
  // 1) Read the distance of the required team
  // 2) Call dist2Score to get the score
  //________________________________________________________________________________________
  long distance;

  if (team & 1) {
    distance = readUS(US1_trigPin, US1_echoPin);
  }

  else {
    distance = readUS(US2_trigPin, US2_echoPin);
  }

  return dist2Score(distance);
}


long readUS(int Trig, int Echo) {
  //________________________________________________________________________________________
  // 1) Read the Ultra Sonic Sensor and calculate the distance in cm
  //________________________________________________________________________________________
  int trigPin = Trig;
  int echoPin = Echo;
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distance = (duration / 2) / 29.1;

  return distance;
}

float dist2Score(long distance) {
  //________________________________________________________________________________________
  // 1) Check what distances in ScoreDist are shorter than the measurement
  // 2.1) Return 15-i since the measurement is from top down
  // 2.2) Return 0 because the measurement is longer than the entries
  //________________________________________________________________________________________
  uint16_t i = 0;
  float score = 0;
  while (ScoreDist[i] < distance) {
    if (ScoreDist[i + 1] >= distance) {
      // ScoreDist[i] is smaller equal & ScoreDist[i+1] is greater -> index resemples score
      score = (15 - (float)i);
    }
    i++;
    if (i > 14) {
      break;
    }
  }
  
  if (score < 0) {
    score = 0;
  }
  return score;
}
