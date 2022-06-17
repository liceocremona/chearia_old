//per usare il codice installare i certificati ssl da i seguenti domini:
//2vhf9o.deta.dev:443
//olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app:443
//impostare timezone qua --> client.get("/timems/{tz}");

#include <Wire.h>
#include <avr/dtostrf.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <ArduinoHttpClient.h>
#include "secret.h"


char time_server[] = "2vhf9o.deta.dev"; //api per ottenere l'ora
String today_date = "01-01-1975";
int start_hour = 0;
int start_min = 0;
int one_state = 0;
int start_day_millis = 0;
int32_t start_ms;
int32_t start_millis;

//Define Firebase data object
FirebaseData firebaseData;//Inizializzanione classe delle funzioni per la libreria FIrebase
WiFiSSLClient wifi_client;//Inizializzazione classe del client WiFi per fare richieste internet
HttpClient client = HttpClient(wifi_client, time_server, 443);//Inizializzazione client HTTP 9quello che esegue la richiesta ai link)
int status;

String path = "/db1";//La sottocartella dove salvare i dati di firebase 
//https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db1  <--

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
  updateTime();
  updateDate();
  
  Serial.println("inizializzazzione libreria firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD); //Accesso al DB Firebase con le credenziali
  Firebase.reconnectWiFi(true);
  //Si indica alla libreria Firebase che nel caso ci si disconnetta dalla rete WiFI 
  //Deve riprovare a connettersi

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

void receiveEvent(int bytes) {//Funzione richiamata al ricevimento dei dati 
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

String getCurrentTime() {//questa funzione ritorna l'ora e i minuti nel formato stringa
  String timestamp = "00-00";
  int now ;
  if (start_day_millis = 0) {
    now = round(millis());
  }
  else 
  {
    int now_global = round(millis());
    now = now_global - start_day_millis;
  }


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
  if (real_time_h > 23) {
    real_time_h = 0;
    start_day_millis = round(millis());
    Serial.println("aggiornamento nuovo giorno");
    updateTime();
    updateDate();
  } 
  if ((real_time_m < 10) && (real_time_h >= 10)) {
    timestamp = String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m < 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m >= 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + String(real_time_m);
  } else if ((real_time_m >= 10) && (real_time_h >= 10)){
    timestamp = String(real_time_h) + "-" + String(real_time_m);
  } else {
    timestamp = String(real_time_h) + "-" + String(real_time_m);
  }
  return timestamp;
}

String getCurrentTimeMS() {//questa funzione ritorna l'ora e i minuti nel formato stringa
  String timestamp = "00-00";
  if int 
  if (start_day_millis = 0) {
    now = round(millis());
  }
  else 
  {
    int now_global = round(millis());
    now = now_global - start_day_millis;
  }

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
  if (real_time_h > 23) {
    real_time_h = 0;
    start_day_millis = round(millis());
    Serial.println("aggiornamento nuovo giorno");
    updateTime();
    updateDate();
  } 
  if ((real_time_m < 10) && (real_time_h >= 10)) {
    timestamp = String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m < 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + "0" + String(real_time_m);
  } else if ((real_time_m >= 10) && (real_time_h < 10)) {
    timestamp = "0" + String(real_time_h) + "-" + String(real_time_m);
  } else if ((real_time_m >= 10) && (real_time_h >= 10)){
    timestamp = String(real_time_h) + "-" + String(real_time_m);
  } else {
    timestamp = String(real_time_h) + "-" + String(real_time_m);
  }
  return timestamp;
}

String getCurrentDate() {//questa funzione ritorna la data nel formato stringa
  return today_date;
}

bool addData(String dataName, float Data) // questa funzione aggiunge i dati al db
{
  Serial.println("inizio lettura data");
  String curr_time = getCurrentTime();
  String curr_date = getCurrentDate();
  Serial.println(curr_time);
  Serial.println(curr_date);
  if (curr_date) {//se la data è valida 
    Serial.println("data ricevuta, inizio lettura tempo");
      if (curr_time) { // se l'orario è valido
        Serial.println("tempo ricevuto, imposto il dato nel db");
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
        Serial.println("errore nel leggere l'ora");
        return false;
      }
  }
  else {
    Serial.println("errore nel leggere la data");
    return false;
  }
}


void updateTime() {
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
}


void updateTimeMS() {
  Serial.println("invio richiesta http a /timems");
  client.get("/timems/-2");
  Serial.println("lettura risposta");
  int response_code4 = client.responseStatusCode();
  if (response_code4 == 200) {
  String timestamp_ms = client.responseBody();
  start_ms = timestamp_ms.toInt();
  Serial.println(start_ms);
}



void updateDate() {
  Serial.println("invio richiesta http a /date");
  client.get("/date");
  Serial.println("lettura risposta");
  int response_code3 = client.responseStatusCode();
  if (response_code3 == 200) {
  today_date = client.responseBody(); }
  Serial.println(today_date);
}
