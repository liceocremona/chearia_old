// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio; // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
   if (!radio.begin(7, 8)) {
  Serial.println(F("radio hardware not responding!"));
  while (1) {Serial.println("radio hardware not responding!");} // hold program in infinite loop to prevent subsequent errors
}
radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  Serial.println(F("radio hardware responding!"));
  if (radio.available()) {
    char data[32] = "";
    radio.read(&data, sizeof(data));
    Serial.println(data);
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(data);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    delay(1000);
  }
}
