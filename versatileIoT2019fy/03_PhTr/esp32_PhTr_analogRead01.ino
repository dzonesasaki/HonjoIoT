#define PIN_READ 25

void setup() {
  Serial.begin(115200);
}
void loop() {
  Serial.println(analogRead(PIN_READ));
  delay(500);
}
