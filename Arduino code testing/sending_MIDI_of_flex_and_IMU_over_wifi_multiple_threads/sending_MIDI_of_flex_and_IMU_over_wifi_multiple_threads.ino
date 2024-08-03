#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>
#include <WiFiNINA.h>

//WiFi:
// char ssid[] = "AndroidAP3BCD";           // il nome della tua rete WiFi
// char pass[] = "jqvy3017";                // la password della tua rete WiFi
// char server[] = "192.168.45.96";         // indirizzo IP del portatile

char ssid[] = "iliadbox-60F315";                // il nome della tua rete WiFi
char pass[] = "napolicuore";                    // la password della tua rete WiFi
char server[] = "192.168.1.71";                 // indirizzo IP del portatile
int ports[3] = { 12345, 12346, 12347 };  // porta su cui il server ascolta
WiFiClient clients[3];

// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 15000;
const float Vin = 3.3;

// for the normalization
const float R_flex_limits[3][2] = { { 12200, 39600 },    //pollice
                                    { 16300, 57500 },    //indice
                                    { 15900, 65800 } };  //medio

const float R_flex_vel_limits[3][2] = { { -20, 20 },    //pollice
                                        { -18, 18 },    //indice
                                        { -16, 16 } };  //medio

// pins
const int flex_sens_pins[3] = { A0, A1, A2 };

long t_prec = 0;
long times_prec[3] = { 0, 0, 0 };

float R_flex[3] = { 0, 0, 0 };
float R_flex_filtered[3] = { 0, 0, 0 };
float R_flex_norms_filtered[3] = { 0, 0, 0 };
float R_flex_prec[3] = { 0, 0, 0 };
float R_flex_filtered_prec[3] = { 0, 0, 0 };

float R_flex_vel[3] = { 0, 0, 0 };
float R_flex_vel_filtered[3] = { 0, 0, 0 };
float R_flex_vel_norms_filtered[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[3] = { 0, 0, 0 };

float a = 0.2;  // z=a -> s=ln(a)/Tc polo del low pass filter di R_flex
float b = 0.8;  // z=b -> s=ln(b)/Tc polo del low pass filter di R_flex_vel

// midi settings
int midi_messages[3][3] = { { 0, 0, 0 },
                            { 0, 0, 0 },
                            { 0, 0, 0 } };
int notes[3] = { 38, 36, 46 };
int channel_index = 0;
const int total_instruments = 3;
int channels[total_instruments] = { 9, 0, 1 };

// trigger di schmitt using two thresholds
float threshold_roll_on = 60;
float threshold_roll_off = 40;
float threshold_on = 0.65;
float threshold_off = 0.5;
bool flag_roll = false;
bool flags[3] = { false, false, false };

float rollFilteredOld;
float pitchFilteredOld;
Madgwick filter;
const float sensorRate = 104.00;

int Tc = 10000;  // in micros


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

  for (int i = 0; i < 3; i++) {
    connectToServer(i);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }
  filter.begin(sensorRate);
  Serial.println("Setup complete!");
}




void loop() {
  while (micros() - t_prec < Tc) {
  }
  Serial.println(micros() - t_prec);
  t_prec = micros();

  for (int i = 0; i < 3; i++) {
    if (!clients[i].connected()) {
      Serial.println("Disconnected from server, attempting to reconnect...");
      connectToServer(i);
    }
  }

  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  float roll, pitch, heading;
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);

    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);

    roll = filter.getRoll();
    pitch = filter.getPitch();
    float rollFiltered = 0.3 * roll + 0.7 * rollFilteredOld;     // low pass filter
    float pitchFiltered = 0.3 * pitch + 0.7 * pitchFilteredOld;  // low pass filter

    if (rollFiltered >= threshold_roll_on && flag_roll == false) {
      // change the instrument
      channel_index = (channel_index + 1) % total_instruments;
      flag_roll = true;

    } else if (rollFiltered < threshold_roll_off && flag_roll == true) {
      flag_roll = false;
    }

    rollFilteredOld = rollFiltered;
    pitchFilteredOld = pitchFiltered;
  }

  for (int i = 0; i < 3; i++) {
    float Vout = analogRead(flex_sens_pins[i]) * 3.3 / 1024.;
    long dt = micros() - times_prec[i];
    times_prec[i] = micros();

    // calculation of the finger flexion
    R_flex[i] = R * float(Vout) / float(Vin - Vout);
    // apply the filter on R_flex->[filter]->R_flex_filtered
    R_flex_filtered[i] = a * R_flex_filtered_prec[i] + (1 - a) * R_flex[i];
    // do the normalization of the R_flex_filtered
    R_flex_norms_filtered[i] = (R_flex[i] - R_flex_limits[i][0]) / (R_flex_limits[i][1] - R_flex_limits[i][0]);

    // apply the derivation R_flex->[(z-1)/z]->R_flex_vel
    R_flex_vel[i] = (R_flex[i] - R_flex_prec[i]) * 1000000. / dt;
    // apply the filter on R_flex_vel->[filter]->R_flex_vel_filtered
    R_flex_vel_filtered[i] = b * R_flex_vel_filtered_prec[i] + (1 - b) * R_flex_vel[i];
    // do the normalization of the R_flex_vel_filtered
    R_flex_vel_norms_filtered[i] = (R_flex_vel_filtered[i] - R_flex_vel_limits[i][0]) / (R_flex_vel_limits[i][1] - R_flex_vel_limits[i][0]);


    if (R_flex_norms_filtered[i] >= threshold_on && flags[i] == false) {
      //constract the midi message
      midi_messages[i][0] = 0x90 + channels[channel_index];  //suona la nota
      midi_messages[i][1] = notes[i];
      midi_messages[i][2] = map(R_flex_vel_filtered[i], 0, R_flex_vel_limits[i][1], 50, 127);
      String midiInfo = String(midi_messages[i][0]) + ";" + String(midi_messages[i][1]) + ";" + String(midi_messages[i][2]) + "[";
      //send the midi message over wifi
      sendData(midiInfo, i);
      flags[i] = true;

    } else if (R_flex_norms_filtered[i] < threshold_off && flags[i] == true) {
      flags[i] = false;
    }

    // update for the new iteration
    R_flex_prec[i] = R_flex[i];
    R_flex_filtered_prec[i] = R_flex_filtered[i];
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

void connectToServer(int i) {
  Serial.print("Connecting to server " + String(i - 1) + " con la porta " + String(ports[i]));
  while (!clients[i].connect(server, ports[i])) {
    Serial.print("Connecting to server " + String(i - 1) + " con la porta " + String(ports[i]));
    delay(1000);  // Riprova ogni 5 secondi
  }
  Serial.println("Connected to server");
}

void sendData(String data, int i) {
  if (clients[i].connected()) {
    clients[i].print(data);
    Serial.println(data);
  } else {
    Serial.println("Failed to send data: Not connected to server");
  }
}