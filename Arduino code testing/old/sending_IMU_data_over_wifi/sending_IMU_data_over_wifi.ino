#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>

// IMU
float rollFiltered;
float pitchFiltered;
float rollFilteredOld;
float pitchFilteredOld;
Madgwick filter;
const float sensorRate = 104.00;

// Replace with your network credentials
const char* ssid = "iliadbox-60F315";
const char* password = "napolicuore";
// const char* ssid = "AndroidAP3BCD";
// const char* password = "jqvy3017";
//const char* ssid = "realme 8 Pro";
//const char* password = "myhotspot";

// Create an instance of the server on port 80
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Initialize the IMU sensor
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  filter.begin(sensorRate);
  Serial.println("IMU initialized");
  Serial.println("Setup complete!");
}

void loop() {

  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  float roll, pitch, heading;          
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro); 
    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
    roll = filter.getRoll();
    pitch = filter.getPitch();
    rollFiltered = 0.1 * roll + 0.9 * rollFilteredOld; // low pass filter
    pitchFiltered = 0.1 * pitch + 0.9 * pitchFilteredOld; // low pass filter
    rollFilteredOld = rollFiltered;
    pitchFilteredOld = pitchFiltered;
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client connected");
    while (client.connected()) {
      if (client.available()) {
        // Read the request
        String request = client.readStringUntil('\r');
        client.flush();

        // Send the IMU data if the request is for "/imu"
        if (request.indexOf("/imu") != -1) {
          
          String imuData = "Roll: " + String(rollFiltered) + " Pitch: " + String(pitchFiltered);
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close");
          client.println();
          client.println(imuData);
        }
        break;
      }
    }
    // Give the client time to receive the data
    //delay(1);
    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
