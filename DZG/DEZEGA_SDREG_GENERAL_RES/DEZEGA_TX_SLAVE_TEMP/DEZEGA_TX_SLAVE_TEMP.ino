// ProMini

#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define PORT_ADS 0  // порт ADS1115 куда подключается датчик, от 0 до 3

//static char outstr[15];  // масив для строки с точкой
int16_t adc0, maxADC = 0;
float maxTemp = 0;

char s_d; // переменная для хранения считывания управляющего байта символа

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
float multiplierADS = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

void setup(){
	Serial.begin(250000);
	delay(10);

	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	ads.begin();
	
	delay(950);
}

void loop(){
	while(1){
		// resive and convert termocouple values and find MAX val
		adc0 = ads.readADC_SingleEnded(PORT_ADS);
		if(maxADC < adc0){maxADC = adc0;}

		if (Serial.available() > 0){
		    s_d = Serial.read(); // управляющий байт (символ't') определяющий старт отправки температуры
		    if (s_d == 't'){
		    	maxTemp = ((maxADC * multiplierADS / 1000.0) - 1.25) / 0.005;

				// transmite temp to serial_1
				// maxTemp = int(maxTemp * 10);
		    	Serial.print('m');
		    	Serial.println(maxTemp, 1);  // передача температуры через serial, 1 знак после запятой
		    	// Serial.print('a');
				// dtostrf(maxTemp, 7, 1, outstr);
				// Serial.println(outstr);
		    	
		    	maxADC = 0;
		    }
		}
	}
}