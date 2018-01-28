////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /******************************************************************************************************\
          MEGAZVON24  Arduino MEGA2560 программа для автоматического перезвона колоколов по заданному времени.
          https://github.com/ss2s/MEGAZVON24_MEGA2560

          created by : ss2s
          author of idea : Aleksandr K
        \******************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                                            // ВАЖНАЯ ИНФОРМАЦИЯ //

// ВСЕ НАСТРОЙКИ ВЫНЕСЕНЫ В ОТДЕЛЬНЫЙ ФАЙЛ "MEGAZVON24_CONFIG.h"
// НАСТРОЙКА ДЛИТЕЛЬНОСТИ КАЖДОЙ НОТЫ В ФАЙЛЕ "MEGAZVON24_CONFIG.h"
// НАСТРОЙКИ МЕЛОДИЙ В ФАЙЛЕ "melody24.h"

// НАСТРОЙКИ ТЕРМОРЕГУЛЯТОРА В ОСНОВНОЙ ПРОГРАММЕ

// все нужные файлы находятся в папке с прошивкой

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Arduino.h"            // Arduino lib
#include <Wire.h>               // библиотека I2C
#include "DS3231.h"             // библиотека часов
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "config24.h"           // файл конфигурации
#include <LiquidCrystal.h>      // библиотека дисплея с кнопками
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ЧАСЫ DS3231:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// инициализация дисплея с кнопками настройки в файле конфигурации
LiquidCrystal lcd(DEF_LCD_INIT_PIN);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <EEPROM.h>
unsigned long EEPROM_ulong_read(int addr);
void EEPROM_ulong_write(int addr, unsigned long data);
void timeOfBlowUnicNotaSet(int _kolokol);
unsigned long timeOfBlowUnicNotaGet(int _kolokol);
void timeOfBlowUnicNotaSetPak();
void timeOfBlowUnicNotaGetPak();
void chekPerezvon();
void bellForMenu();
void timtOfBlowUnicNotaForMenu();
void spisokMelodiyForMenu();
void timeSetForMenu();
void menu24();

#include "nota24.h"             // ноты
#include "melody24.h"           // файл с мелодиями
#include "raspisanie24.h"       // расписание
#include "raspisanie24ext.h"    // расписание

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// пользовательские символы:
byte customCharBell0[8] = {  // колокольчик для дисплея
	0b00000,
	0b00100,
	0b01110,
	0b01110,
	0b11111,
	0b11111,
	0b00100,
	0b00000
};

byte customCharNota1[8] = {  // нота для дисплея
	0b01110,
	0b01111,
	0b01001,
	0b01001,
	0b11001,
	0b11011,
	0b00011,
	0b00000
};

byte customCharPR2[8] = {  // значек праздника для дисплея
	0b00100,
	0b01110,
	0b00100,
	0b01110,
	0b11111,
	0b11111,
	0b11111,
	0b01110
};

byte customCharRightArrow3[8] = {  // стрелка вправо для дисплея
	0b00000,
	0b00000,
	0b00100,
	0b00010,
	0b11111,
	0b00010,
	0b00100,
	0b00000
};

byte customCharMenuArrow4[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100
};

