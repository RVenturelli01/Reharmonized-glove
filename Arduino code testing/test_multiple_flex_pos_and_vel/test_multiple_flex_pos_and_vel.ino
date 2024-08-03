// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 10000;
const float Vin = 3.3;

// for the normalization
const float R_flex_limits[3][2] = { { 8700, 21000 },     //pollice
                                    { 8000, 33000 },     //indice
                                    { 10000, 40000 } };  //medio
// for the normalization
const float R_flex_vel_limits[3][2] = { { 0, 0 },     //pollice
                                        { 0, 0 },     //indice
                                        { 0, 0 } };  //medio

// pins
const int flex_sens_pins[3] = { A0, A1, A2 };

long times_prec[3] = { 0, 0, 0 };

float R_flex_norms_filtered_prec[3] = { 0, 0, 0 };
float R_flex_norms_filtered[3] = { 0, 0, 0 };

float R_flex_vel_filtered[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[3] = { 0, 0, 0 };
float R_flex_vel_norms_filtered[3] = { 0, 0, 0 };


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

    float a = 0.2;
    R_flex_norms_filtered[i] = a * R_flex_norms_filtered_prec[i] + (1-a) * R_flex_norm;
    
    a = 0.8;
    float R_flex_vel = (R_flex_norms_filtered[i] - R_flex_norms_filtered_prec[i]) * 1000000. / dt;
    R_flex_vel_filtered[i] = a * R_flex_vel_filtered_prec[i] + (1-a) * R_flex_vel;
    R_flex_vel_norms_filtered[i] = (R_flex_vel_filtered[i] - R_flex_vel_limits[i][0]) / (R_flex_vel_limits[i][1] - R_flex_vel_limits[i][0]);

    times_prec[i] = micros();
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];
    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
  }
  
  // Serial.print(R_flex_vel_filtered[0]);
  // Serial.print(" ");
  // Serial.print(R_flex_vel_filtered[1]);
  // Serial.print(" ");
  // Serial.println(R_flex_vel_filtered[2]);
  Serial.print(R_flex_vel_filtered[1]);
  Serial.print(" ");
  Serial.println(R_flex_norms_filtered[1]*100);
  delay(1);
}
