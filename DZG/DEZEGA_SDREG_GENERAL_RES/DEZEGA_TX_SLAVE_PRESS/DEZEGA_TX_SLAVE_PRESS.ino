// ProMini
#include "HX711.h"

#define POMP_PIN 4  // пин подключения насоса

HX711 hx711Obj;
#define HX711_GAIN 64  // усилитель

#define HX711_DOUT_PIN A2
#define HX711_PD_SCK_PIN A3
long adc64 = 0;
float voltage = 0;
float realPress, minPress, maxPress;

char s_d; // переменная для хранения считывания управляющего байта символа

// float Map
float flap(float fX, float fY = 0, float fZ = 1000, float fA = 0, float fB = 1000){
	return (fA*fX-fB*fX+fB*fY-fA*fZ)/(fY-fZ);
}

void setup(){
	Serial.begin(250000);
	delay(10);

	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN, HX711_GAIN);
}

void loop(){
	while(1){
		// resive and convert Pressure values and find MAX val
		adc64 = hx711Obj.read();
		// adc64 = hx711Obj.get_value();
		
		voltage = flap(adc64, 0, 1073741826, 0, 5);
		realPress = flap(voltage, 0, 0.08, -9.99, 9.99);
		// pump control
		if(realPress >= -0.02){
		    digitalWrite(POMP_PIN, LOW);
		}
		else if(realPress < -0.02){
		    digitalWrite(POMP_PIN, HIGH);
		}

		if(minPress > realPress){minPress = realPress;}
		if(maxPress < realPress){maxPress = realPress;}

		// transmite pressure to serial_3
		if (Serial.available() > 0){
		    s_d = Serial.read(); // управляющий байт (символ't') определяющий старт отправки давления
		    if (s_d == 'p'){
				// transmite pressure to serial_3
		    	// Serial.println(adc64, 10);
		    	// Serial.println(voltage / 1000, 10);
		    	Serial.print('i');
		    	Serial.println(minPress, 2);  // передача давления через serial, 1 знак после запятой
		    	Serial.print('e');
		    	Serial.println(maxPress, 2);  // передача давления через serial, 1 знак после запятой
		   
		    	// Serial.print('a');
		    	hx711Obj.power_down();			        // put the ADC in sleep mode
  				delay(5);
  				hx711Obj.power_up();

		    	minPress = 10;
		    	maxPress = -10;
		    }
		}
	}
}