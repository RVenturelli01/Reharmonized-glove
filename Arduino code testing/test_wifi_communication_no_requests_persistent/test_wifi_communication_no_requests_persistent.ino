#include <WiFiNINA.h>

char ssid[] = "AndroidAP3BCD";   // il nome della tua rete WiFi
char pass[] = "jqvy3017";  // la password della tua rete WiFi

char server[] = "192.168.45.96";  // indirizzo IP del portatile
int port = 12345;  // porta su cui il server ascolta

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // while (!Serial) {
    // ; // attesa per la connessione seriale
  // }

  // Connessione alla rete WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  connectToServer();
}

void loop() {
  if (!client.connected()) {
    Serial.println("Disconnected from server, attempting to reconnect...");
    connectToServer();
  }

  // Simula la lettura dei dati dalla IMU
  float data = readIMUData();

  // Invia i dati al server
  sendData(data);

  // Delay per evitare di inviare dati troppo frequentemente
  delay(100);  // Modifica secondo le tue necessità
}

void connectToServer() {
  Serial.print("Connecting to server...");
  while (!client.connect(server, port)) {
    Serial.print("Mi sto connettendo...");
    delay(5000);  // Riprova ogni 5 secondi
  }
  Serial.println("Connected to server");
}

void sendData(float data) {
  if (client.connected()) {
    client.print(String(0.56));
    Serial.println(data);
  } else {
    Serial.println("Failed to send data: Not connected to server");
  }
}

float readIMUData() {
  // Simula la lettura dei dati dalla IMU
  return 0.56;
}

