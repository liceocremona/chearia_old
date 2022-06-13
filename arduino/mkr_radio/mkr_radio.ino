





void setup() {
  Serial.begin(9600);
  if (!radio.begin(6, 7)) {
  Serial.println(F("radio hardware not responding!"));
  while (1) {Serial.println("radio hardware not responding!");} // hold program in infinite loop to prevent subsequent errors
}
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  uint8_t pipe;
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    delay(1000);
   // if(text==0){Serial.println("testo vuoto");}
  }
}
