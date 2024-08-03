#include <WiFiNINA.h>

char ssid[] = "AndroidAP3BCD";   // il nome della tua rete WiFi
char pass[] = "jqvy3017";  // la password della tua rete WiFi

char server[] = "192.168.45.2";  // indirizzo IP del portatile
int port = 12345;  // porta su cui il server ascolta

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // while (!Serial) {
  //   ; // attesa per la connessione seriale
  // }

  // connessione alla rete WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // inizializzazione della IMU (assicurati di avere il giusto codice di inizializzazione per il tuo sensore)
  // setupIMU();
}

void loop() {
  // controlla se ci sono dati pronti dalla IMU
  // if (dataReadyFromIMU()) {
    float data = readIMUData();  // sostituisci con la tua funzione per leggere i dati dall'IMU

    // connessione al server
    if (client.connect(server, port)) {
      client.print(data);
      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
    // delay per evitare di inviare dati troppo frequentemente
    delay(500);  // modifica secondo le tue necessità
  // }
}

float readIMUData() {
  // simula la lettura dei dati dalla IMU
  return random(100);
}
