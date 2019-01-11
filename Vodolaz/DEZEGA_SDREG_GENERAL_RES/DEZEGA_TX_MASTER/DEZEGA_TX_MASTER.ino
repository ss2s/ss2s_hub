// MEGA 2560

#include "Arduino.h"            // Arduino lib
#include <SPI.h>                // SPI lib
#include <SD.h>                 // Micro SD lib
#include <Wire.h>               // библиотека I2C
#include "DS3231.h"             // библиотека часов

#include "nRF24L01.h"           // NRF lib
#include "RF24.h"               // NRF lib

#include <EEPROM.h>             // EEPROM lib





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPSELEKT 49  // пин чип селект для флэшки: SPI 50 MISO 51 MOSI 52 SCK 49 CHIPSELEKT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_CLOK_FOR_PROG 0  // если 1 то установка часов будет при записи программы. если 0 то нет
#define SDCHEK 1  // 1 ЕСЛИ ФЛЕШКИ НЕТ ТО НЕ СТАРТОВАТЬ. 0 СТАРТОВАТЬ В ЛЮБОМ СЛУЧАЕ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dataTypes:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clock dataType
DS3231 clock;                // Связываем объект clock с библиотекой DS3231
RTCDateTime DateTime;        // Определяем переменную DateTime, как описанную структурой RTCDateTime
// microSD  dataType
File dataFile;                  // переменная для работы с флэшкой
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NRF rx and tx F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM read write calibration F

