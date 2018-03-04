#include <LiquidCrystal.h>
#include <EEPROM.h>

#define STEP_PIN 11  // step
#define DIR_PIN 12   // dir

#define EEPROM_WRITE_KEY 12345  // если изменить то EEPROM перезапишется при перепрошивке

unsigned int stepVal = 500;  // скорость вращения в герцах (шагов в секунду)
unsigned int dirVal =1;       // направление 1 или 0

unsigned int EEPROMwriteKeyVal = 0;

byte customCharMenuArrow0[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100
};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

unsigned int EEPROM_uint_read(int fAddr){  // чтение из EEPROM 2 байта unsigned int

	int rAddr = fAddr*2;
	byte raw[2];
  	for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(rAddr+i);
  	unsigned int &data = (unsigned int&)raw;
  	return data;
}
//*****************************************************************
void EEPROM_uint_write(int fAddr, unsigned long data){  // запись в EEPROM 2 байта unsigned int

	int rAddr = fAddr*2;
  	byte raw[2];
  	(unsigned int&)raw = data;
  	for(byte i = 0; i < 2; i++) EEPROM.write(rAddr+i, raw[i]);
}

void saveToMemory(int fAddr, unsigned int fVal){

	int rAddr = fAddr;
	unsigned int rVal = fVal;
	unsigned int readVal = EEPROM_uint_read(rAddr);
	if(readVal != rVal){
		EEPROM_uint_write(rAddr, rVal);
	}
}

unsigned int readFromMemory(int fAddr){

	int rAddr = fAddr;
	unsigned int rVal = EEPROM_uint_read(rAddr);
	switch (rAddr) {

	    case 1:
	      stepVal = rVal;
	      break;
	    case 2:
	      dirVal = rVal;
	      break;
	    case 3:
	      EEPROMwriteKeyVal = rVal;
	      break;    
	}

	return rVal;
}

byte key(){  // 1-723, 2-482, 3-133, 4-310, 5-0;
	int val = analogRead(A0);
	byte keyVal = 0;
	if(val < 67) {keyVal = 5;}             // right
	else if(val < 221) {keyVal = 4;}       // up
	else if(val < 396) {keyVal = 3;}       // down
	else if(val < 602) {keyVal = 2;}       // left
	else if(val < 873) {keyVal = 1;}       // select
	else if(val <= 1023) {keyVal = 0;}     // no press
	return keyVal;
}

void drawSetStepValForMenu(){

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("STEP");
	lcd.setCursor(6,0);
	lcd.print(stepVal);
	lcd.setCursor(13,0);
	lcd.print("SET");
}

void setStepValForMenu(){

	unsigned int buttonDelay = 400;  // задержка для меню
	bool fCiklControl = 1;
	byte fVirtualPos = 1;   // виртуальная позиция указателя меню
	byte fRealPos = 1;      // реальная позиция указателя меню
	byte fKey = 0;          // значение кнопок для обработки в цикле while
	unsigned int fMnojitel = 1;  // множитель для мс. в меню

	drawSetStepValForMenu();
	lcd.setCursor(fRealPos,1);
	lcd.write(byte(0));

	while(fCiklControl){

		fKey = key();
		if(fKey > 0){

			delay(buttonDelay);

			if(fKey == 1){  // s

				if(fVirtualPos == 2){

					fMnojitel *= 10;
					if(fMnojitel > 10000){

						fMnojitel = 1;
					}
				}
			}
			else if(fKey == 2){  // l

				fVirtualPos --;
				if(fVirtualPos < 1){

					fVirtualPos = 3;
				}
			}
			else if(fKey == 3){  // d

				if(fVirtualPos == 1){

					fCiklControl = 0;
				}
				else if(fVirtualPos == 2){

					if(stepVal > 0){

						if((stepVal - fMnojitel) < stepVal){

							stepVal -= fMnojitel;
						}else{
							stepVal = 0;
						}
					}
					tone(STEP_PIN, stepVal);
				}
				else if(fVirtualPos == 3){

					saveToMemory(1, stepVal);
					fCiklControl = 0;
				}
			}
			else if(fKey == 4){  // u

				if(fVirtualPos == 1){

					fCiklControl = 0;
				}
				else if(fVirtualPos == 2){

					if(stepVal < 65535){

						if((stepVal + fMnojitel) > stepVal){

							stepVal += fMnojitel;
						}else{

							stepVal = 65535;
						}
					}
					tone(STEP_PIN, stepVal);
				}
				else if(fVirtualPos == 3){

					saveToMemory(1, stepVal);
					fCiklControl = 0;
				}
			}
			else if(fKey == 5){  // r

				fVirtualPos ++;
				if(fVirtualPos > 3){

					fVirtualPos = 1;
				}
			}
			switch (fVirtualPos){

			    case 1:
			    fRealPos = 1;
			    break;
			    case 2:
			    fRealPos = 6;
			    break;
			    case 3:
			    fRealPos = 14;
			    break;
			}

			drawSetStepValForMenu();
			lcd.setCursor(fRealPos,1);
			lcd.write(byte(0));
			if(fVirtualPos == 2){
				lcd.print("+");
				lcd.print(fMnojitel);
			}
		}
	}
}

