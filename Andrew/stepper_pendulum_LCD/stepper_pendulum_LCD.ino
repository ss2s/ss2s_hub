#include <LiquidCrystal.h>
#include <EEPROM.h>

//=====================================================================================================
// НАСТРОЙКИ:
//=====================================================================================================

// РАСПИНОВКА Arduino
#define STEP_PIN 11   // step драйвера
#define DIR_PIN 12    // dir драйвера
#define LIMIT_PIN 13  // концевик

// эта опция нужна, для внесения изменений в EEPROM при перепрошивке
// если изменить число то EEPROM перезапишется при перепрошивке
#define EEPROM_WRITE_KEY 123  // любое трехзначное число

// настройка параметров двигателя
float degresInStep = 1.8;                // градусов в одном шаге(характеристики двигателя) 1.8 = 200 шагов на оборот
unsigned int stepDivider = 32;           // деление шага на драйвере 1-2-4-8-16-32. без делителя 1

// настройка параметров маятника
unsigned int sectorVal = 104;            // сектор работы маятника в градусах
bool dirValSet = 1;                      // направление вращения при старте 1 или 0
unsigned int degresPerSecond = 70;       // скорость, градусов в секунду от 0 до maxSpeed
unsigned int maxSpeed = 1000;            // максимальная скорость, градусов в секунду

// настройка концевого выключателя
bool limitSwitchSet = 1;                 // режим работы концевого выключателя (сработка на флажок 1 : на отверстие 0)
bool limitSwitchType = 0;                // тип концевого выключателя. 0 аналоговый : 1 цифровой
bool limitSwitchEnable = 1;              // разрешить использовать концевик 1 : запретить 0


unsigned int menuDelay = 400;            // задержка для меню мс
unsigned int openMenuDelay = 1000;       // задержка открытия меню мс


String deviceName = "STEPPER PENDULUM";  // название прибора (при включении) не больше 16 символв с пробелами

unsigned long stepHighDelay = 1;         // микросекунд будет высокий уровень при шаге (шум/мощность)

//=====================================================================================================
// КОНЕЦ НАСТРОЕК
//=====================================================================================================


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


bool dirVal = dirValSet;
long int stepPerSector = (sectorVal / degresInStep) * stepDivider;  // шагов в секторе
unsigned long stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay);       // микросекунд между шагами

void stepRuner(){

	digitalWrite(DIR_PIN, dirVal);

	for(int i = 0; i < stepPerSector;  i ++){

		digitalWrite(STEP_PIN, HIGH);
		delayMicroseconds(stepHighDelay);
		digitalWrite(STEP_PIN, LOW);
		delayMicroseconds(stepLowDelay);

		if((digitalRead(LIMIT_PIN) == limitSwitchSet) && limitSwitchEnable && (dirVal == dirValSet)){

			break;
		}
	}

	dirVal = !dirVal;
}



//*****************************************************************
unsigned int EEPROM_uint_read(int fAddr){  // чтение из EEPROM 2 байта unsigned int

	int rAddr = fAddr*2;
	byte raw[2];
  	for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(rAddr+i);
  	unsigned int &data = (unsigned int&)raw;
  	return data;
}
//*****************************************************************
void EEPROM_uint_write(int fAddr, unsigned int data){  // запись в EEPROM 2 байта unsigned int

	int rAddr = fAddr*2;
  	byte raw[2];
  	(unsigned int&)raw = data;
  	for(byte i = 0; i < 2; i++) EEPROM.write(rAddr+i, raw[i]);
}



//*****************************************************************
void saveToMemory(int fAddr, unsigned int fVal){

	int rAddr = fAddr;
	unsigned int rVal = fVal;
	unsigned int readVal = EEPROM_uint_read(rAddr);
	if(readVal != rVal){
		EEPROM_uint_write(rAddr, rVal);
	}
}
//*****************************************************************


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


void drawSetSpeedValForMenu(){

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("SPEED");
	lcd.setCursor(6,0);
	lcd.print(degresPerSecond);
	lcd.setCursor(13,0);
	lcd.print("SET");
}

