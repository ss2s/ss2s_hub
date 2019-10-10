// controller: Atmega328P - (Arduino UNO), (Arduino NANO), (Arduino ProMini). V4
// программа разрабатывалась в Arduino IDE 1.8.1 для компиляции и прошивки используйте эту версию
// скачать можно здесь:  https://www.arduino.cc/en/Main/OldSoftwareReleases#previous
// необходимые библиотеки находятся в папке с проектом

#include "FastLED.h"      // WS2812B LED lib






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           НАСТРОЙКИ ПРОГРАММЫ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:

#define LED_DRIVER WS2812B  // драйвер светодиодов, здесь выберите драйвер умных диодов. Ниже приведены варианты.
// TM1803, TM1804, TM1809, WS2811, WS2812, WS2812B, NEOPIXEL, APA104, UCS1903, UCS1903B, GW6205, GW6205_400, SK6812

#define DELAY_BEFORE_ACTION 2000  // задержка перед началом мигания

// в этом блоке настраиваится алгоритм мигания  HD_x - столько включен; LD_x - столько выключен;
#define HD_1 110
#define LD_1 106
#define HD_2 114
#define LD_2 85
#define HD_3 115
#define LD_3 51

// smart LED color 0 - 255 цвет умных светодиодов
#define S_RED 255
#define S_GREEN 255
#define S_BLUE 255

#define SENSOR_X2_HIGH 0     // сигнал с датчика при срабатывании: 1 = (5V); 0 = (0V);
#define SENSOR_X10_HIGH 0    // сигнал с датчика при срабатывании: 1 = (5V); 0 = (0V);

#define ENABLE_X2_PULLUP 1   // разрешить внутреннюю подтяжку если управляющий сигнал GND. 1 - разрешить; 0 - запретить;
#define ENABLE_X10_PULLUP 1  // разрешить внутреннюю подтяжку если управляющий сигнал GND. 1 - разрешить; 0 - запретить;

#define LED_TYPE_CODE 2   // тип ленты:
// 0 = (ws2812 smartLED умные светодиоды 5-12v);
// 1 = (обычная LED лента 12V);
// 2 = (ws2812 smartLED умные светодиоды 5-12v) + (обычная LED лента 12V);

// если LED_TYPE_CODE = 0 или LED_TYPE_CODE = 2
#define NUM_LEDS_X2 366           // количество умных светодиодов на одну цифру X2 150/1м
#define NUM_LEDS_X10 161          // количество умных светодиодов на одну цифру X10 150/1м

#define SMART_BRIGHTNESS 20     // яркость умных светодиодов (0 - 255)

// если LED_TYPE_CODE = 1 или LED_TYPE_CODE = 2
#define LED_STRIP_BRIGHTNESS 50    // яркость обычных светодиодов (0 - 255)

bool effect_high = 1;      // тип управляющего сигнала реле эффектов: 1 = (5V); 0 = (0V);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА:

// сенсоры
#define SENSOR_X2_PIN 2          // пин подключения сенсора X2
#define SENSOR_X10_PIN 3         // пин подключения сенсора X10

// умные светодиоды WS2812 5-12V
#define SMART_LED_X2_PIN 4       // пин подключения умных светодиодов X2
#define SMART_LED_X10_PIN 5      // пин подключения умных светодиодов X10

// модуль светодиодов с эффектами
#define EFFECT_LED_PIN 8             // пин подключения модуля светодиодов с эффектами

// обычные светодиоды 12V
#define LED_STRIP_X2_PIN 9      // пин подключения обычных светодиодов X2
#define LED_STRIP_X10_PIN 10     // пин подключения обычных светодиодов X10
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           КОНЕЦ ВСЕХ НАСТРОЕК
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define TIMER_PERIOD 1000

int arrow_state = 0;             // состояние стрелки

uint32_t timer_start_time_run = 0;

CRGB objledsx2[NUM_LEDS_X2];
CRGB objledsx10[NUM_LEDS_X10];

void blincWin(int16_t _x){

	if(_x != 0){delay(DELAY_BEFORE_ACTION);}  // задержка перед действием
	

	if(LED_TYPE_CODE == 0){       // Smart LED

		if(_x == 0){
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}    // GRB black
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
		}

		else if(_x == 2){
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}

		else if(_x == 10){
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}
	}

	else if(LED_TYPE_CODE == 1){  // Simple LED

		if(_x == 0){
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
		}

		else if(_x == 2){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			delay(HD_1);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			delay(HD_2);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			delay(HD_3);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
		}

		else if(_x == 10){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			delay(HD_1);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			delay(HD_2);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			delay(HD_3);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
		}
	}

	else if(LED_TYPE_CODE == 2){                         // combo mode

		if(_x == 0){
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}    // GRB black
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();

			digitalWrite(LED_STRIP_X2_PIN, LOW);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
		}

		else if(_x == 2){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X2; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}

		else if(_x == 10){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS_X10; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}
	}

	if(_x != 0){digitalWrite(EFFECT_LED_PIN, effect_high);}
	else{digitalWrite(EFFECT_LED_PIN, !effect_high);}
}

void setup(){

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	if(SENSOR_X2_HIGH == 0 && ENABLE_X2_PULLUP == 1){
	    pinMode(SENSOR_X2_PIN, INPUT_PULLUP);
	}
	else{
		pinMode(SENSOR_X2_PIN, INPUT);
	}

	if(SENSOR_X10_HIGH == 0 && ENABLE_X10_PULLUP == 1){
		pinMode(SENSOR_X10_PIN, INPUT_PULLUP);
	}
	else{
		pinMode(SENSOR_X10_PIN, INPUT);
	}

	pinMode(EFFECT_LED_PIN, OUTPUT);

	if(LED_TYPE_CODE == 1 || LED_TYPE_CODE == 2){
		pinMode(LED_STRIP_X2_PIN, OUTPUT);
		pinMode(LED_STRIP_X10_PIN, OUTPUT);
	}

	if(LED_TYPE_CODE == 0 || LED_TYPE_CODE == 2){
		FastLED.setBrightness(SMART_BRIGHTNESS);
		FastLED.addLeds<LED_DRIVER, SMART_LED_X2_PIN, RGB>(objledsx2, NUM_LEDS_X2);
		FastLED.addLeds<LED_DRIVER, SMART_LED_X10_PIN, RGB>(objledsx10, NUM_LEDS_X10);
	}
	blincWin(0);

	arrow_state = 0;

	digitalWrite(13, LOW);
	timer_start_time_run = millis();
}

void loop(){

	if(arrow_state == 0){
		if(digitalRead(SENSOR_X2_PIN) == SENSOR_X2_HIGH){
			arrow_state = 2;
			blincWin(2);
		}
		else if(digitalRead(SENSOR_X10_PIN) == SENSOR_X10_HIGH){
			arrow_state = 10;
			blincWin(10);
		}

		if(millis() - timer_start_time_run >= TIMER_PERIOD){
			timer_start_time_run = millis();
			digitalWrite(13, !digitalRead(13));
		}
	}
	else{
		if((millis() - timer_start_time_run) >= (TIMER_PERIOD / 10)){
			timer_start_time_run = millis();
			digitalWrite(13, !digitalRead(13));
		}
	}
}