byte customCharChasi5[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10111,
	0b10001,
	0b01110,
	0b00000
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ТЕХНИЧЕСКИЕ ФУНКЦИИ:

// терморегулятор
#if DEF_TEMP_ENABLE == 1
void termoregulator(){
	g3231t = get3231Temp();
	if(g3231t < DEF_TEMP_ON){
		digitalWrite(DEF_TEMP_RELE_PIN, HHH);
	}else if(g3231t > DEF_TEMP_OFF){
		digitalWrite(DEF_TEMP_RELE_PIN, LLL);
	}
}
#endif

// функция проверки кнопок
byte key(){  // 1-723, 2-482, 3-133, 4-310, 5-0;
	int val = analogRead(A0);
	byte keyVal = 0;
	if(val < 67) keyVal = 5;             // right
	else if(val < 221) keyVal = 4;       // up
	else if(val < 396) keyVal = 3;       // down
	else if(val < 602) keyVal = 2;       // left
	else if(val < 873) keyVal = 1;       // select // 873
	else if(val <= 1023) keyVal = 0;     // no press
	// Serial.print(keyVal);
	// Serial.print(" ");
	// Serial.println(val);
	return keyVal;
}

void buttonChekForLoop(){  // обработчик нажатий кнопок в цикле loop
	
	byte val = key();
	unsigned int buttonDelay = 200;  // задержка для меню

	if(val > 0){
		delay(buttonDelay);
	}

	if(val == 1){  // select
		menu24();
	}else if(val == 2){  // left
		rminute = 61;
		timeToDisplay();
	}else if(val == 3){  // down
		flagManualPR = 1;
		prazdnik = 0;
		rminute = 61;
		timeToDisplay();
	}else if(val == 4){  // up
		flagManualPR = 1;
		prazdnik = 1;
		rminute = 61;
		timeToDisplay();
	}else if(val == 5){  // right
		rminute = 61;
		timeToDisplay();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long EEPROM_ulong_read(int fAddr) // чтение из EEPROM 4 байта unsigned long
{ 
	int rAddr = fAddr*4;
	byte raw[4];
  for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(rAddr+i);
  unsigned long &data = (unsigned long&)raw;
  return data;
}
//*****************************************************************
void EEPROM_ulong_write(int fAddr, unsigned long data) // запись в EEPROM 4 байта unsigned long
{
	int rAddr = fAddr*4;
  	byte raw[4];
  	(unsigned long&)raw = data;
  	for(byte i = 0; i < 4; i++) EEPROM.write(rAddr+i, raw[i]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timeOfBlowUnicNotaSet(int _kolokol){
	int tobKolokol = _kolokol;
	unsigned long tobTimtOfBlowUnicNota = findNotaDelayForKolokolNumber(tobKolokol);
	unsigned long readTobTimtOfBlowUnicNota = EEPROM_ulong_read(tobKolokol);
	if(readTobTimtOfBlowUnicNota != tobTimtOfBlowUnicNota){
		EEPROM_ulong_write(tobKolokol, tobTimtOfBlowUnicNota);
	}
}

unsigned long timeOfBlowUnicNotaGet(int _kolokol){
	int tobKolokol = _kolokol;
	unsigned long tobTimtOfBlowUnicNota = EEPROM_ulong_read(tobKolokol);

	switch (tobKolokol) {
	    case 1:
	      timtOfBlowUnicNota_1 = tobTimtOfBlowUnicNota;
	      break;
	    case 2:
	      timtOfBlowUnicNota_2 = tobTimtOfBlowUnicNota;
	      break;
	    case 3:
	      timtOfBlowUnicNota_3 = tobTimtOfBlowUnicNota;
	      break;
	    case 4:
	      timtOfBlowUnicNota_4 = tobTimtOfBlowUnicNota;
	      break;
	    case 5:
	      timtOfBlowUnicNota_5 = tobTimtOfBlowUnicNota;
	      break;
	    case 6:
	      timtOfBlowUnicNota_6 = tobTimtOfBlowUnicNota;
	      break;
	    case 7:
	      timtOfBlowUnicNota_7 = tobTimtOfBlowUnicNota;
	      break;
	    case 8:
	      timtOfBlowUnicNota_8 = tobTimtOfBlowUnicNota;
	      break;
	    case 9:
	      timtOfBlowUnicNota_9 = tobTimtOfBlowUnicNota;
	      break;
	    case 10:
	      timtOfBlowUnicNota_10 = tobTimtOfBlowUnicNota;
	      break;
	    case 11:
	      timtOfBlowUnicNota_11 = tobTimtOfBlowUnicNota;
	      break;
	    case 12:
	      timtOfBlowUnicNota_12 = tobTimtOfBlowUnicNota;
	      break;
	    case 13:
	      timtOfBlowUnicNota_13 = tobTimtOfBlowUnicNota;
	      break;
	    case 14:
	      timtOfBlowUnicNota_14 = tobTimtOfBlowUnicNota;
	      break;
	    case 15:
	      timtOfBlowUnicNota_15 = tobTimtOfBlowUnicNota;
	      break;
	    case 16:
	      timtOfBlowUnicNota_16 = tobTimtOfBlowUnicNota;
	      break;
	    case 17:
	      timtOfBlowUnicNota_17 = tobTimtOfBlowUnicNota;
	      break;
	    case 18:
	      timtOfBlowUnicNota_18 = tobTimtOfBlowUnicNota;
	      break;
	    case 19:
	      timtOfBlowUnicNota_19 = tobTimtOfBlowUnicNota;
	      break;
	    case 20:
	      timtOfBlowUnicNota_20 = tobTimtOfBlowUnicNota;
	      break;
	    case 21:
	      timtOfBlowUnicNota_21 = tobTimtOfBlowUnicNota;
	      break;
	    case 22:
	      timtOfBlowUnicNota_22 = tobTimtOfBlowUnicNota;
	      break;
	    case 23:
	      timtOfBlowUnicNota_23 = tobTimtOfBlowUnicNota;
	      break;
	    case 24:
	      timtOfBlowUnicNota_24 = tobTimtOfBlowUnicNota;
	      break;
	}

	return tobTimtOfBlowUnicNota;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timeOfBlowUnicNotaSetPak(){

	EEPROM_ulong_write(1, DEF_TIME_OF_BLOW_UNIC_NOTA_1);
	EEPROM_ulong_write(2, DEF_TIME_OF_BLOW_UNIC_NOTA_2);
	EEPROM_ulong_write(3, DEF_TIME_OF_BLOW_UNIC_NOTA_3);
	EEPROM_ulong_write(4, DEF_TIME_OF_BLOW_UNIC_NOTA_4);
	EEPROM_ulong_write(5, DEF_TIME_OF_BLOW_UNIC_NOTA_5);
	EEPROM_ulong_write(6, DEF_TIME_OF_BLOW_UNIC_NOTA_6);
	EEPROM_ulong_write(7, DEF_TIME_OF_BLOW_UNIC_NOTA_7);
	EEPROM_ulong_write(8, DEF_TIME_OF_BLOW_UNIC_NOTA_8);
	EEPROM_ulong_write(9, DEF_TIME_OF_BLOW_UNIC_NOTA_9);
	EEPROM_ulong_write(10, DEF_TIME_OF_BLOW_UNIC_NOTA_10);
	EEPROM_ulong_write(11, DEF_TIME_OF_BLOW_UNIC_NOTA_11);
	EEPROM_ulong_write(12, DEF_TIME_OF_BLOW_UNIC_NOTA_12);
	EEPROM_ulong_write(13, DEF_TIME_OF_BLOW_UNIC_NOTA_13);
	EEPROM_ulong_write(14, DEF_TIME_OF_BLOW_UNIC_NOTA_14);
	EEPROM_ulong_write(15, DEF_TIME_OF_BLOW_UNIC_NOTA_15);
	EEPROM_ulong_write(16, DEF_TIME_OF_BLOW_UNIC_NOTA_16);
	EEPROM_ulong_write(17, DEF_TIME_OF_BLOW_UNIC_NOTA_17);
	EEPROM_ulong_write(18, DEF_TIME_OF_BLOW_UNIC_NOTA_18);
	EEPROM_ulong_write(19, DEF_TIME_OF_BLOW_UNIC_NOTA_19);
	EEPROM_ulong_write(20, DEF_TIME_OF_BLOW_UNIC_NOTA_20);
	EEPROM_ulong_write(21, DEF_TIME_OF_BLOW_UNIC_NOTA_21);
	EEPROM_ulong_write(22, DEF_TIME_OF_BLOW_UNIC_NOTA_22);
	EEPROM_ulong_write(23, DEF_TIME_OF_BLOW_UNIC_NOTA_23);
	EEPROM_ulong_write(24, DEF_TIME_OF_BLOW_UNIC_NOTA_24);
	
}
void timeOfBlowUnicNotaGetPak(){

	timtOfBlowUnicNota_1 = EEPROM_ulong_read(1);
	timtOfBlowUnicNota_2 = EEPROM_ulong_read(2);
	timtOfBlowUnicNota_3 = EEPROM_ulong_read(3);
	timtOfBlowUnicNota_4 = EEPROM_ulong_read(4);
	timtOfBlowUnicNota_5 = EEPROM_ulong_read(5);
	timtOfBlowUnicNota_6 = EEPROM_ulong_read(6);
	timtOfBlowUnicNota_7 = EEPROM_ulong_read(7);
	timtOfBlowUnicNota_8 = EEPROM_ulong_read(8);
	timtOfBlowUnicNota_9 = EEPROM_ulong_read(9);
	timtOfBlowUnicNota_10 = EEPROM_ulong_read(10);
	timtOfBlowUnicNota_11 = EEPROM_ulong_read(11);
	timtOfBlowUnicNota_12 = EEPROM_ulong_read(12);
	timtOfBlowUnicNota_13 = EEPROM_ulong_read(13);
	timtOfBlowUnicNota_14 = EEPROM_ulong_read(14);
	timtOfBlowUnicNota_15 = EEPROM_ulong_read(15);
	timtOfBlowUnicNota_16 = EEPROM_ulong_read(16);
	timtOfBlowUnicNota_17 = EEPROM_ulong_read(17);
	timtOfBlowUnicNota_18 = EEPROM_ulong_read(18);
	timtOfBlowUnicNota_19 = EEPROM_ulong_read(19);
	timtOfBlowUnicNota_20 = EEPROM_ulong_read(20);
	timtOfBlowUnicNota_21 = EEPROM_ulong_read(21);
	timtOfBlowUnicNota_22 = EEPROM_ulong_read(22);
	timtOfBlowUnicNota_23 = EEPROM_ulong_read(23);
	timtOfBlowUnicNota_24 = EEPROM_ulong_read(24);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\\
// \\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// КОМПОНЕНТЫ МЕНЮ:

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// функция рисования для функции удара в любой колокол с настройками из меню
inline void drawBellForMenu(int _bfmKolocol = 12, unsigned long _bfmTimeOfBlow = DEF_TIME_OF_BLOW_TO_THE_BELL){

	int dbfmKolocol = _bfmKolocol;
	unsigned long dbfmTimeOfBlow = _bfmTimeOfBlow;
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(0));
	lcd.setCursor(2,0);
	lcd.print(dbfmKolocol);
	lcd.setCursor(4,0);
	lcd.print(" del ");
	lcd.setCursor(9,0);
	lcd.print(dbfmTimeOfBlow);
	lcd.setCursor(14,0);
	lcd.print("OK");
}

// функция удара в любой колокол с настройками из меню
void bellForMenu(){

	unsigned int buttonDelay = 200;  // задержка для меню

	int bfmKolocol = 12;  // переменная для хранения номера колокола
	unsigned long bfmTimeOfBlow = DEF_TIME_OF_BLOW_TO_THE_BELL;  // переменная для хранения выдержки язычка
	bool bfmCikl = 1;  // переменная для управления циклом while
	byte bfmVirtualPos = 1;   // виртуальная позиция указателя меню
	byte bfmRealPos = 0;   // реальная позиция указателя меню
	byte bfmKey = 0;   // значение кнопок для обработки в цикле while
	unsigned long bfmTimeOfBlowMnojitel = 1;  // множитель для мс. в меню

	drawBellForMenu();

	lcd.setCursor(0,1);
	lcd.write(byte(4));

	while(bfmCikl){
		bfmKey = key();


		if(bfmKey > 0){
			if(bfmKey == 1){  // s
				if(bfmVirtualPos == 1){
					bfmCikl = 0;  // выход из меню удара в колокол
				}else if(bfmVirtualPos == 2){
					nota(bfmKolocol, bfmTimeOfBlow, DEF_DEL_UNT_NEXT_NOTA);  // играть выбранную ноту
				}else if(bfmVirtualPos == 3){
					bfmTimeOfBlowMnojitel *= 10;
					if(bfmTimeOfBlowMnojitel > 100){bfmTimeOfBlowMnojitel = 1;}
				}else if(bfmVirtualPos == 4){
					nota(bfmKolocol, bfmTimeOfBlow, DEF_DEL_UNT_NEXT_NOTA);  // играть выбранную ноту
				}
				
				delay(buttonDelay);

			}else if(bfmKey == 2){  // l
				bfmVirtualPos --;
				if(bfmVirtualPos < 1){bfmVirtualPos = 4;}
				delay(buttonDelay);
			}else if(bfmKey == 3){  // d
				if(bfmVirtualPos == 1){
					bfmCikl = 0;  // выход из меню удара в колокол
				}else if(bfmVirtualPos == 2){
					bfmKolocol --;
					if(bfmKolocol <= 0){bfmKolocol = 24;}
				}else if(bfmVirtualPos == 3){
					bfmTimeOfBlow -= bfmTimeOfBlowMnojitel;
					if(bfmTimeOfBlow <= 0){bfmTimeOfBlow = 9999;}
				}else if(bfmVirtualPos == 4){
					nota(bfmKolocol, bfmTimeOfBlow, DEF_DEL_UNT_NEXT_NOTA);  // играть выбранную ноту
				}
				delay(buttonDelay);
			}else if(bfmKey == 4){  // u
				if(bfmVirtualPos == 1){
					delay(buttonDelay);
					menu24();
				}else if(bfmVirtualPos == 2){
					bfmKolocol ++;
					if(bfmKolocol > 24){bfmKolocol = 1;}
				}else if(bfmVirtualPos == 3){
					bfmTimeOfBlow += bfmTimeOfBlowMnojitel;
					if(bfmTimeOfBlow > 9999){bfmTimeOfBlow = 1;}
				}else if(bfmVirtualPos == 4){
					nota(bfmKolocol, bfmTimeOfBlow, DEF_DEL_UNT_NEXT_NOTA);  // играть выбранную ноту
				}
				delay(buttonDelay);
			}else if(bfmKey == 5){  // r
				bfmVirtualPos ++;
				if(bfmVirtualPos > 4){bfmVirtualPos = 1;}
				delay(buttonDelay);
			}

			switch (bfmVirtualPos){
			    case 1:
			      bfmRealPos = 0;
			      break;
			    case 2:
			      bfmRealPos = 2;
			      break;
			    case 3:
			      bfmRealPos = 9;
			      break;
			    case 4:
			      bfmRealPos = 14;
			      break;
			}

			drawBellForMenu(bfmKolocol, bfmTimeOfBlow);
			lcd.setCursor(bfmRealPos,1);
			lcd.write(byte(4));
			if(bfmVirtualPos == 3){
				lcd.print(bfmTimeOfBlowMnojitel);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void drawTimtOfBlowUnicNotaForMenu(int _tobKolocol = 12){

	int dtobKolocol = _tobKolocol;
	unsigned long dtobTimeOfBlow = findNotaDelayForKolokolNumber(dtobKolocol);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(5));
	lcd.write(byte(0));
	lcd.setCursor(2,0);
	lcd.print(dtobKolocol);
	lcd.setCursor(4,0);
	lcd.print(" set ");
	lcd.setCursor(9,0);
	lcd.print(dtobTimeOfBlow);
	lcd.setCursor(14,0);
	lcd.print("OK");
}
// функция настройки длительности нот
void timtOfBlowUnicNotaForMenu(){
	unsigned int buttonDelay = 200;  // задержка для меню

	int tobKolocol = 12;  // переменная для хранения номера колокола
	unsigned long tobTimeOfBlow = DEF_TIME_OF_BLOW_TO_THE_BELL;  // переменная для хранения выдержки язычка
	bool tobCikl = 1;  // переменная для управления циклом while
	byte tobVirtualPos = 1;   // виртуальная позиция указателя меню
	byte tobRealPos = 0;   // реальная позиция указателя меню
	byte tobKey = 0;   // значение кнопок для обработки в цикле while
	unsigned long tobTimeOfBlowMnojitel = 1;  // множитель для мс. в меню

	drawTimtOfBlowUnicNotaForMenu();

	lcd.setCursor(0,1);
	lcd.write(byte(4));

	while(tobCikl){
		tobKey = key();


		if(tobKey > 0){
			if(tobKey == 1){  // s
				if(tobVirtualPos == 1){
					tobCikl = 0;  // выход из меню настройки времени ноты
				}else if(tobVirtualPos == 3){
					tobTimeOfBlowMnojitel *= 10;
					if(tobTimeOfBlowMnojitel > 100){tobTimeOfBlowMnojitel = 1;}
				}else if(tobVirtualPos == 4){
					timeOfBlowUnicNotaSet(tobKolocol);
				}
				delay(buttonDelay);

			}else if(tobKey == 2){  // l
				tobVirtualPos --;
				if(tobVirtualPos < 1){tobVirtualPos = 4;}
				delay(buttonDelay);
			}else if(tobKey == 3){  // d
				if(tobVirtualPos == 1){
					tobCikl = 0;  // выход из меню настройки времени ноты
				}else if(tobVirtualPos == 2){
					tobKolocol --;
					if(tobKolocol <= 0){tobKolocol = 24;}
				}else if(tobVirtualPos == 3){

					switch (tobKolocol) {
					    case 1:
					      timtOfBlowUnicNota_1 -= tobTimeOfBlowMnojitel;
					      break;
					    case 2:
					      timtOfBlowUnicNota_2 -= tobTimeOfBlowMnojitel;
					      break;
					    case 3:
					      timtOfBlowUnicNota_3 -= tobTimeOfBlowMnojitel;
					      break;
					    case 4:
					      timtOfBlowUnicNota_4 -= tobTimeOfBlowMnojitel;
					      break;
					    case 5:
					      timtOfBlowUnicNota_5 -= tobTimeOfBlowMnojitel;
					      break;
					    case 6:
					      timtOfBlowUnicNota_6 -= tobTimeOfBlowMnojitel;
					      break;
					    case 7:
					      timtOfBlowUnicNota_7 -= tobTimeOfBlowMnojitel;
					      break;
					    case 8:
					      timtOfBlowUnicNota_8 -= tobTimeOfBlowMnojitel;
					      break;
					    case 9:
					      timtOfBlowUnicNota_9 -= tobTimeOfBlowMnojitel;
					      break;
					    case 10:
					      timtOfBlowUnicNota_10 -= tobTimeOfBlowMnojitel;
					      break;
					    case 11:
					      timtOfBlowUnicNota_11 -= tobTimeOfBlowMnojitel;
					      break;
					    case 12:
					      timtOfBlowUnicNota_12 -= tobTimeOfBlowMnojitel;
					      break;
					    case 13:
					      timtOfBlowUnicNota_13 -= tobTimeOfBlowMnojitel;
					      break;
					    case 14:
					      timtOfBlowUnicNota_14 -= tobTimeOfBlowMnojitel;
					      break;
					    case 15:
					      timtOfBlowUnicNota_15 -= tobTimeOfBlowMnojitel;
					      break;
					    case 16:
					      timtOfBlowUnicNota_16 -= tobTimeOfBlowMnojitel;
					      break;
					    case 17:
					      timtOfBlowUnicNota_17 -= tobTimeOfBlowMnojitel;
					      break;
					    case 18:
					      timtOfBlowUnicNota_18 -= tobTimeOfBlowMnojitel;
					      break;
					    case 19:
					      timtOfBlowUnicNota_19 -= tobTimeOfBlowMnojitel;
					      break;
					    case 20:
					      timtOfBlowUnicNota_20 -= tobTimeOfBlowMnojitel;
					      break;
					    case 21:
					      timtOfBlowUnicNota_21 -= tobTimeOfBlowMnojitel;
					      break;
					    case 22:
					      timtOfBlowUnicNota_22 -= tobTimeOfBlowMnojitel;
					      break;
					    case 23:
					      timtOfBlowUnicNota_23 -= tobTimeOfBlowMnojitel;
					      break;
					    case 24:
					      timtOfBlowUnicNota_24 -= tobTimeOfBlowMnojitel;
					      break;
					}

					if(tobTimeOfBlow <= 0){tobTimeOfBlow = 9999;}
				}else if(tobVirtualPos == 4){
					timeOfBlowUnicNotaSet(tobKolocol);
				}
				delay(buttonDelay);
			}else if(tobKey == 4){  // u
				if(tobVirtualPos == 1){
					delay(buttonDelay);
					bellForMenu();
				}else if(tobVirtualPos == 2){
					tobKolocol ++;
					if(tobKolocol > 24){tobKolocol = 1;}
				}else if(tobVirtualPos == 3){

					switch (tobKolocol) {
					    case 1:
					      timtOfBlowUnicNota_1 += tobTimeOfBlowMnojitel;
					      break;
					    case 2:
					      timtOfBlowUnicNota_2 += tobTimeOfBlowMnojitel;
					      break;
					    case 3:
					      timtOfBlowUnicNota_3 += tobTimeOfBlowMnojitel;
					      break;
					    case 4:
					      timtOfBlowUnicNota_4 += tobTimeOfBlowMnojitel;
					      break;
					    case 5:
					      timtOfBlowUnicNota_5 += tobTimeOfBlowMnojitel;
					      break;
					    case 6:
					      timtOfBlowUnicNota_6 += tobTimeOfBlowMnojitel;
					      break;
					    case 7:
					      timtOfBlowUnicNota_7 += tobTimeOfBlowMnojitel;
					      break;
					    case 8:
					      timtOfBlowUnicNota_8 += tobTimeOfBlowMnojitel;
					      break;
					    case 9:
					      timtOfBlowUnicNota_9 += tobTimeOfBlowMnojitel;
					      break;
					    case 10:
					      timtOfBlowUnicNota_10 += tobTimeOfBlowMnojitel;
					      break;
					    case 11:
					      timtOfBlowUnicNota_11 += tobTimeOfBlowMnojitel;
					      break;
					    case 12:
					      timtOfBlowUnicNota_12 += tobTimeOfBlowMnojitel;
					      break;
					    case 13:
					      timtOfBlowUnicNota_13 += tobTimeOfBlowMnojitel;
					      break;
					    case 14:
					      timtOfBlowUnicNota_14 += tobTimeOfBlowMnojitel;
					      break;
					    case 15:
					      timtOfBlowUnicNota_15 += tobTimeOfBlowMnojitel;
					      break;
					    case 16:
					      timtOfBlowUnicNota_16 += tobTimeOfBlowMnojitel;
					      break;
					    case 17:
					      timtOfBlowUnicNota_17 += tobTimeOfBlowMnojitel;
					      break;
					    case 18:
					      timtOfBlowUnicNota_18 += tobTimeOfBlowMnojitel;
					      break;
					    case 19:
					      timtOfBlowUnicNota_19 += tobTimeOfBlowMnojitel;
					      break;
					    case 20:
					      timtOfBlowUnicNota_20 += tobTimeOfBlowMnojitel;
					      break;
					    case 21:
					      timtOfBlowUnicNota_21 += tobTimeOfBlowMnojitel;
					      break;
					    case 22:
					      timtOfBlowUnicNota_22 += tobTimeOfBlowMnojitel;
					      break;
					    case 23:
					      timtOfBlowUnicNota_23 += tobTimeOfBlowMnojitel;
					      break;
					    case 24:
					      timtOfBlowUnicNota_24 += tobTimeOfBlowMnojitel;
					      break;
					}

					if(tobTimeOfBlow > 9999){tobTimeOfBlow = 1;}
				}else if(tobVirtualPos == 4){
					timeOfBlowUnicNotaSet(tobKolocol);
				}
				delay(buttonDelay);
			}else if(tobKey == 5){  // r
				tobVirtualPos ++;
				if(tobVirtualPos > 4){tobVirtualPos = 1;}
				delay(buttonDelay);
			}

			switch (tobVirtualPos){
			    case 1:
			      tobRealPos = 0;
			      break;
			    case 2:
			      tobRealPos = 2;
			      break;
			    case 3:
			      tobRealPos = 9;
			      break;
			    case 4:
			      tobRealPos = 14;
			      break;
			}

			drawTimtOfBlowUnicNotaForMenu(tobKolocol);
			lcd.setCursor(tobRealPos,1);
			lcd.write(byte(4));
			if(tobVirtualPos == 3){
				lcd.print(tobTimeOfBlowMnojitel);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// функция рисования для меню проигрывания мелодий
inline void drawSpisokMelodiyForMenu(String _melodiaSTR){

	String dsmMelodiaSTR = _melodiaSTR;
	lcd.clear();
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(1));
	lcd.setCursor(2,0);
	lcd.print(dsmMelodiaSTR);
	lcd.setCursor(14,0);
	lcd.print("OK");
}

// функция проигрывания для меню проигрывания мелодий
void proigratMelodiuForMenu(byte _smVirtualMelodia){
	byte pmVirtualMelodia = _smVirtualMelodia;
	switch (pmVirtualMelodia) {
					    case 1:
					      melodia1();
					      break;
					    case 2:
					      melodia2();
					      break;
					    case 3:
					      melodia3();
					      break;
					    case 4:
					      melodia4();
					      break;
					    case 5:
					      melodia5();
					      break;
					    case 6:
					      melodia6();
					      break;
					    case 7:
					      melodia7();
					      break;
					    case 8:
					      melodia8();
					      break;
					    case 9:
					      melodia9();
					      break;
					    case 10:
					      melodia10();
					      break;
					    case 11:
					      melodia11();
					      break;
					    case 12:
					      melodia12();
					      break;
					    case 13:
					      melodia15();
					      break;
					    case 14:
					      melodia30();
					      break;
					    case 15:
					      melodia45();
					      break;
					    case 16:
					      melodiaEX1();
					      break;
					    case 17:
					      melodiaEX2();
					      break;
					    case 18:
					      melodiaEX3();
					      break;
					    case 19:
					      melodiaRes1();
					      break;
					    case 20:
					      melodiaRes2();
					      break;
					    case 21:
					      melodiaRes3();
					      break;
					    case 22:
					      melodiaRes4();
					      break;
					    case 23:
					      melodiaRes5();
					      break;
					    case 24:
					      melodiaRes6();
					      break;
					    case 25:
					      melodiaRes7();
					      break;
					    case 26:
					      budnichniy();
					      break;
					}
}
// список мелодий для меню
void spisokMelodiyForMenu(){

	unsigned int buttonDelay = 200;  // задержка для меню
	bool smCikl = 1;  // переменная для управления циклом while
	byte smVirtualPos = 1;   // виртуальная позиция указателя меню
	byte smRealPos = 0;   // реальная позиция указателя меню
	byte smVirtualMelodia = 1;  // виртуальная мелодия
	String smRealMelodia = "melodia1";   // реальная мелодия
	byte smKey = 0;   // значение кнопок для обработки в цикле while

	drawSpisokMelodiyForMenu(smRealMelodia);
	lcd.setCursor(0,1);
	lcd.write(byte(4));

	while(smCikl){
		smKey = key();
		if(smKey > 0){

			if(smKey == 1){  // s
				if(smVirtualPos == 1){
					smCikl = 0;  // выход из меню
				}else if(smVirtualPos == 2){
					proigratMelodiuForMenu(smVirtualMelodia);
				}else if(smVirtualPos == 3){
					proigratMelodiuForMenu(smVirtualMelodia);
				}
				
				delay(buttonDelay);

			}else if(smKey == 2){  // l
				smVirtualPos --;
				if(smVirtualPos < 1){smVirtualPos = 3;}
				delay(buttonDelay);
			}else if(smKey == 3){  // d
				if(smVirtualPos == 1){
					smCikl = 0;  // выход из меню
				}else if(smVirtualPos == 2){
					smVirtualMelodia --;
					if(smVirtualMelodia <= 0){smVirtualMelodia = 26;}
				}else if(smVirtualPos == 3){
					proigratMelodiuForMenu(smVirtualMelodia);
				}
				delay(buttonDelay);
			}else if(smKey == 4){  // u
				if(smVirtualPos == 1){
					delay(buttonDelay);
					timtOfBlowUnicNotaForMenu();
				}else if(smVirtualPos == 2){
					smVirtualMelodia ++;
					if(smVirtualMelodia > 26){smVirtualMelodia = 1;}
				}else if(smVirtualPos == 3){
					proigratMelodiuForMenu(smVirtualMelodia);
				}
				delay(buttonDelay);
			}else if(smKey == 5){  // r
				smVirtualPos ++;
				if(smVirtualPos > 3){smVirtualPos = 1;}
				delay(buttonDelay);
			}

			switch (smVirtualPos){
			    case 1:
			      smRealPos = 0;
			      break;
			    case 2:
			      smRealPos = 5;
			      break;
			    case 3:
			      smRealPos = 14;
			      break;
			}

			switch (smVirtualMelodia) {
					    case 1:
					      smRealMelodia = "melodia1";
					      break;
					    case 2:
					      smRealMelodia = "melodia2";
					      break;
					    case 3:
					      smRealMelodia = "melodia3";
					      break;
					    case 4:
					      smRealMelodia = "melodia4";
					      break;
					    case 5:
					      smRealMelodia = "melodia5";
					      break;
					    case 6:
					      smRealMelodia = "melodia6";
					      break;
					    case 7:
					      smRealMelodia = "melodia7";
					      break;
					    case 8:
					      smRealMelodia = "melodia8";
					      break;
					    case 9:
					      smRealMelodia = "melodia9";
					      break;
					    case 10:
					      smRealMelodia = "melodia10";
					      break;
					    case 11:
					      smRealMelodia = "melodia11";
					      break;
					    case 12:
					      smRealMelodia = "melodia12";
					      break;
					    case 13:
					      smRealMelodia = "melodia15";
					      break;
					    case 14:
					      smRealMelodia = "melodia30";
					      break;
					    case 15:
					      smRealMelodia = "melodia45";
					      break;
					    case 16:
					      smRealMelodia = "melodiaEX1";
					      break;
					    case 17:
					      smRealMelodia = "melodiaEX2";
					      break;
					    case 18:
					      smRealMelodia = "melodiaEX3";
					      break;
					    case 19:
					      smRealMelodia = "melodiaRes1";
					      break;
					    case 20:
					      smRealMelodia = "melodiaRes2";
					      break;
					    case 21:
					      smRealMelodia = "melodiaRes3";
					      break;
					    case 22:
					      smRealMelodia = "melodiaRes4";
					      break;
					    case 23:
					      smRealMelodia = "melodiaRes5";
					      break;
					    case 24:
					      smRealMelodia = "melodiaRes6";
					      break;
					    case 25:
					      smRealMelodia = "melodiaRes7";
					      break;
					    case 26:
					      smRealMelodia = "budnichniy";
					      break;
					}

			drawSpisokMelodiyForMenu(smRealMelodia);
			lcd.setCursor(smRealPos,1);
			lcd.write(byte(4));
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ НАСТРОЙКИ ЧАСОВ ИЗ МЕНЮ
// функция рисования для функции настройки даты из меню
inline void drawDataSetForMenu(int _day = 27, int _month = 11, int _year =2017){

	int ddsDay = _day;
	int ddsMonth = _month;
	int ddsYear = _year;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(5));
	lcd.setCursor(2,0);
	lcd.print(ddsDay);
	lcd.setCursor(5,0);
	lcd.print(ddsMonth);
	lcd.setCursor(8,0);
	lcd.print(ddsYear);
	lcd.setCursor(14,0);
	lcd.print("OK");
}

// функция рисования для функции настройки часов из меню
inline void drawTimeSetForMenu(int _hour = 18, int _minute = 30, int _second = 30){

	int dtsHour = _hour;
	int dtsMinute = _minute;
	int dtsSecond = _second;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(5));
	lcd.setCursor(2,0);
	lcd.print(dtsHour);
	lcd.setCursor(5,0);
	lcd.print(dtsMinute);
	lcd.setCursor(8,0);
	lcd.print(dtsSecond);
	lcd.setCursor(12,0);
	lcd.print("SAVE");
}

void timeSetForMenu(){
	// rday;
	// rmonth;
	// ryear;
	// rhour;
	// rminute;
	// rsecond;
	unsigned int buttonDelay = 200;  // задержка для меню
	bool tsCikl = 1;  // переменная для управления циклом while
	byte tsVirtualPos = 1;   // виртуальная позиция указателя меню
	byte tsRealPos = 0;   // реальная позиция указателя меню
	byte tsKey = 0;   // значение кнопок для обработки в цикле while

	chekVremya();
	drawDataSetForMenu(rday, rmonth, ryear);
	lcd.setCursor(0,1);
	lcd.write(byte(4));

	while(tsCikl){

		tsKey = key();
		if(tsKey > 0){

		    if(tsKey == 1){  // s
		    	if(tsVirtualPos == 1){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsCikl = 0;
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);

		    }else if(tsKey == 2){  // l
		    	tsVirtualPos --;
		    	if(tsVirtualPos < 1){tsVirtualPos = 1;}
		    	delay(buttonDelay);

		    }else if(tsKey == 3){  // d
		    	if(tsVirtualPos == 1){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 2){
		    		rday --;
		    		if(rday<1){rday = 31;}
		    	}else if(tsVirtualPos == 3){
		    		rmonth --;
		    		if(rmonth<1){rmonth = 12;}
		    	}else if(tsVirtualPos == 4){
		    		ryear --;
		    		if(ryear<2000){ryear = 2222;}
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsCikl = 0;
		    	}else if(tsVirtualPos == 7){
		    		rhour --;
		    		if(rhour<0){rhour = 23;}
		    	}else if(tsVirtualPos == 8){
		    		rminute --;
		    		if(rminute<0){rminute = 59;}
		    	}else if(tsVirtualPos == 9){
		    		rsecond --;
		    		if(rsecond<0){rsecond = 59;}
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);

		    }else if(tsKey == 4){  // u
		    	if(tsVirtualPos == 1){
		    		delay(buttonDelay);
					spisokMelodiyForMenu();
		    	}else if(tsVirtualPos == 2){
		    		rday ++;
		    		if(rday>31){rday = 1;}
		    	}else if(tsVirtualPos == 3){
		    		rmonth ++;
		    		if(rmonth>12){rmonth = 1;}
		    	}else if(tsVirtualPos == 4){
		    		ryear ++;
		    		if(ryear>2222){ryear = 2000;}
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsVirtualPos = 1;
		    	}else if(tsVirtualPos == 7){
		    		rhour ++;
		    		if(rhour>23){rhour = 0;}
		    	}else if(tsVirtualPos == 8){
		    		rminute ++;
		    		if(rminute>59){rminute = 0;}
		    	}else if(tsVirtualPos == 9){
		    		rsecond ++;
		    		if(rsecond>59){rsecond = 0;}
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);
		    	
		    }else if(tsKey == 5){  // r
		    	tsVirtualPos ++;
		    	if(tsVirtualPos > 10){tsVirtualPos = 10;}
		    }
		    delay(buttonDelay);

		    switch (tsVirtualPos){
			    case 1:
			      tsRealPos = 0;
			      break;
			    case 2:
			      tsRealPos = 2;
			      break;
			    case 3:
			      tsRealPos = 5;
			      break;
			    case 4:
			      tsRealPos = 9;
			      break;
			    case 5:
			      tsRealPos = 14;
			      break;
			    case 6:
			      tsRealPos = 0;
			      break;
			    case 7:
			      tsRealPos = 2;
			      break;
			    case 8:
			      tsRealPos = 5;
			      break;
			    case 9:
			      tsRealPos = 8;
			      break;
			    case 10:
			      tsRealPos = 13;
			      break;
			}

			if(tsVirtualPos<=5){
				drawDataSetForMenu(rday, rmonth, ryear);
			}else if(tsVirtualPos>5){
				drawTimeSetForMenu(rhour, rminute, rsecond);
			}
			lcd.setCursor(tsRealPos,1);
			lcd.write(byte(4));
			if(tsVirtualPos == 5){
				lcd.write(byte(3));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ФУНКЦИЯ МЕНЮ:
void menu24(){
	lcd.clear();
	lcd.setCursor(6,0);
	lcd.print("MENU");
	delay(1000);
	bellForMenu();
	timtOfBlowUnicNotaForMenu();
	spisokMelodiyForMenu();
	timeSetForMenu();
	lcd.clear();
	lcd.setCursor(6,0);
	lcd.print("EXIT");
	delay(1000);
	rminute = 61;
	timeToDisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

	#if DEF_TIME_OF_BLOW_UNIC_NOTA_ENABLE == 1
	timeOfBlowUnicNotaSetPak();  // запись длительностей нот в EEPROM из файла "config24.h" (применяется 1 раз)
	#endif
	timeOfBlowUnicNotaGetPak();  // чтение длительностей нот из EEPROM в переменную (выполняется при старте МК)

	// ЧАСЫ:
	clock.begin();         // Инициализируем работу с объектом библиотеки DS3231
	#if SET_CLOK_FOR_PROG == 1
 	clock.setDateTime(__DATE__, __TIME__);      // Устанавливаем время на часах, основываясь на времени компиляции скетча
	//clock.setDateTime(2016, 9, 15, 0, 0, 0);      // Установка времени вручную (Год, Месяц, День, Час, Минута, Секунда)
	#endif
	//clock.enableOutput(false);// Определяем назначение вывода SQW (INT) для генерации прерываний при сработке будильников
	//setAlarm1(Дата или день, Час, Минута, Секунда, Режим)
	//clock.setAlarm1(0, 0, 0, 1, DS3231_MATCH_S);           // Устанавливаем первый будильник на срабатывание в 10 сек.

  	//pinMode(LED_BUILTIN, OUTPUT);  // фиксим 13 диод
	//Serial.begin(9600);
	Wire.begin();

	// инициализация пользовательских символов
  	lcd.createChar(0, customCharBell0);
  	lcd.createChar(1, customCharNota1);
  	lcd.createChar(2, customCharPR2);
  	lcd.createChar(3, customCharRightArrow3);
  	lcd.createChar(4, customCharMenuArrow4);
  	lcd.createChar(5, customCharChasi5);

	// инициализация колоколов
	pinMode(RELE_K1, OUTPUT);
	pinMode(RELE_K2, OUTPUT);
	pinMode(RELE_K3, OUTPUT);
	pinMode(RELE_K4, OUTPUT);
	pinMode(RELE_K5, OUTPUT);
	pinMode(RELE_K6, OUTPUT);
	pinMode(RELE_K7, OUTPUT);
	pinMode(RELE_K8, OUTPUT);
	pinMode(RELE_K9, OUTPUT);
	pinMode(RELE_K10, OUTPUT);
	pinMode(RELE_K11, OUTPUT);
	pinMode(RELE_K12, OUTPUT);
	pinMode(RELE_K13, OUTPUT);
	pinMode(RELE_K14, OUTPUT);
	pinMode(RELE_K15, OUTPUT);
	pinMode(RELE_K16, OUTPUT);
	pinMode(RELE_K17, OUTPUT);
	pinMode(RELE_K18, OUTPUT);
	pinMode(RELE_K19, OUTPUT);
	pinMode(RELE_K20, OUTPUT);
	pinMode(RELE_K21, OUTPUT);
	pinMode(RELE_K22, OUTPUT);
	pinMode(RELE_K23, OUTPUT);
	pinMode(RELE_K24, OUTPUT);

	digitalWrite(RELE_K1, LLL);
	digitalWrite(RELE_K2, LLL);
	digitalWrite(RELE_K3, LLL);
	digitalWrite(RELE_K4, LLL);
	digitalWrite(RELE_K5, LLL);
	digitalWrite(RELE_K6, LLL);
	digitalWrite(RELE_K7, LLL);
	digitalWrite(RELE_K8, LLL);
	digitalWrite(RELE_K9, LLL);
	digitalWrite(RELE_K10, LLL);
	digitalWrite(RELE_K11, LLL);
	digitalWrite(RELE_K12, LLL);
	digitalWrite(RELE_K13, LLL);
	digitalWrite(RELE_K14, LLL);
	digitalWrite(RELE_K15, LLL);
	digitalWrite(RELE_K16, LLL);
	digitalWrite(RELE_K17, LLL);
	digitalWrite(RELE_K18, LLL);
	digitalWrite(RELE_K19, LLL);
	digitalWrite(RELE_K20, LLL);
	digitalWrite(RELE_K21, LLL);
	digitalWrite(RELE_K22, LLL);
	digitalWrite(RELE_K23, LLL);
	digitalWrite(RELE_K24, LLL);

	#if DEF_TEMP_ENABLE == 1
	pinMode(DEF_TEMP_RELE_PIN, OUTPUT);
	digitalWrite(DEF_TEMP_RELE_PIN, LLL);
	#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	#if DEF_YARKOST_DISPLEYA_DEF <= 255
		analogWrite(DEF_LCD_LIGHT_PIN, DEF_YARKOST_DISPLEYA_DEF);  // установка яркости
	#endif

	lcd.begin(16, 2);
    lcd.clear();
    lcd.clear();
	lcd.print("   MEGAZVON24");
	delay(500);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

	timeToDisplay();  // вывод времени на дисплей
	buttonChekForLoop();  // проверка кнопок
	chekPerezvon();  // проверка расписания
	#if DEF_TEMP_ENABLE == 1
	termoregulator();
	#endif
	delay(200);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