// EEPROM ulong READ WRITE F
unsigned long EEPROM_ulong_read(int fAddr){  // чтение из EEPROM 4 байта unsigned long 
	int rAddr = fAddr*4;
	byte raw[4];
  	for(byte i = 0; i < 4; i++){raw[i] = EEPROM.read(rAddr+i);}
  	unsigned long &data = (unsigned long&)raw;
  	return data;
}
//**********************************************************************************************************************
void EEPROM_ulong_write(int fAddr, unsigned long data){  // запись в EEPROM 4 байта unsigned long
	int rAddr = fAddr*4;
  	byte raw[4];
  	(unsigned long&)raw = data;
  	for(byte i = 0; i < 4; i++){EEPROM.write(rAddr+i, raw[i]);}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// запись/чтение калибровочных значений из/в EEPROM
void eeCalibrWrite(){ // запись значений калибровки в EEPROM

}
void eeCalibrRead(){ // чтение значений калибровки из EEPROM

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// float Map
float flap(float fX, float fY, float fZ, float fA, float fB){
	return (fA*fX-fB*fX+fB*fY-fA*fZ)/(fY-fZ);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ОПРОС ВСЕХ ДАТЧИКОВ И КАЛИБРОВКА
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос быстрой ТЕРМОПАРЫ1 через сериал1
float valTermoparaFast1 = 0;
inline void poolTermoparaFast1(){
	valTermoparaFast1 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос медленной ТЕРМОПАРЫ2
float valTermoparaSlow2 = 0;
inline void poolTermoparaSlow2(){
	valTermoparaSlow2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO2
float valCO2 = 0;
inline void poolCO2(){
	valCO2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика O2
float valO2 = 0;
inline void poolO2(){
	valO2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO
float valCO = 0;
inline void poolCO(){
	valCO = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика ДАВЛЕНИЯ через сериал2
float valPressure = 0;
inline void poolPressure(){
	valPressure = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос уровня заряда батареи
byte valBatteryLevelTX = 50;
inline void poolBatteryLevel(){
	valBatteryLevelTX = 51;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// общий опрос всех датчиков
void poolAllSensors(){
	poolTermoparaFast1();
	poolTermoparaSlow2();
	poolCO2();
	poolO2();
	poolCO();
	poolPressure();
	poolBatteryLevel();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// калибровка всех датчиков
float cal_T1_InMin;
float cal_T1_InMax;
float cal_T1_OutMin;
float cal_T1_OutMax;

float cal_T2_InMin;
float cal_T2_InMax;
float cal_T2_OutMin;
float cal_T2_OutMax;

float cal_CO2_InMin;
float cal_CO2_InMax;
float cal_CO2_OutMin;
float cal_CO2_OutMax;

float cal_O2_InMin;
float cal_O2_InMax;
float cal_O2_OutMin;
float cal_O2_OutMax;

float cal_CO_InMin;
float cal_CO_InMax;
float cal_CO_OutMin;
float cal_CO_OutMax;

float cal_Pre_InMin;
float cal_Pre_InMax;
float cal_Pre_OutMin;
float cal_Pre_OutMax;

void calibrationAllSensors(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// финальная функция: общий опрос и калибровка всех датчиков
void finalPoolAndCalibrationAllSensors(){
	poolAllSensors();
	calibrationAllSensors();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// часы: время
int realSecond;     // Переменная для хранения значения секунд
int realMinute;     // Переменная для хранения значения минут
int realHour;       // Переменная для хранения значения часов
int realDay;        // Переменная для хранения значения дня
int realDayOfWeek;  // Переменная для хранения значения дня недели
int realMonth;      // Переменная для хранения значения месяца
int realYear;       // Переменная для хранения значения года

int treckingSecond = 61;  // Переменная для отслеживания изменения секунд
int treckingMinute = 61;  // Переменная для отслеживания изменения минут
int treckingHour = 25;    // переменная для отслеживания изменения часов
int treckingDay;          // переменная для отслеживания изменения дня

void chekVremya(){
	DateTime = clock.getDateTime();  // Считываем c часов текущие значения даты и времени в сущность DateTime
	realSecond = int(DateTime.second);
	realMinute = int(DateTime.minute);  //  Меняем значение в переменной отслеживания минут на текущее
	realHour = int(DateTime.hour);
	realDay = int(DateTime.day);
	realDayOfWeek = int(DateTime.dayOfWeek);
	realMonth = int(DateTime.month);
	realYear = int(DateTime.year);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clock check. if sec != sec: generate data exchange with the slave and send to DEZEGA_RX
// clock check. if minute != minute: write data to SD

bool recordFlag = 0;  // флаг записи на флешку. если 1 то запись идет если 0 то нет

void finalCheckChangeSecondOrMinuteAndPerformAction(){
	chekVremya();

	if (realSecond != treckingSecond){    // Если значение секунд отличается от значения в переменной отслеживания
    	treckingSecond = realSecond;      //  Меняем значение в переменной отслеживания секунд на текущее
		// выполняем действие каждую секунду:
		// опрашиваем датчики внешние и внутренние, калибруем значения датчиков
		finalPoolAndCalibrationAllSensors();
		// записываем макс значения за секунду в макс значения за минуту
		// управление диодом и бузером
		// отправляем значения за секунду по радио
		// сбрасываем макс значения за секунду
	}

	if (realMinute != treckingMinute && recordFlag == 1){// Если знач минут отличается от знач в переменной отслеживания
    	treckingMinute = realMinute;      //  Меняем значение в переменной отслеживания минут на текущее
    	// выполняем действие каждую минуту:
    	// записываем макс значение за минуту на СД карту
    	// сбрасываем макс значение за минуту
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
	// Serial init
	Serial.begin(250000);
	Serial1.begin(250000);
	Serial2.begin(250000);
	delay(100);


	// ЧАСЫ: init
	Wire.begin();
	Serial.println(" clock init");
	// Инициализируем работу с объектом библиотеки DS3231
	if(!clock.begin()){
    	Serial.println("initialization failed clock not detected!");
	}else{
		Serial.println("clock init ok");
	}
	#if SET_CLOK_FOR_PROG == 1
 	clock.setDateTime(__DATE__, __TIME__);     // Устанавливаем время на часах, основываясь на времени компиляции скетча
 	Serial.println("set time ok");
	//clock.setDateTime(2016, 9, 15, 0, 0, 0);     // Установка времени вручную (Год, Месяц, День, Час, Минута, Секунда)
	#endif


	// MicroSD: init
	delay(500);
    Serial.println(" microSD init");
	if (!SD.begin(CHIPSELEKT)) {
    	Serial.println("initialization failed SD not detected!");
		delay(5000);
    	if(SDCHEK == 1){
    		while(1){
    		}
    	}
  	}else{
		Serial.print(" microSD init ok");
  	}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
	while(1){
		// опрос датчиков без записи: выполнение посекундного блока
			// прослушивание эфира на случай старта записи или перехода в режим калибровки

		// старт записи: создание файла для записи на СД карте, запись заголовков

		// запись: выполнение посекундного и поминутного блоков
			// прослушивание эфира на случай сигнала остановки записи
		
		// конец записи: закрытие файла записи на СД карте
	}
}