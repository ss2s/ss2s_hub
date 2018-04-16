#include <EEPROM.h>

//=====================================================================================================
// НАСТРОЙКИ:
//=====================================================================================================

// РАСПИНОВКА Arduino
#define STEP_PIN 3      // step драйвера (Y)
#define DIR_PIN 6       // dir драйвера (Y)
#define LIMIT_PIN 10    // концевик
#define SWITCH_P 9      // кнопка + скорость
#define SWITCH_M 11     // кнопка - скорость

// эта опция нужна, для внесения изменений в EEPROM при перепрошивке
// если изменить число то EEPROM перезапишется при перепрошивке
#define EEPROM_WRITE_KEY 123  // любое трехзначное число

// настройка параметров

unsigned int sectorVal = 104;            // сектор работы маятника в градусах

unsigned int stepDivider = 32;           // деление шага на драйвере 1-2-4-8-16-32. без делителя 1

float degresInStep = 1.8;                // градусов в одном шаге(характеристики двигателя)

bool dirValSet = 1;                      // направление вращения при старте 1 или 0

bool limitSwitchSet = 0;                 // режим работы концевого выключателя 1 или 0 (сработка на флажок или на отверстие)

bool limitSwitchEnable = 0;              // разрешить использовать концевик 1 : запретить 0

unsigned int degresPerSecond = 69;       // скорость, градусов в секунду


unsigned long stepHighDelay = 500;       // микросекунд будет высокий уровень при шаге (шум/мощность)

//=====================================================================================================
// КОНЕЦ НАСТРОЕК
//=====================================================================================================


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
unsigned int readFromMemory(int fAddr){

	int rAddr = fAddr;
	unsigned int rVal = EEPROM_uint_read(rAddr);
	switch (rAddr) {

	    case 1:
	      degresPerSecond = rVal;
	      break;    
	}
	return rVal;
}
//*****************************************************************

void setSpeedVal(){	
stepLowDelay = ((1000000/((1/(degresInStep/stepDivider))*degresPerSecond))-stepHighDelay); //-- здесь внести изменения в скорость
}


void buttonChekForLoop(){
}

void setup() {

	if(readFromMemory(2) != EEPROM_WRITE_KEY){
		saveToMemory(1, degresPerSecond);
		saveToMemory(2, EEPROM_WRITE_KEY);
	}else{
		degresPerSecond = readFromMemory(1);
	}

	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
}

void loop() {

	stepRuner();
	buttonChekForLoop();
}
