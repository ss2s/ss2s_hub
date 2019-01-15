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
#define START_STOP_BUTTON_PIN 30  // кнопка включения выключения записи. замыкать на землю
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_CLOK_FOR_PROG 0  // если 1 то установка часов будет при записи программы. если 0 то нет
#define SDCHEK 1  // 1 ЕСЛИ ФЛЕШКИ НЕТ ТО НЕ СТАРТОВАТЬ. 0 СТАРТОВАТЬ В ЛЮБОМ СЛУЧАЕ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// часы: время текущее
int realSecond;     // Переменная для хранения значения секунд
int realMinute;     // Переменная для хранения значения минут
int realHour;       // Переменная для хранения значения часов
int realDay;        // Переменная для хранения значения дня
int realMonth;      // Переменная для хранения значения месяца
int realYear;       // Переменная для хранения значения года
// часы: время отслеживания
int treckingSecond = 61;  // Переменная для отслеживания изменения секунд
int treckingMinute = 61;  // Переменная для отслеживания изменения минут
int treckingHour = 25;    // переменная для отслеживания изменения часов
int treckingDay;          // переменная для отслеживания изменения дня
// значения сенсоров
float val_O2 = 0;
float val_T1 = 0;
float val_T2 = 0;
float val_Press = 0;
float val_CO2 = 0;
float val_CO = 0;
// МАКС значения сенсоров за минуту
float maxVal_O2 = 0;
float maxVal_T1 = 0;
float maxVal_T2 = 0;
float maxVal_Press = 0;
float maxVal_CO2 = 0;
float maxVal_CO = 0;
// уровень заряда батареи
byte val_BatteryLevel_TX = 50;
// калибровка всех датчиков. значения
float calibr_T1_Mas[] = {0, 1, 0, 10};
float calibr_T2_Mas[] = {0, 1, 0, 10};
float calibr_CO2_Mas[] = {0, 1, 0, 10};
float calibr_O2_Mas[] = {0, 1, 0, 10};
float calibr_CO_Mas[] = {0, 1, 0, 10};
float calibr_Press_Mas[] = {0, 1, 0, 10};
// флаг записи на флешку. если 1 то запись идет если 0 то нет
bool recordFlag = 0;
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// чтение из EEPROM 4 байта float
float EEPROM_float_read(int addr){  
	addr *= 4;  
	byte raw[4];
	for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
	float &data = (float&)raw;
	return data;
}
// запись в EEPROM 4 байта float
void EEPROM_float_write(int addr, float data){
	addr *= 4;
	byte raw[4];
	(float&)raw = data;
	for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// запись/чтение калибровочных значений из/в EEPROM
void eePackWrite(){ // запись значений калибровки в EEPROM

}
void eePackRead(){ // чтение значений калибровки из EEPROM

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// microSD write F. функция записи на микро СД
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DEFMaxFileToSD 1000
unsigned int globalFileIndex = 1;
unsigned long Npp = 0;

// ФУНКЦИЯ СОЗДАНИЯ НАЗВАНИЯ ФАЙЛА ПО ИНДЕКСУ
String indexToNameFileSD(unsigned int _index){
	unsigned int index = _index;
	String name = "";
	name = "f";
	name += String(index);
	name += ".txt";
	return name;
}

// ФУНКЦИЯ ЗАПИСИ ИНФОРМАЦИИ В ФАЙЛ НА ФЛЭШКЕ ПРИ СТАРТЕ
void dataSetToFileSTRT(){
	String dataString = "";
 	Npp = 0;

	if(globalFileIndex > DEFMaxFileToSD){
		return;
	}

	while(!SD.exists(indexToNameFileSD(globalFileIndex))){
		globalFileIndex ++;
	}

	dataString += "N 	DATE: 	TIME: 	O2% 	Temp 1 	Temp 2 	Pres kPa 	CO2% 	O2% 	BAT%";
	dataString += "\n";

	dataFile = SD.open(indexToNameFileSD(globalFileIndex), FILE_WRITE);
 	if(dataFile){
 	 	dataFile.println(dataString);
 	}else{
 	 	Serial1.print("error SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}

}

// ФУНКЦИЯ ЗАПИСИ ИНФОРМАЦИИ ИЗ ОПЕРАТИВНОЙ ПАМЯТИ В ФАЙЛ НА ФЛЭШКЕ КАЖДУЮ МИНУТУ
void dataSetToFileWHL(){
	String dataString = "";
 	Npp ++;

	if(globalFileIndex > DEFMaxFileToSD){
		return;
	}

	dataString += String(Npp);
	dataString += " 	";
	dataString += String(realYear); dataString += ".";
	dataString += String(realMonth); dataString += ".";
	dataString += String(realDay);
	dataString += " 	";
	dataString += String(realHour); dataString += ":";
	dataString += String(realMinute);
	dataString += " 	";
	dataString += String(maxVal_O2);
	dataString += " 	";
	dataString += String(maxVal_T1);
	dataString += " 	";
	dataString += String(maxVal_T2);
	dataString += " 	";
	dataString += String(maxVal_Press);
	dataString += " 	";
	dataString += String(maxVal_CO2);
	dataString += " 	";
	dataString += String(maxVal_CO);
	dataString += " 	";
	dataString += String(val_BatteryLevel_TX);
	dataString += "\n";

 	if (dataFile){
 	 	dataFile.println(dataString);
 	}else{
 	 	Serial1.print("error SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}

 	if(recordFlag == 0){dataFile.close();}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ОПРОС ВСЕХ ДАТЧИКОВ И КАЛИБРОВКА
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика O2
inline void poolO2(){
	val_O2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос быстрой ТЕРМОПАРЫ1 через сериал1
inline void poolTermoparaFast1(){
	val_T1 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос медленной ТЕРМОПАРЫ2
inline void poolTermoparaSlow2(){
	val_T2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика ДАВЛЕНИЯ через сериал2
inline void poolPressure(){
	val_Press = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO2
inline void poolCO2(){
	val_CO2 = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO
inline void poolCO(){
	val_CO = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос уровня заряда батареи
inline void poolBatteryLevel(){
	val_BatteryLevel_TX = 51;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// общий опрос всех датчиков
void poolAllSensors(){
	poolO2();
	poolTermoparaFast1();
	poolTermoparaSlow2();
	poolPressure();
	poolCO2();
	poolCO();
	poolBatteryLevel();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// калибровка всех датчиков

// float Map
float flap(float fX, float fY = 0, float fZ = 1, float fA = 0, float fB = 1){
	return (fA*fX-fB*fX+fB*fY-fA*fZ)/(fY-fZ);
}

// калибровка всех сенсоров
void calibrationAllSensors(){
	val_CO = flap(val_CO, calibr_CO_Mas[0], calibr_CO_Mas[1], calibr_CO_Mas[2], calibr_CO_Mas[3]);
	val_T1 = flap(val_T1, calibr_T1_Mas[0], calibr_T1_Mas[1], calibr_T1_Mas[2], calibr_T1_Mas[3]);
	val_T2 = flap(val_T2, calibr_T2_Mas[0], calibr_T2_Mas[1], calibr_T2_Mas[2], calibr_T2_Mas[3]);
	val_Press = flap(val_Press, calibr_Press_Mas[0], calibr_Press_Mas[1], calibr_Press_Mas[2], calibr_Press_Mas[3]);
	val_CO2 = flap(val_CO2, calibr_CO2_Mas[0], calibr_CO2_Mas[1], calibr_CO2_Mas[2], calibr_CO2_Mas[3]);
	val_O2 = flap(val_O2, calibr_O2_Mas[0], calibr_O2_Mas[1], calibr_O2_Mas[2], calibr_O2_Mas[3]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// финальная функция: общий опрос и калибровка всех датчиков
void finalPoolAndCalibrationAllSensors(){
	poolAllSensors();
	calibrationAllSensors();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// часы: время

void chekVremya(){
	DateTime = clock.getDateTime();  // Считываем c часов текущие значения даты и времени в сущность DateTime
	realSecond = int(DateTime.second);
	realMinute = int(DateTime.minute);  //  Меняем значение в переменной отслеживания минут на текущее
	realHour = int(DateTime.hour);
	realDay = int(DateTime.day);
	realMonth = int(DateTime.month);
	realYear = int(DateTime.year);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// clock check. if sec != treckSec:
void finalCheckChangeSecondOrMinuteAndPerformAction(){
	chekVremya();

	// выполняем действие каждую секунду:
	if (realSecond != treckingSecond){    // Если значение секунд отличается от значения в переменной отслеживания
    	treckingSecond = realSecond;      //  Меняем значение в переменной отслеживания секунд на текущее

		// опрашиваем датчики, калибруем значения датчиков
		finalPoolAndCalibrationAllSensors();

		// записываем макс значения за секунду в макс значения за минуту
		if(maxVal_O2 < val_O2 && recordFlag){maxVal_O2 = val_O2;}
		if(maxVal_T1 < val_T1 && recordFlag){maxVal_T1 = val_T1;}
		if(maxVal_T2 < val_T2 && recordFlag){maxVal_T2 = val_T2;}
		if(maxVal_Press < val_Press && recordFlag){maxVal_Press = val_Press;}
		if(maxVal_CO2 < val_CO2 && recordFlag){maxVal_CO2 = val_CO2;}
		if(maxVal_CO < val_CO && recordFlag){maxVal_CO = val_CO;}

		// управление диодом и бузером
		// отправляем значения за секунду по радио

		// сбрасываем макс значения за секунду
		val_O2 = 0;
		val_T1 = 0;
		val_T2 = 0;
		val_Press = 0;
		val_CO2 = 0;
		val_CO = 0;

	    // выполняем действие каждую минуту:
		if (realMinute != treckingMinute && recordFlag == 1){// Если знач минут отличается от знач в переменной отслежив
	    	treckingMinute = realMinute;      //  Меняем значение в переменной отслеживания минут на текущее

	    	// записываем макс значение за минуту на СД карту
	    	dataSetToFileWHL();

	    	// сбрасываем макс значение за минуту
	    	maxVal_O2 = 0;
			maxVal_T1 = 0;
			maxVal_T2 = 0;
			maxVal_Press = 0;
			maxVal_CO2 = 0;
			maxVal_CO = 0;

		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// START STOP RECORDING BUTTON:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool buttonChangeREC(){
	if(!digitalRead(START_STOP_BUTTON_PIN)){  // если кнопка нажата
		delay(50);
		if(!digitalRead(START_STOP_BUTTON_PIN)){
			if(recordFlag == 0){
				recordFlag = 1;
				delay(500);
				dataSetToFileSTRT();
			}
			else{
				recordFlag = 0;
				dataFile.close();
				delay(500);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

	pinMode(START_STOP_BUTTON_PIN, INPUT_PULLUP);

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
		// опрос датчиков без записи: выполнение посекундно || запись: выполнение посекундного и поминутного блоков
		finalCheckChangeSecondOrMinuteAndPerformAction();

		// отслеживание кнопки на случай старта стопа записи
		buttonChangeREC();//старт стоп записи: созд файла для записи на СД карте, запись заголовков || закрытие файла

		// прослушивание эфира на случай старта стопа записи или перехода в режим калибровки
	}
}