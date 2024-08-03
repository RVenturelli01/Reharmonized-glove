#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>
#include <WiFiNINA.h>

//WiFi:
char ssid[] = "AndroidAP3BCD";   // il nome della tua rete WiFi
char pass[] = "jqvy3017";  // la password della tua rete WiFi
char server[] = "192.168.45.96";  // indirizzo IP del portatile
int port = 12345;  // porta su cui il server ascolta
WiFiClient client;

// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 15000;
const float Vin = 3.3;

// for the normalization
const float R_flex_limits[3][2] = { { 12200, 39600 },     //pollice
                                    { 16300, 57500 },     //indice
                                    { 15900, 65800 } };  //medio

const float R_flex_vel_limits[3][2] = { { -20, 20 },     //pollice
                                        { -18, 18 },     //indice
                                        { -16, 16 } };  //medio

// pins
const int flex_sens_pins[3] = { A0, A1, A2 };

long times_prec[3] = { 0, 0, 0 };

float R_flex_norms_filtered_prec[3] = { 0, 0, 0 };
float R_flex_norms_filtered[3] = { 0, 0, 0 };
float a = 0;  // position filter

float R_flex_vel_filtered[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[3] = { 0, 0, 0 };
float R_flex_vel_norms_filtered[3] = { 0, 0, 0 };
float b = 0.8;  // velocity filter

int midi_messages[3][3] = { { 0, 0, 0 },
                            { 0, 0, 0 },
                            { 0, 0, 0 } };
int notes[3] = { 38, 36, 46 };
int channel = 9;

// trigger di schmitt using two thresholds
float threshold_on = 0.65;
float threshold_off = 0.5;
bool flags[3] = { false, false, false };


void setup() {

  for (int i = 0; i < 3; i++) {
    pinMode(flex_sens_pins[i], INPUT);
  }

  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);
    Serial.print("Connecting to WiFi... ");
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

  for (int i = 0; i < 3; i++) {
    float Vout = analogRead(flex_sens_pins[i]) * 3.3 / 1024.;
    long dt = micros() - times_prec[i];
    times_prec[i] = micros();

    //compute finger flexion, normalize and filter it
    float R_flex = R * float(Vout) / float(Vin - Vout);
    float R_flex_norm = (R_flex - R_flex_limits[i][0]) / (R_flex_limits[i][1] - R_flex_limits[i][0]);
    R_flex_norms_filtered[i] = a * R_flex_norms_filtered_prec[i] + (1-a) * R_flex_norm;
    
    //compute finger flexion derivative, filter and normalize it
    float R_flex_vel = (R_flex_norms_filtered[i] - R_flex_norms_filtered_prec[i]) * 1000000. / dt;
    R_flex_vel_filtered[i] = b * R_flex_vel_filtered_prec[i] + (1-b) * R_flex_vel;
    R_flex_vel_norms_filtered[i] = (R_flex_vel_filtered[i] - R_flex_vel_limits[i][0]) / (R_flex_vel_limits[i][1] - R_flex_vel_limits[i][0]);

    if (R_flex_norms_filtered[i] >= threshold_on && flags[i] == false) {
      //constract the midi message
      midi_messages[i][0] = 0x90 + channel;  //suona la nota
      midi_messages[i][1] = notes[i];
      midi_messages[i][2] = map(R_flex_vel_filtered[i], 0, R_flex_vel_limits[i][1], 50, 127);
      String midiInfo = String(midi_messages[i][0]) + ";" + String(midi_messages[i][1]) + ";" + String(midi_messages[i][2]) + "[";
      //send the midi message over wifi
      sendData(midiInfo);
      flags[i] = true;

    } else if (R_flex_norms_filtered[i] < threshold_off && flags[i] == true) {
      flags[i] = false;
    }
    
    //update the prec values for the filter
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
  }
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  float y = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return sat(y, out_min, out_max);
}

float sat(float a, float min, float max) {
  if (a > max) return max;
  if (a < min) return min;
  return a;
}

void connectToServer() {
  Serial.print("Connecting to server... ");
  while (!client.connect(server, port)) {
    Serial.print("Connecting to server... ");
    delay(1000);  // Riprova ogni 5 secondi
  }
  Serial.println("Connected to server");
}

void sendData(String data) {
  if (client.connected()) {
    client.print(data);
    Serial.println(data);
  } else {
    Serial.println("Failed to send data: Not connected to server");
  }
}