void drawSetDirValForMenu(){

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("DIR");
	lcd.setCursor(6,0);
	lcd.print(dirVal);
	lcd.setCursor(13,0);
	lcd.print("SET");
}

void setDirValForMenu(){

	unsigned int buttonDelay = 400;  // задержка для меню
	bool fCiklControl = 1;
	byte fVirtualPos = 1;   // виртуальная позиция указателя меню
	byte fRealPos = 1;      // реальная позиция указателя меню
	byte fKey = 0;          // значение кнопок для обработки в цикле while

	drawSetDirValForMenu();
	lcd.setCursor(fRealPos,1);
	lcd.write(byte(0));

	while(fCiklControl){

		fKey = key();
		if(fKey > 0){

			delay(buttonDelay);

			if(fKey == 1){  // s
			}
			else if(fKey == 2){  // l

				fVirtualPos --;
				if(fVirtualPos < 1){

					fVirtualPos = 3;
				}
			}
			else if(fKey == 3){  // d

				if(fVirtualPos == 1){

					fCiklControl = 0;
				}
				else if(fVirtualPos == 2){

					dirVal = 0;
					digitalWrite(DIR_PIN, LOW);
				}
				else if(fVirtualPos == 3){

					saveToMemory(2, dirVal);
					fCiklControl = 0;
				}
			}
			else if(fKey == 4){  // u

				if(fVirtualPos == 1){

					fCiklControl = 0;
				}
				else if(fVirtualPos == 2){

					dirVal = 1;
					digitalWrite(DIR_PIN, HIGH);
				}
				else if(fVirtualPos == 3){

					saveToMemory(2, dirVal);
					fCiklControl = 0;
				}
			}
			else if(fKey == 5){  // r

				fVirtualPos ++;
				if(fVirtualPos > 3){

					fVirtualPos = 1;
				}
			}
			switch (fVirtualPos){

			    case 1:
			    fRealPos = 1;
			    break;
			    case 2:
			    fRealPos = 6;
			    break;
			    case 3:
			    fRealPos = 14;
			    break;
			}

			drawSetDirValForMenu();
			lcd.setCursor(fRealPos,1);
			lcd.write(byte(0));
		}
	}
}

void stepperSetingMenu(){

	lcd.clear();
	lcd.print("     SETING");
	delay(1000);

	setStepValForMenu();
	setDirValForMenu();

	if(dirVal > 0){
		digitalWrite(DIR_PIN, HIGH);
	}else{
		digitalWrite(DIR_PIN, LOW);
	}
	tone(STEP_PIN, stepVal);

	lcd.clear();
	lcd.print("  EXIT SETING");
	delay(1000);
	lcd.clear();
	lcd.print("STEP  ");
	lcd.print(stepVal);
	lcd.setCursor(0,1);
	lcd.print("DIR  ");
	lcd.print(dirVal);
}

void buttonChekForLoop(){

	byte val = key();
	unsigned int buttonDelay = 200;  // задержка для меню
	if(val > 0){
		delay(buttonDelay);
	}
	if(val == 1){  // select
		stepperSetingMenu();
	}
}

void setup() {

	if(readFromMemory(3) != EEPROM_WRITE_KEY){
		saveToMemory(1, stepVal);
		saveToMemory(2, dirVal);
		saveToMemory(3, EEPROM_WRITE_KEY);
	}else{
		stepVal = readFromMemory(1);
		dirVal = readFromMemory(2);
	}

	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);

	if(dirVal > 0){
		digitalWrite(DIR_PIN, HIGH);
	}else{
		digitalWrite(DIR_PIN, LOW);
	}
	// digitalWrite(DIR_PIN, dirVal);

	tone(STEP_PIN, stepVal);

	lcd.createChar(0, customCharMenuArrow0);

	lcd.begin(16, 2);
    lcd.clear();
    lcd.clear();
	lcd.print("STEP DIR CONTROL");
	delay(500);
	lcd.clear();
	lcd.print("STEP  ");
	lcd.print(stepVal);
	lcd.setCursor(0,1);
	lcd.print("DIR  ");
	lcd.print(dirVal);
}

void loop() {

	buttonChekForLoop();
	delay(50);
}
