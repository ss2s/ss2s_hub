#include "UXtimer.h"

UXtimer t1(1000);  // t1 init 1s
UXtimer t2(2000);  // t2 init 2s
UXtimer t3(2000);  // t3 init 2s
UXtimer t4(2000);  // t4 init 2s
UXtimer t5(2000);  // t5 init 2s
UXtimer t6(2000);  // t6 init 2s
UXtimer t7(2000);  // t7 init 2s

void setup(){

	Serial.begin(250000);

	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);

	digitalWrite(8, LOW);
	digitalWrite(9, LOW);
	digitalWrite(10, LOW);
	digitalWrite(11, LOW);
	digitalWrite(12, LOW);
	digitalWrite(13, LOW);
}

float x_val = 1000;

void loop(){

	// if (Serial.available() > 0) {
	//     x_val = Serial.parseInt();
	// }

	// t1.run(tFunc1, 1 * x_val);
	// t2.run(tFunc2, 2 * x_val);
	// t3.run(tFunc3, 4 * x_val);
	// t4.run(tFunc4, 8 * x_val);
	// t5.run(tFunc5, 16 * x_val);
	// t6.run(tFunc6, 32 * x_val);

	pulsePin(8);
	pulsePin(9);
	pulsePin(10);
	pulsePin(11);
	pulsePin(12);
	pulsePin(13);

	// if(t1.over(1000)){digitalWrite(13, !digitalRead(13));}

	// t7.end(0,0,1,30);
}

static void toglePin(uint8_t _pin = 13){
	digitalWrite(_pin, !digitalRead(_pin));
}

static void tFunc1(){
	toglePin(8);
}
static void tFunc2(){
	toglePin(9);
}
static void tFunc3(){
	toglePin(10);
}
static void tFunc4(){
	toglePin(11);
}
static void tFunc5(){
	toglePin(12);
}
static void tFunc6(){
	toglePin(13);
}

static void pulsePin(uint8_t _pin = 13){
	if(_pin == 8){if(t1.overst(1000)){digitalWrite(_pin,HIGH);}}
	else if(_pin == 9){if(t2.overst(1000)){digitalWrite(_pin,HIGH);}}
	else if(_pin == 10){if(t3.overst(1000)){digitalWrite(_pin,HIGH);}}
	else if(_pin == 11){if(t4.overst(1000)){digitalWrite(_pin,HIGH);}}
	else if(_pin == 12){if(t5.overst(1000)){digitalWrite(_pin,HIGH);}}
	else if(_pin == 13){if(t6.overst(1000)){digitalWrite(_pin,HIGH);}}
	if(_pin == 8){if(t1.overwr(100)){digitalWrite(_pin,LOW);}}
	else if(_pin == 9){if(t2.overwr(200)){digitalWrite(_pin,LOW);}}
	else if(_pin == 10){if(t3.overwr(300)){digitalWrite(_pin,LOW);}}
	else if(_pin == 11){if(t4.overwr(400)){digitalWrite(_pin,LOW);}}
	else if(_pin == 12){if(t5.overwr(500)){digitalWrite(_pin,LOW);}}
	else if(_pin == 13){if(t6.overwr(600)){digitalWrite(_pin,LOW);}}
}