#define PLATFORM 2560
#include "ADF.h"

unsigned long startMil;
unsigned long stopMil;
unsigned long incomingVal;
unsigned long ciklCount1 = 140590;  // 140590 1589000
unsigned long ciklCount2 = 1589000;  // 140590 1589000

void setup() {
	Serial.begin(9600);
	delay(200);
	ADF_PMO13;
	Serial.println("");
	Serial.println("start test 1 ");
	Serial.print(ciklCount1);
	Serial.println(" ciklCount1 ");
	startMil = millis();

	for(unsigned long i=0; i < ciklCount1; i++){
		ADF_DWH13;
		ADF_DWL13;	
	}
	stopMil = millis();
	incomingVal = stopMil - startMil;
	Serial.print("ADF_DW - ");
	Serial.print(incomingVal);
	Serial.println(" ms ");
	startMil = millis();

	for(unsigned long i=0; i < ciklCount1; i++){
		digitalWrite(13, HIGH);
		digitalWrite(13, LOW);
	}
	stopMil = millis();
	incomingVal = stopMil - startMil;
	Serial.print("digitalWrite - ");
	Serial.print(incomingVal);
	Serial.println(" ms ");
	Serial.println("test 1 ok ");

	Serial.println("start test 2 ");
	Serial.print(ciklCount2);
	Serial.println(" ciklCount2 ");
	startMil = millis();

	for(unsigned long i=0; i < ciklCount2; i++){
		ADF_DWH13;
		ADF_DWL13;	
	}
	stopMil = millis();
	incomingVal = stopMil - startMil;
	Serial.print("ADF_DW - ");
	Serial.print(incomingVal);
	Serial.println(" ms ");
	startMil = millis();

	for(unsigned long i=0; i < ciklCount2; i++){
		digitalWrite(13, HIGH);
		digitalWrite(13, LOW);
	}
	stopMil = millis();
	incomingVal = stopMil - startMil;
	Serial.print("digitalWrite - ");
	Serial.print(incomingVal);
	Serial.println(" ms ");
	Serial.println("test 2 ok ");
	Serial.println("stop ");
}

void loop() {
}
