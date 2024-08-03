// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 15000;
const float Vin = 3.3;

long t_prec = 0;
long Tc = 10000;  //in micros

// for the flex sensor normalizations
const float R_flex_limits[3][2] = { { 14639, 43565 },    //pollice
                                    { 16128, 64477 },    //indice
                                    { 16309, 69587 } };  //medio

const float R_flex_vel_limits[3][2] = { { -331080, 331080 },    //pollice
                                        { -699100, 699100 },    //indice
                                        { -777320, 777320 } };  //medio

// pins
const int flex_sens_pins[3] = { A0, A1, A2 };

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

float a = exp(-2.*3.14*10.*0.01);  // z=a -> s=ln(a)/Tc polo del low pass filter di R_flex
float b = exp(-2.*3.14*15.*0.01);  // z=b -> s=ln(b)/Tc polo del low pass filter di R_flex_vel

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(flex_sens_pins[i], INPUT);
  }
}

void loop() {

  while (micros() - t_prec < Tc) {
  }
  //Serial.println(micros() - t_prec);
  t_prec = micros();

  for (int i = 0; i < 3; i++) {
    float Vout = analogRead(flex_sens_pins[i]) * 3.3 / 1024.;
    long dt = micros() - times_prec[i];
    times_prec[i] = micros();

    // calculation of the finger flexion
    R_flex[i] = R * float(Vout) / float(Vin - Vout);
    // apply the filter on R_flex->[filter]->R_flex_filtered
    R_flex_filtered[i] = a * R_flex_filtered_prec[i] + (1 - a) * R_flex[i];
    // do the normalization of the R_flex_filtered
    R_flex_norms_filtered[i] = map(R_flex_filtered[i], R_flex_limits[i][0], R_flex_limits[i][1], 0, 1);
    
    // apply the derivation R_flex->[(z-1)/z]->R_flex_vel
    R_flex_vel[i] = (R_flex[i] - R_flex_prec[i]) * 1000000. / dt;
    // apply the filter on R_flex_vel->[filter]->R_flex_vel_filtered
    R_flex_vel_filtered[i] = b * R_flex_vel_filtered_prec[i] + (1 - b) * R_flex_vel[i];
    // do the normalization of the R_flex_vel_filtered
    R_flex_vel_norms_filtered[i] = map(R_flex_vel_filtered[i], R_flex_vel_limits[i][0], R_flex_vel_limits[i][1], -1, 1);
    
    // update for the new iteration
    R_flex_prec[i] = R_flex[i];
    R_flex_filtered_prec[i] = R_flex_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
  }

  Serial.print(R_flex_norms_filtered[0]);
  Serial.print("  ");
  Serial.print(R_flex_norms_filtered[1]);
  Serial.print("  ");
  Serial.println(R_flex_norms_filtered[2]);
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
