// comparazione tra due modi diversi di calcolare la velocità, 
// nel primo caso R_flex_vel viene calcolata usando la norma filtrata di R_flex
// nel secondo caso R_flex_vel2 viene calcolata usando R_flex non normalizzato e non filtrato
// Quindi vedendola in schemi a blocchi nel primo caso abbiamo un singolo ramo con filtro(a)->norma->derivazione->filtro(b)->norma
// mentre nel secondo caso abbiamo due rami con: filtro(a)->norma e derivazione->filtro(b)->norma


// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout )
const float R = 15000;
const float Vin = 3.3;

// for the normalization
const float R_flex_limits[3][2] = { { 8700, 21000 },     //pollice
                                    { 8000, 33000 },     //indice
                                    { 10000, 40000 } };  //medio
// for the normalization
const float R_flex_vel_limits[3][2] = { { -40, 40 },    //pollice
                                        { -40, 40 },    //indice
                                        { -40, 40 } };  //medio

const float R_flex_vel_limits2[3][2] = { { -800000, 800000 },    //pollice
                                        { -800000, 800000 },    //indice
                                        { -800000, 800000 } };  //medio

// pins
const int flex_sens_pins[3] = { A0, A1, A2 };

long times_prec[3] = { 0, 0, 0 };

float R_flex[3] = { 0, 0, 0 };
float R_flex_prec[3] = { 0, 0, 0 };
float R_flex_filtered[3] = { 0, 0, 0 };
float R_flex_filtered_prec[3] = { 0, 0, 0 };
float R_flex_norms_filtered[3] = { 0, 0, 0 };
float R_flex_norms_filtered_prec[3] = { 0, 0, 0 };

float R_flex_vel[3] = { 0, 0, 0 };
float R_flex_vel_filtered[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec[3] = { 0, 0, 0 };
float R_flex_vel_norms_filtered[3] = { 0, 0, 0 };

float R_flex_vel2[3] = { 0, 0, 0 };
float R_flex_vel_filtered2[3] = { 0, 0, 0 };
float R_flex_vel_filtered_prec2[3] = { 0, 0, 0 };
float R_flex_vel_norms_filtered2[3] = { 0, 0, 0 };

float a = 0.8, b = 0.8;


void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(flex_sens_pins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    float Vout = analogRead(flex_sens_pins[i]) * 3.3 / 1024.;
    long dt = micros() - times_prec[i];
    times_prec[i] = micros();

    R_flex[i] = R * float(Vout) / float(Vin - Vout);
    R_flex_filtered[i] = a * R_flex_filtered_prec[i] + (1 - a) * R_flex[i];
    R_flex_norms_filtered[i] = (R_flex[i] - R_flex_limits[i][0]) / (R_flex_limits[i][1] - R_flex_limits[i][0]);

    R_flex_vel[i] = (R_flex_norms_filtered[i] - R_flex_norms_filtered_prec[i]) * 1000000. / dt;
    R_flex_vel_filtered[i] = b * R_flex_vel_filtered_prec[i] + (1 - b) * R_flex_vel[i];
    R_flex_vel_norms_filtered[i] = (R_flex_vel_filtered[i] - R_flex_vel_limits[i][0]) / (R_flex_vel_limits[i][1] - R_flex_vel_limits[i][0]);

    R_flex_vel2[i] = (R_flex[i] - R_flex_prec[i]) * 1000000. / dt;
    R_flex_vel_filtered2[i] = b * R_flex_vel_filtered_prec2[i] + (1 - b) * R_flex_vel2[i];
    R_flex_vel_norms_filtered2[i] = (R_flex_vel_filtered2[i] - R_flex_vel_limits2[i][0]) / (R_flex_vel_limits2[i][1] - R_flex_vel_limits2[i][0]);


    R_flex_filtered_prec[i] = R_flex_filtered[i];
    R_flex_prec[i] = R_flex[i];
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];

    R_flex_vel_filtered_prec[i] = R_flex_vel_filtered[i];
    R_flex_norms_filtered_prec[i] = R_flex_norms_filtered[i];

    R_flex_vel_filtered_prec2[i] = R_flex_vel_filtered2[i];
  }


  Serial.print((R_flex_vel[1] - R_flex_vel_limits[1][0]) / (R_flex_vel_limits[1][1] - R_flex_vel_limits[1][0]));
  Serial.print(" ");
  Serial.print(R_flex_vel_norms_filtered[1]);
  Serial.print(" ");
  Serial.print((R_flex_vel2[1] - R_flex_vel_limits2[1][0]) / (R_flex_vel_limits2[1][1] - R_flex_vel_limits2[1][0]));
  Serial.print(" ");
  Serial.println(R_flex_vel_norms_filtered2[1]);
  delay(1);
}




