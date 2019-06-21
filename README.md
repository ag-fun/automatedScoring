# automatedScoring
code for the automated scoring system

needs 
https://github.com/Schullebernd/SBNetwork

download the zip and add it to the libary folder

also needs 
RF24 lib



## Master

Controller : Wemos D1 mini 

```c++
* Step 1 - Prepare your device
* Connect a nRF24L01 transmitter to a Wemos D1 mini 
* WEMOS > RF24      
* ------------      
*   3V3 > VCC         
*   GND > GND         
*    D2 > CE           
*    D8 > CSN            
*    D7 > MOSI       
*    D6 > MISO         
*    D5 > SCK  
*    D8 -> with widerstand zu GND      
```

## Client
```
 Step 1 - Prepare your device
* Connect a nRF24L01 transmitter to a Wemos D1 mini or an Arduino Device
* ARDUINO > RF24     
* ------------      
* VCC  > VCC     
* GND  > GND   
* 6  > CE      
* 7  > CSN      
* 11  > MOSI    
* 12  > MISO
* 13  > SCK
```

Ultrasonic sensors 
```
* 5V > VCC 
* GND > GND
* 2 > US1_echoPin     
* 3 > US1_trigPin
* 4 > US2_echoPin     
* 5 > US2_trigPin

```

