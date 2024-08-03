#include <Arduino_LSM6DSOX.h>
#include <MadgwickAHRS.h>
#include <Math.h>

float rollFilteredOld;
float pitchFilteredOld;
Madgwick filter;
const float sensorRate = 104.00;

long prevTime = 0;
float xVel = 0, yVel = 0, zVel = 0;

// variables for the calculation of the cycle time
int Tc = 10000;  // in micros
long t_prec = 0;


void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
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

  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
  float roll, pitch, heading;
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);

    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);

    roll = filter.getRoll();
    pitch = -filter.getPitch();
    float rollFiltered = 0.3 * roll + 0.7 * rollFilteredOld;  // low pass filter
    float pitchFiltered = 0.3 * pitch + 0.7 * pitchFilteredOld;  // low pass filter

    Serial.println(String(rollFiltered) + " " + String(pitchFilteredOld));

    rollFilteredOld = rollFiltered;
    pitchFilteredOld = pitchFiltered;
  }
}