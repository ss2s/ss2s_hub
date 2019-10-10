#include <EEPROM.h>          // библиотека для работы с EEPROM
#include "TM1637.h"          // библиотека для работы с TM1637
#include "iarduino_IR_TX.h"  // библиотека для работы с ИК-передатчиком,
                             // использует второй аппаратный таймер, НЕ ВЫВОДИТЕ СИГНАЛЫ ШИМ НА 3 ИЛИ 11 ВЫВОД!





// PINOUT:
//-
#define IR_TRANSMITTER_PIN 10  // ИК передатчик 10 вывод

#define LEFT_BUTTON_PIN 8     // ЛЕВАЯ КНОПКА "-"
#define RIGHT_BUTTON_PIN 9    // ПРАВАЯ КНОПКА "+"

#define DISPLAY_CLK_PIN 5     // дисплей CLK
#define DISPLAY_DIO_PIN 4     // дисплей DIO
//-

// SETTING:
//-
#define MIN_EXH_NUM_VAL 1       // МИНИМАЛЬНЫЙ НОМЕР ЕКСПОНАТА
#define MAX_EXH_NUM_VAL 9999    // МАКСИМАЛЬНЫЙ НОМЕР ЕКСПОНАТА

#define LOOP_DELAY 300          // ЗАДЕРЖКА МЕЖДУ ОТПРАВКАМИ НОМЕРА МАЯЧКА В ЦИКЛЕ
#define RANDOM_LOOP_DELAY 30    // + РАНДОМНАЯ ЗАДЕРЖКА МЕЖДУ ОТПРАВКАМИ НОМЕРА МАЯЧКА В ЦИКЛЕ
#define BUTTON_DELAY 200        // ЗАДЕРЖКА ПОСЛЕ НАЖАТИЯ КНОПКИ (СКОРОСТЬ ПРИРОСТА)
#define STRT_MENU_DELAY 2000    // ЗАДЕРЖКА ДЛЯ ВХОДА В МЕНЮ
#define IN_MENU_DELAY 2000      // ЗАДЕРЖКА НАХОЖДЕНИЯ ВНУТРИ МЕНЮ

#define DISPLAY_BRIGHTNES 3     // ЯРКОСТЬ ДИСПЛЕЯ, MINIMIM 0 - MAXIMUM 7
//-





// var
//-
int16_t exhibitNumber = 15;     // номер експоната
int16_t prevExhibitNumber = 0;  // номер експоната
int16_t eeAddress = 0;          // адресс EEPROM где хранится значение номера маячка
//-

// init
//-
TM1637 disp(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);  // объект disp (дисплей)
iarduino_IR_TX IR_DIODE(IR_TRANSMITTER_PIN);  // объект IR_DIODE, с указанием вывода к которому подключён ИК-передатчик
//-
void serReceiverExhibitNumF()
{
	if(Serial.available() > 0){
		int16_t tempSerVal = Serial.parseInt();
		if(tempSerVal >= MIN_EXH_NUM_VAL && tempSerVal <= MAX_EXH_NUM_VAL){
	    	exhibitNumber = tempSerVal;
	    	Serial.print("new exhibit number = ");
	    	Serial.println(exhibitNumber);
	    	EEPROM.put(eeAddress, exhibitNumber);  // запись в EEPROM
		}
		else{
			Serial.print("overflow! val = ");
	    	Serial.print(tempSerVal);
	    	Serial.print(";   exhibit number = ");
	    	Serial.println(exhibitNumber);
		}

		while(Serial.available() > 0){
		    Serial.read();
		}
	}
}

void menuButton()
{
	if(!digitalRead(LEFT_BUTTON_PIN) || !digitalRead(RIGHT_BUTTON_PIN)){
		delay(STRT_MENU_DELAY);
		if(!digitalRead(LEFT_BUTTON_PIN) || !digitalRead(RIGHT_BUTTON_PIN)){
			uint32_t strtMillForMenu = millis();
			while(millis() - strtMillForMenu < IN_MENU_DELAY){
				if(plusMinusButtonCheck() == 1){
					strtMillForMenu = millis();
				}
			}

		}
	}

	disp.point(0);
	disp.clearDisplay();
	disp.displayInt(exhibitNumber);

	EEPROM.put(eeAddress, exhibitNumber);  // запись в EEPROM
}

bool pmbcState = 0;
bool plusMinusButtonCheck()
{
	if(!digitalRead(LEFT_BUTTON_PIN)){
		exhibitNumber --;
		if(exhibitNumber < MIN_EXH_NUM_VAL){exhibitNumber = MIN_EXH_NUM_VAL;}
    	disp.point(1);
		disp.clearDisplay();
    	disp.displayInt(exhibitNumber);
		delay(BUTTON_DELAY);
		pmbcState = 1;
	}
	else if(!digitalRead(RIGHT_BUTTON_PIN)){
		exhibitNumber ++;
		if(exhibitNumber > MAX_EXH_NUM_VAL){exhibitNumber = MAX_EXH_NUM_VAL;}
    	disp.point(1);
		disp.clearDisplay();
    	disp.displayInt(exhibitNumber);
		delay(BUTTON_DELAY);
		pmbcState = 1;
	}
	else{pmbcState = 0;}

	disp.displayInt(exhibitNumber);
	return pmbcState;
}

void setup()
{
	pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

	Serial.begin(115200);

	randomSeed(analogRead(A0));

	EEPROM.get(eeAddress, exhibitNumber);

	disp.init();  // disp init
	disp.set(DISPLAY_BRIGHTNES);  // яркость дисплея, minimim 0 - maximum 7

	IR_DIODE.begin();             // ИК-передатчиком init
	IR_DIODE.protocol("AeQQV~zK]Kp^KJp[@@@@@@@Bp");      // протокол передачи данных от пульта ELENBERG

	// вывод названия или бренда, 
	// список допустимых символов в файле \TM1637\TM1637.h

	// byte strt_banner[] = {_i, _r, _empty, _t, _e, _c, _h};  
	// disp.runningString(strt_banner, sizeof(strt_banner), 300);
	disp.clearDisplay();
}

void loop()
{
    menuButton();

	IR_DIODE.send(exhibitNumber);  // отправляем код експоната через ИК-передатчик

	serReceiverExhibitNumF();

	if(exhibitNumber != prevExhibitNumber){
    	disp.clearDisplay();
    	disp.displayInt(exhibitNumber);
    	prevExhibitNumber = exhibitNumber;
	}

	delay(LOOP_DELAY + random(0, RANDOM_LOOP_DELAY));               // задержка между отправками в мс + Random
	// delay(LOOP_DELAY);               // задержка между отправками в мс
}