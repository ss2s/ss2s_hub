// ProMini

#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define PORT_ADS 0  // порт ADS1115 куда подключается датчик, от 0 до 3

char srData; // переменная для хранения считывания управляющего байта символа

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
float multiplierADS = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

// resive and convert termocouple values and find MAX val
// transmite temp to serial_1
void setup(){
	Serial.begin(250000);
	delay(10);

	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	ads.begin();
}

void loop(){
	while(1){
		if (Serial.available() > 0){
		    
		}
	}
}