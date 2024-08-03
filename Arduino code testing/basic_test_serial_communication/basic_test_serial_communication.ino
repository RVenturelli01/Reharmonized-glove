
void setup() {
  Serial.begin(115200);
}

void loop() {
  delay(1000);
  Serial.println("A11");
  Serial.println("F1200");
  delay(1000);
  Serial.println("A22");
  Serial.println("F2400");
  delay(1000);
  Serial.println("A12");
  Serial.println("F1800");
}
