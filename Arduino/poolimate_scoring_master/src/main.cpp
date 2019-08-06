
/*

Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
Please read through 'NRFLite.h' for a description of all the methods available in the library.

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include <Arduino.h>
#include <SPI.h>
#include <NRFLite.h>


//---OLED---//
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
//define on which pins is sda and scl
const uint8_t O_dataPin = 4;//12;
const uint8_t O_sclkPin = 3;//13;

boolean withDisplay =true;



const static uint8_t RADIO_ID = 0; // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 6;
const static uint8_t PIN_RADIO_CSN = 7;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
  uint8_t FieldNumber;
  uint32_t OnTimeMillis;
  uint32_t ScoreA;
  uint32_t ScoreB;
  uint32_t FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;

void setup()
{
  Serial.begin(115200);

  // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
  // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
  // You can run the 'ChannelScanner' example to help select the best channel for your environment.
  // You can assign a different bitrate and channel as shown below.
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS,3))
  {
    Serial.println("Cannot communicate with radio");
    while (1)
      ; // Wait here forever.
  }

  if(withDisplay){
    Serial.print("Display Start");
   //---DISPLAY INIT-----//
  //Wire.begin(O_dataPin,O_sclkPin);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
   
  delay(10);
}
  // Serial.println("Start Master");
}
void draw(float field, int score_a, int score_b,int time){

  
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Field:  ");display.println(field);
  display.setTextSize(1);
  display.setCursor(0,8);
  display.print("Score 1:  ");display.println(score_a);
  display.print("Score 2:  ");display.println(score_b);
  display.print("timestamp:  ");display.println(time);
 // display.print("Hum:   ");display.print(humidity); display.println("  % r.F.");
 // display.print("Dewp:  ");display.print(dewpoint); display.println("  C");
  
  display.display();
 
 display.clearDisplay();
}

void loop()
{
  // Serial.print("in loop");
  while (_radio.hasData())
  {
    _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

    String msg = "Field ";
    msg += _radioData.FieldNumber;
    msg += ", ";
    msg += _radioData.OnTimeMillis;
    msg += " ms, ";
    msg += " A:";
    msg += _radioData.ScoreA;
    msg += ",";
    msg += " B:";
    msg += _radioData.ScoreB;
    msg += ",";
    msg += _radioData.FailedTxCount;
    msg += " Failed TX";

    Serial.println(msg);
  }

   if(withDisplay){     
      draw(_radioData.FieldNumber ,_radioData.ScoreA, _radioData.ScoreB, _radioData.OnTimeMillis);
      }

}