## Arduino
Prima versione del firmware della scatola

[**docs**](../docs/hardware/index.md)

- fly-box: Codice per Arduino Nano Every, è contenuta nella scatola volante, esegue le letture dei sensori e li trasmette via radio 
- base-radio: Codice per Arduino Nano Every, è ricevitore radio per fly-box 
- base-master: Codice per Arduino MKR WiFi 1010, riceve i dati via i2c da base-radio e li invia al server HTTP
## Configurazione
- Inserire le credenziali nei file `secret.h`
  ```
  #define WIFI_SSID ""  //inserire il nome della WiFi
  #define WIFI_PASSWORD ""  //inserire la password della WiFi
  #define KEY "" //inserire la chiave di autenticazione per le richieste HTTP alle API
  ```
