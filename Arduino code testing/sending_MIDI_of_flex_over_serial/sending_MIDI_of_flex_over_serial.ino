#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>

// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 15000;
const float Vin = 3.3;

// // for the normalization
// const float R_flex_limits[3][2] = { { 8700, 21000 },     //pollice
//                                     { 8000, 33000 },     //indice
//                                     { 10000, 40000 } };  //medio
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
float R_flex_vel_filtered[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[3] = { 0, 0, 0 };
int midi_messages[3][3] = { { 0, 0, 0 },
                            { 0, 0, 0 },
                            { 0, 0, 0 } };
int notes[3] = { 38, 36, 46 };
bool flags[3] = { false, false, false };

int channel = 9;
float soglia = 0.5;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(flex_sens_pins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    float Vout = analogRead(flex_sens_pins[i]) * 3.3 / 1024.;
    float R_flex = R * float(Vout) / float(Vin - Vout);
    float R_flex_norm = (R_flex - R_flex_limits[i][0]) / (R_flex_limits[i][1] - R_flex_limits[i][0]);
    long dt = micros() - times_prec[i];
    
    float a = 0;
    R_flex_norms_filtered[i] = a * R_flex_norms_filtered_prec[i] + (1-a) * R_flex_norm;
    
    a = 0.95;
    float R_flex_vel = (R_flex_norms_filtered[i] - R_flex_norms_filtered_prec[i]) * 1000000. / dt;
    R_flex_vel_filtered[i] = a * R_flex_vel_filtered_prec[i] + (1-a) * R_flex_vel;

    times_prec[i] = micros();
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];

    if (R_flex_norms_filtered[i] >= soglia && flags[i] == false) {
      midi_messages[i][0] = 0x90 + channel;  //suona la nota
      midi_messages[i][1] = notes[i];
      midi_messages[i][2] = map(R_flex_vel_filtered[i], 0, R_flex_vel_limits[i][1], 50, 127);
      flags[i] = true;
      String midiInfo = String(midi_messages[i][0]) + ";" + String(midi_messages[i][1]) + ";" + String(midi_messages[i][2]);
      Serial.println(midiInfo);

    } else if (R_flex_norms_filtered[i] < soglia && flags[i] == true) {
      midi_messages[i][0] = 0x80 + channel;  //elimina la nota
      midi_messages[i][1] = notes[i];
      midi_messages[i][2] = 0;
      flags[i] = false;
      String midiInfo = String(midi_messages[i][0]) + ";" + String(midi_messages[i][1]) + ";" + String(midi_messages[i][2]);
      Serial.println(midiInfo);
    }
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
