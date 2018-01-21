void setup() {
  DDRB = 1<<5;
}

void loop() {
  PORTB = 1<<5;
  //digitalWrite(13, HIGH);
  delay(1000);
  PORTB = 0<<5;
  //digitalWrite(13, LOW);
  delay(1000);
}
