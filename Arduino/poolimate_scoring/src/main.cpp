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

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS))
  {
    Serial.println("Cannot communicate with radio");
    while (1)
      ; // Wait here forever.
  }
  Serial.println("Start Client");
  _radioData.FieldNumber = RADIO_ID;
}

void loop()
{
  _radioData.OnTimeMillis = millis();
  _radioData.ScoreA = random(0, 15);
  _radioData.ScoreB =random(0, 15);

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
    delay(10000);
  }
  else
  {
    Serial.println("...Failed");
    _radioData.FailedTxCount++;
  }

  delay(1000);
}
