#define PORT_ADS 0  // порт ADS1115 куда подключается датчик, от 0 до 3

int FramePerSec = 0;
float curTemp = 0;
float tempValForDot = 0;
float maxTemp100 = 0, maxTemp51 = 0, maxTemp52 = 0;
float voltValForDot = 0.0F;

unsigned long strtMil_1000 = 0;
unsigned long strtMil_20 = 0;
unsigned long intervalMil = 1000;


float multiplierADS = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

static char outstr[15];

#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

float tempT1F(float analogVal){
	float tempValF = ((analogVal / 204.6) - 1.25) / 0.005;
	return tempValF;
}
// time value
unsigned long hourTime = 0;
unsigned long minuteTime = 0;
unsigned long minuteTime60 = 0;
unsigned long secondTime = 0;
unsigned long secondTime60 = 0;
unsigned long milliSecondTime = 0;

unsigned long currentMil = 0;

unsigned long prevMinuteTime = 0;
unsigned long prevSecondTime = 0;
unsigned long prevMilliSecondTime = 0;

inline void timeUpdate(){
	
	currentMil = millis();

	secondTime = currentMil / 1000;
	milliSecondTime = currentMil - (secondTime * 1000);
	minuteTime = secondTime / 60;
	hourTime = minuteTime / 60;
	secondTime60 = secondTime - (minuteTime * 60);
	minuteTime60 = minuteTime - (hourTime * 60);
}

void setup(){
	Serial.begin(250000);
	delay(10);
	Serial.println("100fps   	50fps1  	50fps2");
	Serial.println("");
	Serial.println(7%2);
	Serial.println(8%2);
	Serial.println("");

	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
	// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
	// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
	// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
	// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
	ads.begin();
}

int16_t adc0, max100, max51, max52;

void loop(){
	while(1){
		prevMinuteTime = minuteTime;
		prevSecondTime = secondTime;
		prevMilliSecondTime = milliSecondTime;
		FramePerSec = 0;
		maxTemp100 = 0;
		maxTemp51 = 0;
		maxTemp52 = 0;

		while(1){
		    adc0 = ads.readADC_SingleEnded(PORT_ADS);
		    FramePerSec ++;
		    timeUpdate();

		    Serial.print(adc0);Serial.print("			");
			voltValForDot = adc0 * multiplierADS / 1000;
			tempValForDot = (voltValForDot - 1.25) / 0.005;
			dtostrf(voltValForDot, 7, 5, outstr);
			Serial.print(outstr);Serial.print("	  Volt");
			Serial.print("			");
			dtostrf(tempValForDot, 7, 1, outstr);
			Serial.print(outstr);Serial.println("	 TEMP");

		    if(prevSecondTime != secondTime){
		    	break;
		    }
		}
		Serial.print("												FPS");
		Serial.print("	");
		Serial.print(FramePerSec);
		Serial.print("			");
		
		Serial.print(adc0);Serial.print("			");
		voltValForDot = adc0 * multiplierADS / 1000;
		tempValForDot = (voltValForDot - 1.25) / 0.005;
		dtostrf(voltValForDot, 7, 5, outstr);
		Serial.print(outstr);Serial.print("	  Volt");
		Serial.print("			");
		dtostrf(tempValForDot, 7, 1, outstr);
		Serial.print(outstr);Serial.println("	 TEMP");
	}
}