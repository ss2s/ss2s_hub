// ProMini
#include "HX711.h"

#define PUMP_PIN 4  // пин подключения насоса

#define HX711_GAIN 32  // усилитель 32 chanel B

HX711 hx711Obj;

#define HX711_DOUT_PIN A2
#define HX711_PD_SCK_PIN A3

// 5242880 = 0.05V = -9.99 kPa
// 8388608 = 0.08V = 9.99 kPa

long adc64 = 0, minADC64 = 8388608, maxADC64 = -8388608;

long pumpThresholdAdcVal = 6807871;  // 0.0649249249V = -0.05 kPa

char s_d = 'x'; // переменная для хранения считывания управляющего байта символа



void setup(){
	Serial.begin(250000);

	delay(450);

	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN, HX711_GAIN);
	delay(50);
	hx711Obj.set_gain(HX711_GAIN);

	delay(500);
}



void loop(){
	while(1){
		// read ADC Pressure values and find MAX val
		adc64 = hx711Obj.read();

		// pump control
		if(adc64 >= pumpThresholdAdcVal){
		    digitalWrite(PUMP_PIN, LOW);
		}
		else if(adc64 < pumpThresholdAdcVal){
		    digitalWrite(PUMP_PIN, HIGH);
		}

		if(minADC64 > adc64){minADC64 = adc64;}
		if(maxADC64 < adc64){maxADC64 = adc64;}

		// transmite ADC pressure to serial_3
		if (Serial.available() > 0){
		    s_d = Serial.read(); // управляющий байт (символ'p' || 'f') определяющий старт отправки давления. 'n' калибровка мотора. 'r' restart ADC
		    if (s_d == 'p' || s_d == 'f'){

		    	// 	hx711Obj.power_down();			        // put the ADC in sleep mode

				// transmite ADC pressure to serial_2
		    	Serial.print('i');
		    	Serial.println(minADC64);  // передача давления через serial, 1 знак после запятой
		    	Serial.print('e');
		    	Serial.println(maxADC64);  // передача давления через serial, 1 знак после запятой

				if(s_d == 'f'){
					pumpThresholdAdcVal = Serial.parseInt();
					Serial.print('n');
		    		Serial.println(pumpThresholdAdcVal);
				}

		    	minADC64 = 8388608;
		    	maxADC64 = -8388608;

  				// delay(5);
  				// hx711Obj.power_up();
		    }
		    else if(s_d == 'n'){
		    	pumpThresholdAdcVal = Serial.parseInt();
		    	Serial.print('n');
		    	Serial.println(pumpThresholdAdcVal);
		    }
		    else if(s_d == 'r'){
		    	hx711Obj.power_down();
		    	delay(500);
		    	hx711Obj.power_up();
		    }
		}
	}
}