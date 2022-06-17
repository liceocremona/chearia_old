/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio; // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  if (!radio.begin(7, 8)) {
  Serial.println(F("radio hardware not responding!"));
  while (1) {Serial.println("radio hardware not responding!");} // hold program in infinite loop to prevent subsequent errors
}

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  Serial.println(F("radio hardware responding!"));
  const char text[32] = "23-itwork";
  if (radio.write(&text, sizeof(text))){
    Serial.println("ok");
  }
  delay(1000);
}
