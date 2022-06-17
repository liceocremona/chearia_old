#include <Wire.h>

#include <SFE_BMP180.h>
SFE_BMP180 pressure;
double baseline; // baseline pressure
bool pressure_state;

#define MQ7 A1

#include <DHT.h>
DHT dht(22, DHT11);

#include <MQ131.h>
int time_wait_ozone = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);


  //impostazioni sensore pressione
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin()) {
    Serial.println("BMP180 init success");
    pressure_state = true;
  }
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    pressure_state = false;
     
  }

  // Get the baseline pressure:
  
  baseline = getPressure();
  
  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");  

  //impostazzioni sensore co
  pinMode(MQ7, INPUT);

  //inizializzazzione sensore temperatura
  dht.begin();

  MQ131.begin(23,A2, LOW_CONCENTRATION, 1000000);  

  Serial.println("Calibration in progress...");
  
  MQ131.calibrate();
  
  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(MQ131.getR0());
  Serial.println(" Ohms");
  Serial.print("Time to heat = ");
  time_wait_ozone = MQ131.getTimeToRead();
  Serial.print(time_wait_ozone);
  Serial.println(" s");
}

void loop() {
  //inivio richieste a sensori pressione e passaggio a mkr
  char altitude[20];
  getAltitude(altitude);
  Serial.println(altitude);
  sendData(altitude);
  
  delay(3000);
  
  char CO_data[20];//get co data e manda a mkr
  getCO(CO_data);
  Serial.println(CO_data);
  sendData(CO_data);

  delay(3000);

  char temperture_mis[20];
  getTemperature_sec(temperture_mis);
  Serial.println(temperture_mis);
  sendData(temperture_mis);

  delay(3000);

  char humidity_mis[20];
  getHumidity(humidity_mis);
  Serial.println(humidity_mis);
  sendData(humidity_mis);

  delay(3000);
  
// char ozone_data[20];
 //getOzone(ozone_data);
 //Serial.println(ozone_data);
  //sendData(ozone_data);
 //time_wait_ozone = MQ131.getTimeToRead();

 if (time_wait_ozone > 62000) {
  delay(1000);
 }else {
  delay((63000 - time_wait_ozone));
 }
//  
//  char pm25_data[20];
//  getPM25(pm25_data);
//  Serial.println(pm25_data);
//  sendData(pm25_data);
}


void sendData(char data[]) {//manda i dati
  Wire.beginTransmission(9);
  Wire.write(data);
  Wire.endTransmission();
}


//funzioni che ritornano il valore
void getAltitude(char return_value[]) {
  if (pressure_state) {
    //get ìaltitude
  float altitude = 101.00;
  double a,P;
  
  // Get a new pressure reading:

  P = getPressure();

  // Show the relative altitude difference between
  // the new reading and the baseline reading:
  if (P) {
    a = pressure.altitude(P,baseline);
    a += 139.00;
    altitude = (float) a;
  }
  char altitude_ch[5];
  dtostrf(altitude, 6, 2, altitude_ch);
  char unified_val[20];
  strcpy(unified_val, altitude_ch);
  strcat(unified_val, "-altitude");
  int len = (sizeof(unified_val))-1;
  for (int i = 0; i < len; i ++) {
    return_value[i] = unified_val[i];
  }
  } 
  else {
    char default_result[] = "100.11-altitude";
    int len = (sizeof(default_result)) -1;
    for (int i = 0; i < len; i++) {
      return_value[i] = default_result[i];
    }
  }
}

void getCO(char return_value[]) {
  //get CO
  float CO;
  CO = analogRead(MQ7);
  CO = CO + 120.00;
  char CO_ch[5];
  dtostrf(CO, 6, 2, CO_ch);
  char unified[20];
  strcpy(unified, CO_ch);
  strcat(unified, "-CO");
  int len = (sizeof(unified)) -1;
  for (int i = 0; i< len; i++) {
    return_value[i] = unified[i];
  }
}

void getTemperature_sec(char return_value[]) {
  float temperature = 0.00;
  temperature = dht.readTemperature();
  
  
  //altrimenti
  //int temp = dht.readTemperature();
  //temperature = (float) temp;
  char temperature_ch[4];
  dtostrf(temperature, 5, 2, temperature_ch);
  char unified[20];
  strcpy(unified, temperature_ch);
  strcat(unified, "-temperature");
  int len = (sizeof(unified)) -1;
  for (int i = 0; i< len; i++) {
    return_value[i] = unified[i];
  }  
}

void getHumidity(char return_value[]) {
  float humidity = 0.00;
  humidity = dht.readHumidity();
  
  
  //altrimenti
  //int temp = dht.readTemperature();
  //temperature = (float) temp;
  char humidity_ch[4];
  dtostrf(humidity, 5, 2, humidity_ch);
  char unified[20];
  strcpy(unified, humidity_ch);
  strcat(unified, "-humidity");
  int len = (sizeof(unified)) -1;
  for (int i = 0; i< len; i++) {
    return_value[i] = unified[i];
  }  
}

void getOzone(char return_value[]) {
  float ozone = 0.00;
  MQ131.sample();
  ozone = MQ131.getO3(UG_M3);
  //inserire operazioni sesnsore
  char ozone_ch[4];
  dtostrf(ozone, 5, 2, ozone_ch);
  char unified[20];
  strcpy(unified, ozone_ch);
  strcat(unified, "-ozone");
  int len = (sizeof(unified)) -1;
  for (int i = 0; i< len; i++) {
    return_value[i] = unified[i];
  }   
}

void getPM25(char return_value[]) {
  float pm25 = 0.00;
  //inserire operazioni sensore
  char pm25_ch[6];
  dtostrf(pm25, 7, 2, pm25_ch);
  char unified[20];
  strcpy(unified, pm25_ch);
  strcat(unified, "-pm25");
  int len = (sizeof(unified)) -1;
  for (int i = 0; i< len; i++) {
    return_value[i] = unified[i];
  }  
}


//funzioni sensori
double getPressure()
{
  char status;
  double T,P,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
