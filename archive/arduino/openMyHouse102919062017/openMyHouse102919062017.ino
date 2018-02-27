
#include <Servo.h>

Servo myservo;

void setup() {

  myservo.attach(10);

  myservo.write(180);
  
  delay(600);
  
  myservo.write(0);

}

void loop() {

}
