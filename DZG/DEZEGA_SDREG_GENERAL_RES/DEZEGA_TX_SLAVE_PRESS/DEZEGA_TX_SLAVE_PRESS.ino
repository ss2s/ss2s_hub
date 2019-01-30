// ProMini
#include "HX711.h"

#define POMP_PIN 4  // пин подключения насоса

HX711 hx711Obj;

#define HX711_DOUT_PIN A1
#define HX711_PD_SCK_PIN A0
int adc64 = 0, minADC = 8388608, maxADC = 0;
float realPress, minPress, maxPress;

char s_d; // переменная для хранения считывания управляющего байта символа

// float Map
float flap(float fX, float fY = 0, float fZ = 1000, float fA = 0, float fB = 1000){
	return (fA*fX-fB*fX+fB*fY-fA*fZ)/(fY-fZ);
}

void setup(){
	Serial.begin(250000);
	delay(10);

	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN);
}

void loop(){
	while(1){
		// resive and convert termocouple values and find MAX val
		adc64 = hx711Obj.read();
		if(minADC > adc64){minADC = adc64;}
		if(maxADC < adc64){maxADC = adc64;}
		
		// pump control
		realPress = flap(adc64, 83886, 134217, -9.99, 9.99);
		if(realPress >= -0.02){
		    digitalWrite(POMP_PIN, LOW);
		}
		else if(realPress < -0.02){
		    digitalWrite(POMP_PIN, HIGH);
		}

		// transmite pressure to serial_3
		if (Serial.available() > 0){
		    s_d = Serial.read(); // управляющий байт (символ't') определяющий старт отправки температуры
		    if (s_d == 'p'){
		    	minPress = flap(minADC, 83886, 134217, -9.99, 9.99);
		    	maxPress = flap(minADC, 83886, 134217, -9.99, 9.99);

				// transmite pressure to serial_3
		    	Serial.print('i');
		    	Serial.println(minPress, 2);  // передача температуры через serial, 1 знак после запятой
		    	Serial.print('e');
		    	Serial.println(maxPress, 2);  // передача температуры через serial, 1 знак после запятой

		    	minADC = 8388608;
		    	maxADC = 0;
		    }
		}
	}
}