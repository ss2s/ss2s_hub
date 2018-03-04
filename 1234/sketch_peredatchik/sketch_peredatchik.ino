#include <SPI.h>


#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>


RF24 radio(9, 10);
const uint64_t pipe = 0xF0F0F0F000LL;
int msg[1];
int potpin_1 = A0;
int val_1;
int potpin_2 = A1;
int val_2;
int potpin_3 = A2;
int va1_3;
int potpin_4 = A3;
int val_4;
int potpin_5 = A4;
int val_5;




void setup(void) {
  radio.begin();
  radio.openWritingPipe(pipe);

}
void loop() {
  val_1 = analogRead(potpin_1), val_1 = map(val_1, 0, 1023, 0, 180), msg[0] = val_1, radio.write(msg, 2);
  val_2 = analogRead(potpin_2), val_2 = map(val_2, 0, 1023, 200, 380), msg[0] = val_2, radio.write(msg, 2);
  va1_3 = analogRead(potpin_3), va1_3 = map(va1_3, 0, 1023, 400, 580), msg[0] = va1_3, radio.write(msg, 2);
  val_4 = analogRead(potpin_4), val_4 = map(val_4, 0, 1023, 600, 780), msg[0] = val_4, radio.write(msg, 2);
  val_5 = analogRead(potpin_5), val_5 = map(val_5, 0, 1023, 800, 980), msg[0] = val_5, radio.write(msg, 2);






}

