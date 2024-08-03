// flex sensor: flat = 7KOHM-13KOHM, 180* bend = minimum 2Xflat resistance
// using a voltage divider: Vout = Vin * R_flex / ( R_flex + R ) --> R_flex = R * Vout / ( Vin - Vout ) 
const float R = 15000;
const float Vin = 3.3;

// for the normalization
// medio A2
const float R_flex_min = 10000;
const float R_flex_max = 40000;

//indice A1
//const float R_flex_min = 8000;
//const float R_flex_max = 33000;

//pollice A0
//const float R_flex_min = 8700;
//const float R_flex_max = 21000;

// pins
const int flex_sens_1 = A2;



void setup() {
  Serial.begin(115200);
  pinMode(flex_sens_1, INPUT);
}

void loop() {
  float Vout = analogRead(flex_sens_1)*3.3/1024.;
  float R_flex = R*float(Vout)/float(Vin-Vout);
  //float R_flex = limits(R_flex, R_flex_min, R_flex_max);
  float R_flex_norm = (R_flex-R_flex_min)/(R_flex_max-R_flex_min);

  //Serial.print("resistenza flex sensor (KOHM)  ");
  Serial.print(R_flex/1000.);
  //Serial.print("resistenza normalizzata  ");
  Serial.print("  ");
  Serial.println(R_flex_norm);
}
