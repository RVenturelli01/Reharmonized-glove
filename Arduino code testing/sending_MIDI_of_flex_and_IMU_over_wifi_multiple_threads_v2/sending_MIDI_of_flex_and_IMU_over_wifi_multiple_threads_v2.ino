#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>
#include <WiFiNINA.h>

// WiFi settings:
// char ssid[] = "AndroidAP3BCD";           // il nome della tua rete WiFi
// char pass[] = "jqvy3017";                // la password della tua rete WiFi
// char server[] = "192.168.45.96";         // indirizzo IP del portatile

// WiFi settings:
char ssid[] = "iliadbox-60F315";  // il nome della tua rete WiFi
char pass[] = "napolicuore";      // la password della tua rete WiFi
char server[] = "192.168.1.71";   // indirizzo IP del portatile

const int n_clients = 4;
int ports[n_clients] = { 12345, 12346, 12347, 12348 };  // porta su cui il server ascolta
WiFiClient clients[n_clients];

// flex sensor settings
const float R = 15000;
const float Vin = 3.3;
// flex sensor pins
const int n_flex_sensors = 3;
const int flex_sens_pins[n_flex_sensors] = { A0, A1, A2 };

// for the flex sensor normalizations
const float R_flex_limits[n_flex_sensors][2] = { { 12200, 39600 },    //pollice
                                                 { 16300, 57500 },    //indice
                                                 { 15900, 65800 } };  //medio

const float R_flex_vel_limits[n_flex_sensors][2] = { { -20, 20 },    //pollice
                                                     { -18, 18 },    //indice
                                                     { -16, 16 } };  //medio

// variables for the flex sensors elaboration
long times_prec[n_flex_sensors] = { 0, 0, 0 };
//
float R_flex[n_flex_sensors] = { 0, 0, 0 };
float R_flex_filtered[n_flex_sensors] = { 0, 0, 0 };
float R_flex_norms_filtered[n_flex_sensors] = { 0, 0, 0 };
float R_flex_prec[n_flex_sensors] = { 0, 0, 0 };
float R_flex_filtered_prec[n_flex_sensors] = { 0, 0, 0 };
//
float R_flex_vel[n_flex_sensors] = { 0, 0, 0 };
float R_flex_vel_filtered[n_flex_sensors] = { 0, 0, 0 };
float R_flex_vel_norms_filtered[n_flex_sensors] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[n_flex_sensors] = { 0, 0, 0 };
//
float a = 0.6;  // z=a -> s=ln(a)/Tc polo del low pass filter di R_flex
float b = 0.65;  // z=b -> s=ln(b)/Tc polo del low pass filter di R_flex_vel

// midi settings
int channel_index = 0;
const int n_instruments = 3;
int channels[n_instruments] = { 9, 0, 1 };  // each channel corresponds to an instrument
int notes[n_instruments][6] = { { 46, 40, 36, 45, 47, 50 },
                                { 58, 61, 63, 66, 68, 70 },
                                { 76, 75, 71, 74, 72, 69 } };

// change instrument
float threshold_change_on = 0;
float threshold_change_off = 0.3;
bool flag_change = false;

// note on for each finger
float threshold_on = 0.65;
float threshold_off = 0.5;
bool flags[n_flex_sensors] = { false, false, false };

// note on for right roll
float threshold_roll_on = 40;
float threshold_roll_off = 30;
bool flag_roll = false;

// note on for up pitch
float threshold_pitch_up_on = 35;
float threshold_pitch_up_off = 30;
bool flag_pitch_up = false;

// note on for down pitch
float threshold_pitch_down_on = -30;
float threshold_pitch_down_off = -20;
bool flag_pitch_down = false;

// IMU variables
float rollFilteredOld;
float pitchFilteredOld;
Madgwick filter;
const float sensorRate = 104.00;

// variables for the calculation of the cycle time
int Tc = 10000;  // in micros
long t_prec = 0;


void setup() {

  for (int i = 0; i < n_flex_sensors; i++) {
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

  for (int i = 0; i < n_clients; i++) {
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
  //Serial.println(micros() - t_prec);
  t_prec = micros();

  for (int i = 0; i < n_clients; i++) {
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
    pitch = -filter.getPitch();

    float rollFiltered = 0.7 * roll + 0.3 * rollFilteredOld;     // low pass filter
    float pitchFiltered = 0.7 * pitch + 0.3 * pitchFilteredOld;  // low pass filter

    if (rollFiltered >= threshold_roll_on && flag_roll == false) {
      //Serial.println("roll");
      //construct and send midi message
      String midiMessage = numbers2midi(0x90 + channels[channel_index], notes[channel_index][3], 127);
      sendData(midiMessage, 3);
      flag_roll = true;

    } else if (rollFiltered < threshold_roll_off && flag_roll == true) {
      flag_roll = false;
    }

    if (pitchFiltered >= threshold_pitch_up_on && flag_pitch_up == false) {
      //Serial.println("pitch_up");
      //construct and send midi message
      String midiMessage = numbers2midi(0x90 + channels[channel_index], notes[channel_index][4], 127);
      sendData(midiMessage, 3);
      flag_pitch_up = true;

    } else if (pitchFiltered < threshold_pitch_up_off && flag_pitch_up == true) {
      flag_pitch_up = false;
    }

    if (pitchFiltered <= threshold_pitch_down_on && flag_pitch_down == false) {
      //Serial.println("pitch_down");
      //construct and send midi message
      String midiMessage = numbers2midi(0x90 + channels[channel_index], notes[channel_index][5], 127);
      sendData(midiMessage, 3);
      flag_pitch_down = true;

    } else if (pitchFiltered > threshold_pitch_down_off && flag_pitch_down == true) {
      flag_pitch_down = false;
    }

    //Serial.println(String(rollFiltered) + " " + String(pitchFiltered) + " " + String(flag_pitch_down * 100));

    rollFilteredOld = rollFiltered;
    pitchFilteredOld = pitchFiltered;
  }

  for (int i = 0; i < n_flex_sensors; i++) {
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
      //construct and send the midi message
      int status = 0x90 + channels[channel_index];
      int note = notes[channel_index][i];
      int vel = map(R_flex_vel_filtered[i], R_flex_vel_limits[i][0], R_flex_vel_limits[i][1], 50, 127);
      String midiMessage = numbers2midi(status, note, vel);
      sendData(midiMessage, i);
      flags[i] = true;

    } else if (R_flex_norms_filtered[i] < threshold_off && flags[i] == true) {
      flags[i] = false;
    }

    // update for the new iteration
    R_flex_prec[i] = R_flex[i];
    R_flex_filtered_prec[i] = R_flex_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
  }

  if (R_flex_norms_filtered[0] <= threshold_change_on && R_flex_norms_filtered[1] <= threshold_change_on && R_flex_norms_filtered[2] <= threshold_change_on && flag_change == false) {
    // change the instrument
    //Serial.println("cambio strumento");
    channel_index = (channel_index + 1) % n_instruments;
    flag_change = true;
  } else if ((R_flex_norms_filtered[0] > threshold_change_off || R_flex_norms_filtered[1] > threshold_change_off || R_flex_norms_filtered[2] > threshold_change_off) && flag_change == true) {
    flag_change = false;
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

String numbers2midi(int status, int note, int vel) {
  return String(status) + ";" + String(note) + ";" + String(vel) + "[";
}

void connectToServer(int i) {
  Serial.print("Connecting to server " + String(i));
  while (!clients[i].connect(server, ports[i])) {
    Serial.print("Connecting to server " + String(i));
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