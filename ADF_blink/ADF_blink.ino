#define PLATFORM 328
#include "ADF.h"

void setup() {
	ADF_PMO13;
	ADF_PMO10;
}

void loop() {
	ADF_DWH13;
	//digitalWrite(13, ADF_DR10);
	delay(200);
	ADF_DWL13;
	//digitalWrite(13, ADF_DR10);
	delay(200);
}
