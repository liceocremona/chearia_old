//per usare il codice installare i certificati ssl da i seguenti domini:
//2vhf9o.deta.dev:443
//olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app:443
//impostare timezone qua --> client.get("/timems/{tz}");

#include <Wire.h>
#include <avr/dtostrf.h>
#include <ArduinoHttpClient.h>
#include "secret.h"
#include <ArduinoJson.h>

#define ARDUINOJSON_USE_DOUBLE 0
char api_server[] = "api.progettochearia.it";
String key = KEY;
//Define Firebase data object
WiFiSSLClient wifi_client;//Inizializzazione classe del client WiFi per fare richieste internet
HttpClient client = HttpClient(wifi_client, time_server, 443);//Inizializzazione client HTTP 9quello che esegue la richiesta ai link)
int status;




void setup() {
  Serial.begin(9600);
  Serial.println("inizio connessione alla rete");
  status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //Connessione alla rete WiFi
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nconnessione riuscita!");

  //richiesta della data e del tempo al server


  addData("itwork", 69.66);//Aggiunge un valore al DB per indicare che funziona
  
  Wire.begin(9);//Inizializzazione connessione I2C con il microcontrollore a 3.3V
  Wire.onReceive(receiveEvent);
  //Si imposta un CallBack quando vengono ricevuti i dati:
  //Quando arriva questa funzione viene richiamata e le viene passato come parametro il valore ricevuto
}

void loop() {
  while (status != WL_CONNECTED)//Nel caso non sia connesso prova a riconnettersi
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(200);
  }
  delay(50);
}




void receiveEvent(int bytes) {
//Funzione richiamata al ricevimento dei dati 
  //Funzionamento connessione I2C con libreria Wire:
  //La scheda a 5V (Es. Mega) invia i dati un byte alla volta con questo formato {valore}-{nomedato}
  //nomedato equivale alla chiave del DB Firebase
  //Es. Temperatura: 22.8-temperature 
  String text_num;
  while(Wire.available() > 0) // finchè non sono letti tutti i byte
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
// questa funzione aggiunge i dati al db
  String contentType = "";
  String body = "id="+dataName + "&key=" + KEY;
  client.put("/board/putdata", contentType, body);
  if (curr_time) {//se l'orario è valido
    Serial.println("tempo ricevuta, inizio lettura data");
    String curr_date = getCurrentDate();
    Serial.println(curr_date);
      if (curr_date) { // se l'orario è valido
        Serial.println("data ricevuta, imposto il dato nel db");
        Serial.println(path+"/" + dataName + "/data/" + curr_date + "/" + curr_time);
        //imposta i dati sul DB Firebase 
        if (Firebase.setFloat(firebaseData, path+"/" + dataName + "/data/" + curr_date + "/" + curr_time, Data)) { 
           if (firebaseData.floatData()) {
            Serial.print("azione riuscita, il dato è: ");
            Serial.println(firebaseData.floatData());
            return true;
           }
           else {
            Serial.println("errore nel inserimento del dato, data error");
            Serial.println(firebaseData.errorReason());
            return false;
           }
        } 
        else {
          Serial.println("errore nel inserimento del dato, net error");
          Serial.println(firebaseData.errorReason());
          return false;
        }
        
      }
      else {
        Serial.println("errore nel leggere la data");
        return false;
      }
  }
  else {
    Serial.println("errore nel leggere l'orario");
    return false;
  }
}
