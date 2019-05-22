/*
* Author - Marcel Schulz (alias Schullebernd)
* *************************************************************
* See further library details on https://github.com/Schullebernd/SBNetwork
* *************************************************************
* This Getting started is prepared for using it with a Wemos D1 mini as a master device.
* If you want to use an Arduino, then change the line 36 "SBNetwork networkDevice(false, D2, D8);" to the correct pinout for an Arduino "SBNetwork networkDevice(false, 6, 7)".
* **************************************************************
* Step 1 - Prepare your device
* Connect a nRF24L01 transmitter to a Wemos D1 mini or an Arduino Device
* WEMOS > RF24      ARDUINO > RF24      --------------------------------------
* ------------      --------------     |   GND #  # VCC          TOP VIEW     |
*   3V3 > VCC          VCC  > VCC      |    CE #  # CSN          of nRF24L01  |
*   GND > GND          GND  > GND      |   SCK #  # MOSI                      |
*    D2 > CE             6  > CE       |  MISO #  # IRQ                       |
*    D8 > CSN            7  > CSN      |                                      |
*    D7 > MOSI          11  > MOSI      --------------------------------------
*    D6 > MISO          12  > MISO
*    D5 > SCK           13  > SCK
*
* Step 2 - Build the sketch for the master device
* Connect the Wemos via USB to the PC, select the right board and COM interface in the tools menu and run the project.
* After building and loading up to the Wemos, the serial monitor should show some log data.
*/

#include <SBNetwork_config.h>
#include <SBNetwork.h>

// WIFI
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> //http-communication zu server

#include <ArduinoJson.h>


const char* ssid = "VIRUS.EXE";
const char* password = "Martinrouterking58";

const char* hostData ="http://192.168.0.143:5000/api/sendScore";

// Type in here the mac address of the device.
// This must be unique within your complete network otherwise the network will not work.
SBMacAddress deviceMac(0x01, 0x02, 0x03, 0x04, 0x05);
//SBMacAddress deviceMac(0x05, 0x04, 0x03, 0x02, 0x01);

// Create a new network device with Wemos D1 mini and set the _ce and _cs pin.
// The first argument defines the type of the device. false=master and true=client device
SBNetwork networkDevice(false, D2, D8);
//SBNetwork networkDevice(true, 6, 7);

void setup() {
	// Init serial connection
	Serial.begin(19200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

	// Initialize the network device
	networkDevice.initialize(deviceMac);

	Serial.println(F("*** PRESS 'N' to reset the device"));
}

void loop() {

	// This routine is for resetting the device
	// All flash data will be deleted
	if (Serial.available())
	{
		char c = toupper(Serial.read());
		if (c == 'N') {
			networkDevice.resetData();
		}
	}

	// Call this in the loop() function to maintain the network device
	networkDevice.update();

	// Check, if there are messages available
	uint8_t messageSize = networkDevice.available();
	if (messageSize > 0) {
		byte* message = (byte*)networkDevice.getMessage();
		Serial.print(F("Received Content from sensor: "));
		Serial.println((char*)message);
		if (strcmp((char*)message, "sensor") == 0) {
			// We have received a BME280 transmission
			float field, score_a, score_b;
			memcpy(&field, (void*)(message + 10), sizeof(float));
			memcpy(&score_a, (void*)(message + 10 + 4), sizeof(float));
			memcpy(&score_b, (void*)(message + 10 + 4 + 4), sizeof(float));
			Serial.print("Field: ");
			Serial.println(field,0);
	
			Serial.print("Score_a: ");
			Serial.println(score_a,0);

			Serial.print("Score_b: ");
			Serial.println(score_b,0);

      sendDataToServer(field,score_a, score_b);
		}
	}
} // Loop

void sendDataToServer(float field, float score_a, int score_b){
   if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
    JSONencoder["field"] = field;
    JSONencoder["score_a"] = score_a;
    JSONencoder["score_b"] = score_b;

    

   
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
 
    HTTPClient http;    //Declare object of class HTTPClient
 
    http.begin(hostData);      //Specify request 
    http.addHeader("X-POOLAUTH","42");
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
 
    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();             //Get the response payload
 
    Serial.println(httpCode);   //Print HTTP return code
    //Serial.println(payload);    //Print request response payload
 
    http.end();  //Close connection
                 
                
  } else {
 
    Serial.println("Error in WiFi connection");
    
  }
}
