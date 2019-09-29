// controller: Atmega328P - (Arduino UNO), (Arduino NANO), (Arduino ProMini).
// программа разрабатывалась в Arduino IDE 1.8.1 для компиляции и прошивки используйте эту версию
// скачать можно здесь:  https://www.arduino.cc/en/Main/OldSoftwareReleases#previous
// необходимые библиотеки находятся в папке с проектом

#include "FastLED.h"      // WS2812B LED lib






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                           НАСТРОЙКИ ПРОГРАММЫ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:

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

#define SENSOR_HIGH 0     // сигнал с датчика при срабатывании: 1 = (5V); 0 = (0V);

#define LED_TYPE_CODE 2   // тип ленты:
// 0 = (ws2812 smartLED умные светодиоды 5-12v);
// 1 = (обычная LED лента 12V);
// 2 = (ws2812 smartLED умные светодиоды 5-12v) + (обычная LED лента 12V);

// если LED_TYPE_CODE = 0 или LED_TYPE_CODE = 2
#define NUM_LEDS 8              // количество умных светодиодов на одну цифру 150/1м
#define SMART_BRIGHTNESS 20     // яркость умных светодиодов (0 - 255)

// если LED_TYPE_CODE = 1 или LED_TYPE_CODE = 2
#define LED_STRIP_BRIGHTNESS 50    // яркость обычных светодиодов (0 - 255)
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





int arrow_state = 0;             // состояние стрелки

CRGB objledsx2[NUM_LEDS];
CRGB objledsx10[NUM_LEDS];

void blincWin(int16_t _x){

	delay(DELAY_BEFORE_ACTION);  // задержка перед действием

	if(LED_TYPE_CODE == 0){       // Smart LED

		if(_x == 0){
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
		}

		else if(_x == 2){
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}

		else if(_x == 10){
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
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
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			digitalWrite(LED_STRIP_X10_PIN, LOW);

			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
		}

		else if(_x == 2){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			digitalWrite(LED_STRIP_X2_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X2_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X2_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx2[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}

		else if(_x == 10){
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_1);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_1);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_2);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_2);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
			delay(HD_3);
			digitalWrite(LED_STRIP_X10_PIN, LOW);
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(0, 0, 0);}  // GRB black
			FastLED.show();
			delay(LD_3);
			if(LED_STRIP_BRIGHTNESS < 255){
				analogWrite(LED_STRIP_X10_PIN, LED_STRIP_BRIGHTNESS);
			}
			else{
			digitalWrite(LED_STRIP_X10_PIN, HIGH);
			}
			for(int i=0; i<NUM_LEDS; i++){objledsx10[i] = CRGB(S_GREEN, S_RED, S_BLUE);}  // GRB white
			FastLED.show();
		}
	}

	if(_x != 0){digitalWrite(EFFECT_LED_PIN, HIGH);}
	else{digitalWrite(EFFECT_LED_PIN, LOW);}
}

void setup(){

	pinMode(SENSOR_X2_PIN, INPUT_PULLUP);
	pinMode(SENSOR_X10_PIN, INPUT_PULLUP);

	pinMode(EFFECT_LED_PIN, OUTPUT);
	digitalWrite(EFFECT_LED_PIN, LOW);

	if(LED_TYPE_CODE == 1 || LED_TYPE_CODE == 2){
		pinMode(LED_STRIP_X2_PIN, OUTPUT);
		pinMode(LED_STRIP_X10_PIN, OUTPUT);
		digitalWrite(LED_STRIP_X2_PIN, LOW);
		digitalWrite(LED_STRIP_X10_PIN, LOW);
	}

	if(LED_TYPE_CODE == 0 || LED_TYPE_CODE == 2){
		FastLED.setBrightness(SMART_BRIGHTNESS);
		FastLED.addLeds<WS2811, SMART_LED_X2_PIN, RGB>(objledsx2, NUM_LEDS);
		FastLED.addLeds<WS2811, SMART_LED_X10_PIN, RGB>(objledsx10, NUM_LEDS);
		
		for(int i=0; i<NUM_LEDS; i++){
		    objledsx2[i] = CRGB(0, 0, 0);  // GRB black
		    objledsx10[i] = CRGB(0, 0, 0);  // GRB black
		}
		FastLED.show();
	}

	arrow_state = 0;
}

void loop(){

	// blincWin(2);
	// delay(3000);
	// blincWin(0);
	// delay(3000);

	// blincWin(10);
	// delay(3000);
	// blincWin(0);
	// delay(3000);

	if(arrow_state == 0){
		if(digitalRead(SENSOR_X2_PIN) == SENSOR_HIGH){
			arrow_state = 2;
			blincWin(2);
		}
		else if(digitalRead(SENSOR_X10_PIN) == SENSOR_HIGH){
			arrow_state = 10;
			blincWin(10);
		}
	}
}