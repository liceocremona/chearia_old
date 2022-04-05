## Credenziali connessione wifi
- Creare nelle cartelle `base` e `fly-slave` un file chiamato `secret.h`
- Inserire nei file il seguente codice con le credenziali:
  ```
  #define WIFI_SSID "SSID"  //inserire il nome della WiFi
  #define WIFI_PASSWORD "password"  //inserire la password della WiFi
  #define FIREBASE_HOST ""  //inserire il dominio del DB Firebase
  #define FIREBASE_AUTH "" //inserire il "secret" del DB Firebase
  ```