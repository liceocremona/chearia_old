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
int ms_from_midnight_upd = 0;
int start_millis = 0;

//Define Firebase data object
FirebaseData firebaseData;//Inizializzanione classe delle funzioni per la libreria FIrebase
WiFiSSLClient wifi_client;//Inizializzazione classe del client WiFi per fare richieste internet
HttpClient client = HttpClient(wifi_client, time_server, 443);//Inizializzazione client HTTP 9quello che esegue la richiesta ai link)
int status;

String path = "/db1";//La sottocartella dove salvare i dati di firebase 
//https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db1  <--
//Aggiorna l'orario facendo una richiesta http alle API


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

void updateTime() {
  Serial.println("invio richiesta http a /timems");
  //richiesta a endpoint API
  client.get("/timems/-2");// /timems/{tz} tz = timezone in formato ETC/GMT https://www.ibm.com/docs/en/cloudpakw3700/2.3.0.0?topic=SS6PD2_2.3.0/doc/psapsys_restapi/time_zone_list.html
  Serial.println("lettura risposta");
  int response_code4 = client.responseStatusCode();
  if (response_code4 == 200) {
  String timestamp_ms = client.responseBody();//Testo risposta
  ms_from_midnight_upd = timestamp_ms.toInt();}
  start_millis = round(millis());//Impostazione del millis() al momento dell'aggiornamento
  Serial.println(ms_from_midnight_upd);
}

void updateDate() {
  Serial.println("invio richiesta http a /date");
  //Richiesta a Endpoint API
  client.get("/date/-2");
  Serial.println("lettura risposta");
  int response_code3 = client.responseStatusCode();
  if (response_code3 == 200) {
  today_date = client.responseBody(); }
  Serial.println(today_date);
}


String getCurrentTime() {
  //questa funzione ritorna l'ora e i minuti nel formato stringa
  String timestamp = "00-00"; //La timestamp è il formato con il quale indichiamo la data su Firebase

  int day_millis = ms_from_midnight_upd + (round(millis()) - start_millis); 
  // calcolo ms da mezzanotte sommando l'orario a cui si è aggiornata la data e il tempo di attività della scheda dall'ultimo aggiornamento

  Serial.println("ms da mezzanotte");
  Serial.println(day_millis);
  if (day_millis>=86400000) {
    Serial.println("E' passata mezzanotte, aggiorno la data e l'ora");
    updateTime();
    updateDate();
    day_millis = ms_from_midnight_upd + (round(millis()) - start_millis);
  }
  int now_hour = day_millis / 3600000;//calcolo ora
  int now_min = (day_millis / 60000) % 60;//calcolo minuti
  
  Serial.println(now_hour);
  Serial.println(now_min);
  
  //Creazione della timestamp
  //Le ore e i minuti devono sempre essere indicati con due cifre, quando il numero è minore 
  //di 10 bisogna agiungere uno zero prima. Quest'operazione è necessaria per permettere a Firebase 
  //di ordinare i dati nel modo corretto nel DB
  //Esempio: 
  //02-08
  //02-24
  //11-04
  //16-47
  if (now_hour<10 && now_min<10) {//0n-0n
    timestamp = String("0") + String(now_hour) + String("-0") + String(now_min);
  } else if (now_hour<10 && now_min>=10) {//0n-nn
    timestamp = String("0") + String(now_hour) + String("-") + String(now_min);
  } else if (now_hour>=10 && now_min<10) {//nn-0n
    timestamp = String(now_hour) + String("-0") + String(now_min);
  } else if (now_hour>=10 && now_min>=10) {//nn-nn
    timestamp = String(now_hour) + String("-") + String(now_min);
  } else {
    timestamp = String(now_hour) + String("-") + String(now_min);
  }
  return timestamp;
}

String getCurrentDate() {
  //questa funzione ritorna la data nel formato stringa
  return today_date;
}




bool addData(String dataName, float Data) { 
// questa funzione aggiunge i dati al db
  Serial.println("inizio lettura data");
  String curr_time = getCurrentTime(); 
  Serial.println(curr_time);
  
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
