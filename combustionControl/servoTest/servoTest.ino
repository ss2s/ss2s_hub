#define servoPin 12  // к этому выходу подключить серву
byte resVal = 45;    // начальный угол серво

#include <Servo.h>
Servo myservo;
int val;

void setup() {
  	myservo.attach(servoPin);
  	Serial.begin(9600);
  	myservo.write(resVal);
}

void loop() {
	val = 5;
	while(Serial.available() == 0){}
	val = Serial.read();
	switch (val) {
	    case '-':
	    	resVal --;
	    	if(resVal < 0){resVal = 0;}
	    	Serial.println(resVal);
	    	myservo.write(resVal);
	    break;
	    case '+':
	    	resVal ++;
	    	if(resVal > 180){resVal = 180;}
	    	Serial.println(resVal);
	    	myservo.write(resVal);
	    break;
	}
  	delay(100);
}
