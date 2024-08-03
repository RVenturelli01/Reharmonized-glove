#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>

// // Replace with your network credentials
// const char* ssid = "AndroidAP3BCD";
// const char* password = "jqvy3017";
const char* ssid = "iliadbox-60F315";
const char* password = "napolicuore";

// Create an instance of the server on port 80
WiFiServer server(80);

// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout ) 
const float R = 10000;
const float Vin = 3.3;

// for the normalization
const float R_flex_limits[3][2] = {{8700, 21000},  //pollice
                                   {8000, 33000},  //indice
                                   {10000, 40000}};  //medio

// pins
const int flex_sens_pins[3] = {A0, A1, A2};

long times_prec[3] = {0,0,0};
float R_flex_norms_filtered_prec[3] = {0,0,0};
float R_flex_norms_filtered[3] = {0,0,0};
float R_flex_vel_filtered[3] = {0,0,0};
float R_flex_vel_filtered_prec[3] = {0,0,0};
int status[3]={0x00, 0x00, 0x00};
bool flags[3] = {false,false,false};

int channel = 5;
float soglia = 0.7;

void setup() {
  Serial.begin(115200);

  for(int i=0;i<3;i++){
    pinMode(flex_sens_pins[i], INPUT);
  }

  wifi_connection();
}

void loop() {
  for(int i=0;i<3;i++){
    float Vout = analogRead(flex_sens_pins[i])*3.3/1024.;
    float R_flex = R*float(Vout)/float(Vin-Vout);
    float R_flex_norm = (R_flex-R_flex_limits[i][0])/(R_flex_limits[i][1]-R_flex_limits[i][0]);
    long dt = micros()-times_prec[i];
    R_flex_norms_filtered[i] = 0.1*R_flex_norm + 0.9*R_flex_norms_filtered_prec[i];
    //R_flex_norms_filtered[i] = R_flex_norm;
    //Serial.println(dt);
    float R_flex_vel = (R_flex_norms_filtered[i] - R_flex_norms_filtered_prec[i])*1000000./dt;
    R_flex_vel_filtered[i] = 0.1*R_flex_vel + 0.9*R_flex_vel_filtered_prec[i];

    times_prec[i] = micros();
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
  }

  /*
  String midiInfo = String(playDita[0]) + ", " + String(60) + ", " + String(127) + "\n"
                  + String(playDita[1]) + ", " + String(70) + ", " + String(127) + "\n"
                  + String(playDita[2]) + ", " + String(80) + ", " + String(127) + "\n";
  Serial.print(midiInfo);
  */
  Serial.println(status[1]);

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
        if (request.indexOf("/flex") != -1) {

          for(int i=0;i<3;i++){
            if(R_flex_norms_filtered[i] >= soglia && flags[i] == false){
              status[i] = 0x90 + channel;  //suona la nota
              flags[i] = true;
            }else if(R_flex_norms_filtered[i] < soglia && flags[i] == true){
              status[i] = 0x80 + channel;  //elimina la nota
              flags[i] = false;
            }else{
              status[i] = 0x00 + channel;  //elimina la nota
            }
          }

          String midiInfo = String(status[0]) + ";" + String(60) + ";" + String(127) + ";"
                          + String(status[1]) + ";" + String(70) + ";" + String(127) + ";"
                          + String(status[2]) + ";" + String(80) + ";" + String(127);
                          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close");
          client.println();
          client.println(midiInfo);
        }
        break;
      }
    }
    // Give the client time to receive the data
    delay(1);
    // Close the connection
    client.stop();
  }
}



void wifi_connection(){
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
}


