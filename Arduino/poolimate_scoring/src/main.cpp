/*
Demonstrates simple RX and TX operation.
Any of the Basic_RX examples can be used as a receiver.
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

// US Sensors 
#define US1_echoPin     2       // Ultra Sonic Echo Pin, Sensor 1
#define US1_trigPin     3       // Ultra Sonic Trigger Pin, Sensor 1
#define US2_echoPin     4       // Ultra Sonic Echo Pin, Sensor 2       
#define US2_trigPin     5       // Ultra Sonic Trigger Pin, Sensor 2    

static long ScoreDist[17] = {0, 3, 8, 15, 24, 32, 39, 47, 54, 62, 70, 78, 84, 92, 97,105,111}; // in cm; 105.5=1,111=0
                              // 15 14 13  12  11  10   9  8   7   6   5   4   3   2   1 0 


const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
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
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS,3))
  {
    Serial.println("Cannot communicate with radio");
    while (1)
      ; // Wait here forever.
  }

    // =================== Init Sensors / Fan ====================
  // Ultra Sonic Range
  // Sensor 1
  pinMode(US1_trigPin, OUTPUT);
  pinMode(US1_echoPin, INPUT);
  // Sensor 2
  pinMode(US2_trigPin, OUTPUT);
  pinMode(US2_echoPin, INPUT);

  Serial.println("Start Client");
  _radioData.FieldNumber = RADIO_ID;
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
  Serial.println(distance);
  return dist2Score(distance);
}





void loop()
{
  _radioData.OnTimeMillis = millis();
  // _radioData.ScoreA = random(0, 15);
  // _radioData.ScoreB =random(0, 15);
  _radioData.ScoreA =  ReadScore(1);
  _radioData.ScoreB =ReadScore(2);


  Serial.print("Sending ");
  Serial.print(_radioData.OnTimeMillis);
  Serial.print(" ms");

  // By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
  // it will try again up to 16 times.  This retry logic is built into the radio hardware itself, so it is very fast.
  // You can also perform a NO_ACK send that does not request an acknowledgement.  In this situation, the data packet
  // will only be transmitted a single time and there is no verification of delivery.  So NO_ACK sends are suited for
  // situations where performance is more important than reliability.
  //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
  //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)

  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
  {
    Serial.println("...Success");
    delay(1000);
  }
  else
  {
    Serial.println("...Failed");
    _radioData.FailedTxCount++;
  }

  delay(1000);
}
