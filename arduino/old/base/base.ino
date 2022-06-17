/**************************************************************************
  This example shows how to compose images from bitmaps

  We have used this tool to convert the .bmp to cpp
  https://javl.github.io/image2cpp/
  Thanks!
  
 **************************************************************************/
//#include "Images.h"   
#include <Arduino_MKRIoTCarrier.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <ArduinoHttpClient.h>
#include "secret.h"

MKRIoTCarrier carrier;

#define NUMPIXELS 5
String attivo = "attivato";


int allarme;
int stato;

char time_server[] = "2vhf9o.deta.dev";
String today_date = "01-01-1975";
int start_hour = 0;
int start_min = 0;
int one_state = 0;

float temperature;
float humidity;

WiFiSSLClient wifi_client;
HttpClient client = HttpClient(wifi_client, time_server, 443);
FirebaseData firebaseData;

String path = "/db1";

const int AirValue = 620; //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int mapsoilmoisturevalue;
int pir = A5;

unsigned long initial_time = 0;

void setup() {
  Serial.begin(9600);


Serial.println("inizio connessione alla rete");
 int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nconnessione riuscita!");
  
  //richiesta della data e del tempo al server
  Serial.println("invio richiesta http a /time");
  client.get("/time");
  Serial.println("lettura risposta");
  int response_code4 = client.responseStatusCode();
  if (response_code4 == 200) {
  String time_timestamp = client.responseBody();
  char timestamp_ch[6];
  char *numb = NULL;
  time_timestamp.toCharArray(timestamp_ch, 6);
  numb = strtok(timestamp_ch, "-");
  while(numb != NULL)
    {   
      if (one_state ==0) {
         String h_s = String(numb);
        start_hour = h_s.toInt();
        one_state ++;
      }
      else {      
        String minu_s = String(numb);
        start_min = minu_s.toInt();
      }
       numb = strtok(NULL, "-"); 
    };
  }
  Serial.println(start_hour);
  Serial.println(start_min);
  
  Serial.println("invio richiesta http a /date");
  client.get("/date");
  Serial.println("lettura risposta");
  int response_code3 = client.responseStatusCode();
  if (response_code3 == 200) {
  today_date = client.responseBody(); }
  Serial.println(today_date);
  
  Serial.println("inizializzazzione libreria firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  addData("itwork", 50.23);
Serial.println("inizio calibrazione pir");
pinMode(pir, INPUT_PULLUP);
delay(30000);
Serial.println("fine calibrazione pir");

 CARRIER_CASE = false;
  carrier.begin();
  Serial.println("passo al prossimo");
carrier.display.fillScreen(ST77XX_CYAN);
 testdrawtext ("Che ", ST77XX_BLACK, 7, 60, 80);
  testdrawtext ("Aria", ST77XX_BLACK, 7, 40, 135);
   carrier.display.drawCircle(180, 50, 30, ST77XX_RED);
    carrier.display.fillCircle(180, 50, 30, ST77XX_RED);
     carrier.display.fillCircle(195, 40, 5, ST77XX_WHITE);
      carrier.display.fillRect(179, 80, 2, 240, ST77XX_BLACK);
       carrier.display.fillRect(0, 200, 240, 40, ST77XX_GREEN);
        carrier.display.fillRect(170, 192, 18, 8, ST77XX_RED);
         carrier.display.fillRect(175, 87, 10, 5, ST77XX_RED);
          carrier.display.fillCircle(0, 0, 55, ST77XX_YELLOW);
  luci();    
 
      delay(10000);
      spegni();
       luci2 ();
     carrier.display.fillScreen(ST77XX_WHITE);
      testdrawtext ("IIS ", ST77XX_ORANGE, 7, 50, 80);
       delay(750);
        testdrawtext ("Cremona ", ST77XX_BLACK, 5, 20, 140);
       
      delay(10000);
spegni();

menu();


      
}



void loop() {

  if (initial_time ==0) 
{
  initial_time = millis();
  Sensor();
}
else {
  unsigned long curr_time = millis();
  unsigned long pass_time = curr_time - initial_time;
  if (pass_time > 61000) {
    Sensor();
    initial_time = millis();
  }
}

carrier.Buttons.update();

checkallarm();

Balloon();
temperature = carrier.Env.readTemperature();
humidity = carrier.Env.readHumidity();
  //displayEnv();
 if (carrier.Buttons.getTouch(TOUCH1)) {
    Serial.println("Touching Button 1");
    carrier.display.fillScreen(ST77XX_BLACK);
    testdrawtext ("Temperatura:", ST77XX_WHITE, 3, 10, 20);
    testdrawnum (temperature, ST77XX_WHITE, 15, 32, 100);
    testdrawtext ("o", ST77XX_WHITE, 5, 200, 65);
  }

if (carrier.Buttons.getTouch(TOUCH2)) {
    Serial.println("Touching Button 2");
    carrier.display.fillScreen(ST77XX_BLACK);
    testdrawtext ("Umidita':", ST77XX_WHITE, 4, 10, 20);
    testdrawnum (humidity, ST77XX_WHITE, 15, 32, 100);
    testdrawtext ("%", ST77XX_WHITE, 5, 200, 65);
  }



  
  delay(1000);
}


void luci () {
  
carrier.leds.setPixelColor(0, 20, 0, 20);       // G R B
carrier.leds.setPixelColor(1, 20, 20, 0);
carrier.leds.setPixelColor(2, 20, 0, 20);
carrier.leds.setPixelColor(3, 20, 20, 0);
carrier.leds.setPixelColor(4, 20, 0, 20);
  carrier.leds.show(); 
}

void luci2 () {
  
carrier.leds.setPixelColor(0, 12, 20, 0);       
carrier.leds.setPixelColor(1, 20, 20, 20);
carrier.leds.setPixelColor(2, 12, 20, 0);
carrier.leds.setPixelColor(3, 20, 20, 20);
carrier.leds.setPixelColor(4, 12, 20, 0);
  carrier.leds.show(); 
}

void luci3 () {
  
carrier.leds.setPixelColor(0, 20, 0, 0);       // G R B
carrier.leds.setPixelColor(1, 20, 0, 0);
carrier.leds.setPixelColor(2, 20, 0, 0);
carrier.leds.setPixelColor(3, 20, 0, 0);
carrier.leds.setPixelColor(4, 20, 0, 0);
  carrier.leds.show(); 
}

void spegni () {
carrier.leds.setPixelColor(0, 0);       
carrier.leds.setPixelColor(1, 0);
carrier.leds.setPixelColor(2, 0);
carrier.leds.setPixelColor(3, 0);
carrier.leds.setPixelColor(4, 0);
  carrier.leds.show();
}



void testdrawtext(char *text, uint16_t color, int size, int posizionex, int posizioney) {
  carrier.display.setCursor(posizionex, posizioney);
  carrier.display.setTextColor(color);
  carrier.display.setTextWrap(true);
  carrier.display.setTextSize(size);
  carrier.display.print(text);
}

void testdrawnum (int num, uint16_t color, int size, int posizionex, int posizioney) {
  carrier.display.setCursor(posizionex, posizioney);
  carrier.display.setTextColor(color);
  carrier.display.setTextWrap(true);
  carrier.display.setTextSize(size);
  carrier.display.print(num);
}



void menu () {

carrier.display.fillScreen(ST77XX_BLACK);
testdrawtext ("MENU", ST77XX_BLUE, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_BLUE);
testdrawtext ("-temperatura P1", ST77XX_WHITE, 2.5, 8, 130);
testdrawtext ("-umidita P2", ST77XX_WHITE, 3, 15, 190);

delay(750);
testdrawtext ("MENU", ST77XX_RED, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_RED);
delay(750);
testdrawtext ("MENU", ST77XX_YELLOW, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_YELLOW);
delay(750);
testdrawtext ("MENU", ST77XX_ORANGE, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_ORANGE);
delay(750);
testdrawtext ("MENU", ST77XX_CYAN, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_CYAN);
delay(750);
testdrawtext ("MENU", ST77XX_MAGENTA, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_MAGENTA);
delay(750);
testdrawtext ("MENU", ST77XX_GREEN, 8, 30, 30);
carrier.display.drawRect(15, 15, 210, 85, ST77XX_GREEN);


} 
void allarmee () {
carrier.display.fillScreen(ST77XX_RED);
  testdrawtext ("ALLARME", ST77XX_BLACK, 5, 15, 180);
   carrier.display.fillTriangle(120, 30, 30, 160, 210, 160, ST77XX_BLACK);
    carrier.display.fillRoundRect(110, 58, 20, 70, 60, ST77XX_RED);
     carrier.display.fillCircle(119, 145 , 10, ST77XX_RED);
}

void Sensor() {
  float soilmoisture;
  soilmoisture = getHumid();
  addData("soilhumidity", soilmoisture);
   temperature = carrier.Env.readTemperature();
  addData("base-temperature", temperature);
  humidity = carrier.Env.readHumidity();
  addData("base-humidity", humidity);

Serial.print(soilmoisture);
Serial.println("  %");
}

void checkallarm() {
 
  if (carrier.Light.proximityAvailable()) { 
 
     allarme = carrier.Light.readProximity();
     Serial.println(allarme);}

      int PIR= digitalRead(pir);
      Serial.println(PIR);

if(allarme <= 190 || PIR == 1){
  

  if(Firebase.getInt(firebaseData, "/db4/allarm/status")){
    
  stato = firebaseData.intData();
   Serial.println(stato);
   if(stato==1){
    
    carrier.Buzzer.sound(1000);
    luci3();
    delay(250);
    spegni();
    carrier.Buzzer.noSound();
    if (Firebase.setString(firebaseData, "/db4/allarm/action", attivo));
   }
}

}
}

void Balloon() {
  if (Firebase.getString(firebaseData, "/db4/pallone/azione")) {
    String azione = firebaseData.stringData();
    if (Firebase.getString(firebaseData, "/db4/pallone/stato")) {
      String stato = firebaseData.stringData();
      if (azione == "aria" && stato == "terra") {
        if (Firebase.setString(firebaseData, "/db4/pallone/stato", "salendo")){
          Serial.println("sto facendo salire il pallone");
          for (int n = 0; n< 15; n++) {
            Serial.print(".");
          delay(1000);
                  }
           Serial.println("finito");
           Firebase.setString(firebaseData, "/db4/pallone/stato", "aria");
        }
        
    } else if (azione == "terra" && stato == "aria") {
      if (Firebase.setString(firebaseData, "/db4/pallone/stato", "scendendo")){
          Serial.println("sto facendo scendere il pallone");
          for (int n = 0; n< 15; n++) {
            Serial.print(".");
          delay(1000);
                  }
           Serial.println("finito");
           Firebase.setString(firebaseData, "/db4/pallone/stato", "terra");
        }
    }
    }
  }
}

float getHumid() {
  soilMoistureValue = analogRead(A6);  //put Sensor insert into soil
mapsoilmoisturevalue = map(soilMoistureValue, 1023, 0, 0, 1023);  //Ilsensore restituisce da 1023 a 0 quindi rimappo da 0 a 1023
Serial.println(mapsoilmoisturevalue);
soilmoisturepercent= map(mapsoilmoisturevalue, 0, 247, 0, 100);
return soilmoisturepercent;

}
String getCurrentTime() {//questa funzione ritorna l'ora e i minuti nel formato stringa
  String timestamp = "00-00";
  float mill = millis();
  int now = round(millis());
  int now_sec = now /1000;
  int now_min = now_sec / 60;
  int now_hour = now_min / 60;
  int now_real_min = now_min - (60 *now_hour);
  int real_time_h = start_hour + now_hour;
  int real_time_m = start_min + now_real_min;
  if (real_time_m > 59) {
    real_time_h += 1;
    real_time_m = real_time_m - 60;
  };
  if ((real_time_m < 10) && (real_time_h > 10)) {
    timestamp = String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m < 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m > 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + String(real_time_m);
  } else if ((real_time_m > 10) && (real_time_h > 10)){
    timestamp = String(real_time_h) + "-" + String(real_time_m);
  };
  return timestamp;
}
String getCurrentDate() {//questa funzione ritorna la data nel formato stringa
  return today_date;
}

bool addData(String dataName, float Data) // questa funzione aggiunge i dati al db
{
  Serial.println("inizio lettura data");
  String curr_date = getCurrentDate();
  if (curr_date) {
    Serial.println("data ricevuta, inizio lettura tempo");
      String curr_time = getCurrentTime();
      if (curr_time) {
        Serial.println("tempo ricevuto, imposto il dato nel db");
        if (Firebase.setFloat(firebaseData, path+"/" + dataName + "/data/" + curr_date + "/" + curr_time, Data)) {
           if (firebaseData.floatData()) {
            Serial.print("azione riuscita, il dato è: ");
            Serial.println(firebaseData.floatData());
            return true;
           }
           else {
            Serial.println("errore nel inserimento del dato, data error");
            return false;
           }
        } 
        else {
          Serial.println("errore nel inserimento del dato, net error");
          return false;
        }
        
      }
      else {
        Serial.println("errore nel leggere l'ora");
        return false;
      }
  }
  else {
    Serial.println("errore nel leggere la data");
    return false;
  }
}
