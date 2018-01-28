#define PLATFORM 2560
#include "ADF.h"

void setup() {
	ADF_PMO13;
}

void loop() {
	ADF_DWH13;
	//digitalWrite(13, ADF_DR10);
	delay(100);
	ADF_DWL13;
	//digitalWrite(13, ADF_DR10);
	delay(500);
}
