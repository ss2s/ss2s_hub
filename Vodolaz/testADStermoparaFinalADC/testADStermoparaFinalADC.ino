#define M_OR_S 1    // если 0 то посекундно если 1 то поминутно

#define PORT_ADS 0  // порт ADS1115 куда подключается датчик, от 0 до 3
#include <Wire.h>
#include <Adafruit_ADS1015.h>

static char outstr[15];
int FramePerSec = 0;
float curTemp = 0;
float tempValForDot = 0;
float maxTemp100 = 0, maxTemp50 = 0;
int16_t adc0, maxADC100 = 0, maxADC50 = 0;
float voltValForDot = 0.0F;

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
float multiplierADS = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

// time value
unsigned long currentMil = 0;
unsigned long hourTime = 0;
unsigned long minuteTime = 0;
unsigned long minuteTime60 = 0;
unsigned long secondTime = 0;
unsigned long secondTime60 = 0;
unsigned long milliSecondTime = 0;

unsigned long prevMinuteTime = 0;
unsigned long prevSecondTime = 0;

inline void timeUpdate(){
	
	currentMil = millis();
	secondTime = currentMil / 1000UL;
	milliSecondTime = currentMil - (secondTime * 1000UL);
	minuteTime = secondTime / 60UL;
	hourTime = minuteTime / 60UL;
	secondTime60 = secondTime - (minuteTime * 60UL);
	minuteTime60 = minuteTime - (hourTime * 60UL);
}

void setup(){
	Serial.begin(250000);
	delay(10);
	#if M_OR_S == 0
		Serial.print("TIME S");
	#elif M_OR_S == 1
		Serial.print("TIME M");
	#endif
	Serial.print("	");
	Serial.print("100 FPS");
	Serial.print("	");
	Serial.print(" 50 FPS");
	Serial.print("	");
	Serial.print("ADC 100");
	Serial.print("	");
	#if M_OR_S == 0
		Serial.print("FPS");
	#elif M_OR_S == 1
		Serial.print("FPM");
	#endif
	Serial.println("");

	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  
	ads.begin();
}


void loop(){
	while(1){
		prevMinuteTime = minuteTime;
		prevSecondTime = secondTime;
		FramePerSec = 0;
		maxADC100 = 0;
		maxADC50 = 0;
		maxTemp100 = 0;
		maxTemp50 = 0;

		while(1){
		    adc0 = ads.readADC_SingleEnded(PORT_ADS);
		    FramePerSec ++;
		    timeUpdate();
		    if(maxADC100 < adc0){maxADC100 = adc0;}
		    if(maxADC50 < adc0 && FramePerSec % 2){maxADC50 = adc0;}

		    #if M_OR_S == 0
		    	if(prevSecondTime != secondTime){break;}
		    #elif M_OR_S == 1
		    	if(prevMinuteTime != minuteTime){break;}
		    #endif
		}

		#if M_OR_S == 0
			Serial.print(secondTime);
		#elif M_OR_S == 1
			Serial.print(minuteTime);
		#endif
		Serial.print("	");

		maxTemp100 = ((maxADC100 * multiplierADS / 1000) - 1.25) / 0.005;
		dtostrf(maxTemp100, 7, 1, outstr);
		Serial.print(outstr);
		Serial.print("	");

		maxTemp50 = ((maxADC50 * multiplierADS / 1000) - 1.25) / 0.005;
		dtostrf(maxTemp50, 7, 1, outstr);
		Serial.print(outstr);
		Serial.print("	");

		Serial.print(maxADC100);
		Serial.print("	");

		Serial.print(FramePerSec);
		Serial.println("");
	}
}