void setSpeedValForMenu(){

	unsigned int buttonDelay = menuDelay;  // задержка для меню
	bool fCiklControl = 1;
	byte fVirtualPos = 1;   // виртуальная позиция указателя меню
	byte fRealPos = 1;      // реальная позиция указателя меню
	byte fKey = 0;          // значение кнопок для обработки в цикле while
	unsigned int fMnojitel = 1;  // множитель для мс. в меню

	drawSetSpeedValForMenu();
	lcd.setCursor(fRealPos,1);
	lcd.write(byte(0));

	while(fCiklControl){

		fKey = key();
		if(fKey > 0){

			delay(buttonDelay);

			if(fKey == 1){  // s

				if(fVirtualPos == 2){

					fMnojitel *= 10;
					if(fMnojitel > 100){

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

					if(degresPerSecond > 0){

						if((degresPerSecond - fMnojitel) < degresPerSecond){

							degresPerSecond -= fMnojitel;
						}else{
							degresPerSecond = 0;
						}
					}
					stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay); //-- здесь внести изменения в скорость
				}
				else if(fVirtualPos == 3){

					saveToMemory(1, degresPerSecond);
					fCiklControl = 0;
				}
			}
			else if(fKey == 4){  // u

				if(fVirtualPos == 1){

					fCiklControl = 0;
				}
				else if(fVirtualPos == 2){

					if(degresPerSecond < maxSpeed){

						if((degresPerSecond + fMnojitel) > degresPerSecond){

							degresPerSecond += fMnojitel;
							if(degresPerSecond > maxSpeed){
								degresPerSecond = maxSpeed;
							}
						}else{

							degresPerSecond = maxSpeed;
						}
					}
					stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay); //-- здесь внести изменения в скорость
				}
				else if(fVirtualPos == 3){

					saveToMemory(1, degresPerSecond);
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

			drawSetSpeedValForMenu();
			lcd.setCursor(fRealPos,1);
			lcd.write(byte(0));
			if(fVirtualPos == 2){

				lcd.print("+");
				lcd.print(fMnojitel);
			}
		}
	}
}

void stepperSetingMenu(){

	lcd.clear();
	lcd.print("     SETING");
	delay(openMenuDelay);

	setSpeedValForMenu();

	stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay); //-- здесь внести изменения в скорость

	lcd.clear();
	lcd.print("  EXIT SETING");
	delay(openMenuDelay);
	lcd.clear();
	lcd.print("SPEED   ");
	lcd.print(degresPerSecond);
	lcd.setCursor(0,1);
	lcd.print("SECTOR  ");
	lcd.print(sectorVal);
}

void buttonChekForLoop(){
	byte val = key();
	if(val == 1){  // select
		stepperSetingMenu();
	}
}

void setup() {

	if(EEPROM_uint_read(2) != EEPROM_WRITE_KEY){
		saveToMemory(1, degresPerSecond);
		saveToMemory(2, EEPROM_WRITE_KEY);
	}else{
		degresPerSecond = EEPROM_uint_read(1);;
	}

	if(maxSpeed > 1000)maxSpeed=1000;

	stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay); //-- здесь внести изменения в скорость

	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	if(!limitSwitchType){
		pinMode(LIMIT_PIN, INPUT_PULLUP);
	}else{
		pinMode(LIMIT_PIN, INPUT);
	}

	lcd.createChar(0, customCharMenuArrow0);

	lcd.begin(16, 2);
    lcd.clear();
    lcd.clear();
	lcd.print(deviceName);

	// for(int i=0; i<2; i++){
	// 	stepRuner();
	// }
	stepRuner();

	lcd.clear();
	lcd.print("SPEED   ");
	lcd.print(degresPerSecond);
	lcd.setCursor(0,1);
	lcd.print("SECTOR  ");
	lcd.print(sectorVal);
}

void loop() {

	stepRuner();
	buttonChekForLoop();
}
