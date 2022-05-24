//per usare il codice installare i certificati ssl da i seguenti domini:
//api.progettochearia.it:443
//impostare timezone qua --> client.get("/timems/{tz}");

#include <Wire.h>
#include <avr/dtostrf.h>
#include <ArduinoHttpClient.h>
#include "secret.h"
#include <ArduinoJson.h>
#include <WiFiNINA.h>

#define ARDUINOJSON_USE_DOUBLE 0
char api_server[] = "api.progettochearia.it";
String key = KEY;
//Define Firebase data object
WiFiSSLClient wifi_client;//Inizializzazione classe del client WiFi per fare richieste internet
int status;

#define LED_1 3
#define LED_2 4



void setup() {
  Wire.begin(9);//Inizializzazione connessione I2C con il microcontrollore a 3.3V
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  Serial.begin(9600);
  Serial.println("inizio connessione alla rete");
  digitalWrite(LED_1, HIGH);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //Connessione alla rete WiFi
    Serial.print(".");
    delay(200);
  }
  digitalWrite(LED_1, LOW);
  Serial.println("\nconnessione riuscita!");
  Serial.println(WL_CONNECTED);
  Serial.println(WL_CONNECT_FAILED);
  Serial.println(WL_CONNECTION_LOST);
  Serial.println(WL_DISCONNECTED);
  //richiesta della data e del tempo al server

  
  addData("itwork", 69.66);//Aggiunge un valore al DB per indicare che funziona


  Wire.onReceive(receiveEvent);
  //Si imposta un CallBack quando vengono ricevuti i dati:
  //Quando arriva questa funzione viene richiamata e le viene passato come parametro il valore ricevuto
}

void loop() {
  Serial.print(WiFi.status());
  delay(100);
}

void receiveEvent(int bytes) {
  //Funzione richiamata al ricevimento dei dati
  //Funzionamento connessione I2C con libreria Wire:
  //La scheda a 5V (Es. Mega) invia i dati un byte alla volta con questo formato {valore}-{nomedato}
  //nomedato equivale alla chiave del DB Firebase
  //Es. Temperatura: 22.8-temperature
  Serial.println("");
  String text_num;
  while (Wire.available() > 0) // finchè non sono letti tutti i byte
  {
    char c = Wire.read();//Legge il byte come valore char
    String char_n = String(c);//Transforma il tipo char in tipo String
    text_num = text_num + char_n;//Aggiunge il carattere alla String finale
  }
  Serial.println(text_num);
  scalettaData(text_num);
}

void scalettaData(String dato) {
  //Qesta funzione ottiene la String dalla funzione precedente e separa il valore dal nome del dato
  //Es. 22.8-temperature --> valore: 22.8 nome dato: temperature
  int char_len = dato.length() + 1;//Lunghezza String
  char coded_data[char_len];//Creazione di un array di char pari alla lunghezza della String
  dato.toCharArray(coded_data, char_len);//Conversione della Strin in un array di char
  char * ch_measurament = strtok(coded_data, "-");//Array di char contenente il valore
  char * data_name_ch = strtok(NULL, "-");//Array di char contenente il nome del dato
  String str_measurament = String(ch_measurament);//COnversione in Float
  float measurament = str_measurament.toFloat();
  String data_name = String(data_name_ch);
  bool db_stat;
  db_stat = addData(data_name, measurament);//Chiamata della funzione per aggiungere il valore al DB Firebase
  if (db_stat == true) {
    //continue
  }
}

bool addData(String dataName, float Data) {
  digitalWrite(LED_2, HIGH);
  Serial.print(WiFi.status());
  while (WiFi.status() != WL_CONNECTED)//Nel caso non sia connesso prova a riconnettersi
  {
    Serial.println(WiFi.status());
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(100);
  }
  Serial.println(dataName);
  Serial.println(Data);
  // questa funzione aggiunge i dati al db
  HttpClient client = HttpClient(wifi_client, api_server, 443);//Inizializzazione client HTTP 9quello che esegue la richiesta ai link)
  StaticJsonDocument<120> doc; //creazione doc json per body  della richiesta http
  String contentType = "application/json"; //formato body http
  String body;
  doc["key"] = key; // aggiunta dati al doc json
  doc["datavalue"] = Data;
  serializeJson(doc, body); //conversione del doc json in String
  String req_path = "/v1/board/putdata/" + dataName;
  Serial.println(body);
  Serial.print("faccio una richiesta a: ");
  Serial.println(req_path);
  client.put(req_path, contentType, body);//richiesta http put
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  digitalWrite(LED_2, LOW);
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Risposta: ");
  Serial.println(response);
  
  while (client.available()) client.flush();
  
  client.stop();//chiusura client http
  if (statusCode == 200) //se la richiesta ha successo ritorna true
  {
    return true;
  }
  else
  {
    return false;
  }
}
