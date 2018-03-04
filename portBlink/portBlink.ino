
#define DEFportDEF 0
#define DEFpinDEF 8

void setup() {
  DDRB = 1<<DEFportDEF;
}

void loop() {
  PORTB |= 1<<DEFportDEF;
  //PORTB |= B1;
  digitalWrite(13, digitalRead(DEFpinDEF));
  delay(100);
  PORTB &= 0<<DEFportDEF;
  //PORTB &= B11111110;
  digitalWrite(13, digitalRead(DEFpinDEF));
  delay(1000);
}
