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
#define SET_CLOK_FOR_PROG 0      // если 1 то установка часов будет при записи программы. если 0 то нет
#define SDCHEK 1                 // 1 ЕСЛИ ФЛЕШКИ НЕТ ТО НЕ СТАРТОВАТЬ. 0 СТАРТОВАТЬ В ЛЮБОМ СЛУЧАЕ
#define DEFMaxFileToSD 1000      // макс количество файлов на флешке. макс 65535
#define EEPROM_WRITE_KEY 123     // код перезаписи EEPROM < 255. если изменить то EEPROM перезапишется из оперативки
#define EEPROM_WRITE_K_ADDR 200  // адрес кода перезаписи EEPROM . 200 <= x < 500
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
// значения сенсоров за секунду
float val_O2 = 0;
float val_T1 = 0;
float val_T2 = 0;
float val_Press_inh = 0;  // мин
float val_Press_exh = 0;  // макс
float val_CO2 = 0;
float val_CO = 0;
// МАКС значения сенсоров за минуту
float minVal_O2 = 0;
float maxVal_T1 = 0;
float maxVal_T2 = 0;
float maxVal_Press_inh = 0;  // мин
float maxVal_Press_exh = 0;  // макс
float maxVal_CO2 = 0;
float maxVal_CO = 0;
// уровень заряда батареи в %
byte val_BatteryLevel_TX = 50;
// калибровка всех датчиков. значения: inMin, outMin, inMax, outMax
float calibr_T1_Mas[] = {0, 1, 0, 10};
float calibr_T2_Mas[] = {0, 1, 0, 10};
float calibr_CO2_Mas[] = {0, 1, 0, 10};
float calibr_O2_Mas[] = {0, 1, 0, 10};
float calibr_CO_Mas[] = {0, 1, 0, 10};
float calibr_Press_Mas[] = {0, 1, 0, 10};

// флаг записи на флешку. если 1 то запись идет если 0 то нет
bool recordFlag = 0;
// номер файла для записи, подбирается автоматически
unsigned int globalFileIndex = 1;
// номер строки в файле по порядку
unsigned long NppStr = 0;
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
// EEPROM read write calibration F res
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
	if (EEPROM_float_read(addr) == data){return;}  // если сохраняемое неотличается от сохраненного, не сохранять
	byte raw[4];
	(float&)raw = data;
	for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// запись/чтение калибровочных значений из/в EEPROM
void eeSingleWriteOfRam(byte addrName){ // калибровка и запись значений калибровки в EEPROM
	for(int i=0; i<4; i++){	
		switch (addrName){
		    case 0:
			EEPROM_float_write(0+i, calibr_T1_Mas[i]);
		    break;
		    case 1:
			EEPROM_float_write(4+i, calibr_T2_Mas[i]);
		    break;
		    case 2:
			EEPROM_float_write(8+i, calibr_CO2_Mas[i]);
		    break;
		    case 3:
			EEPROM_float_write(12+i, calibr_O2_Mas[i]);
		    break;
		    case 4:
			EEPROM_float_write(16+i, calibr_CO_Mas[i]);
		    break;
		    case 5:
			EEPROM_float_write(20+i, calibr_Press_Mas[i]);
		    break;
		}
	}
}

void eePackWrite(){ // запись всех значений калибровки в EEPROM (ОДНОРАЗОВАЯ ФУНКЦИЯ ДЛЯ ЗАПИСИ EEPROM ПРИ 1 ПРОШИВКЕ)
	if(EEPROM.read(EEPROM_WRITE_K_ADDR) == EEPROM_WRITE_KEY){return;}  // если код совпадает то не перезаписывать
	for(int i=0; i<4; i++){
	    EEPROM_float_write(0+i, calibr_T1_Mas[i]);
	    EEPROM_float_write(4+i, calibr_T2_Mas[i]);
	    EEPROM_float_write(8+i, calibr_CO2_Mas[i]);
	    EEPROM_float_write(12+i, calibr_O2_Mas[i]);
	    EEPROM_float_write(16+i, calibr_CO_Mas[i]);
	    EEPROM_float_write(20+i, calibr_Press_Mas[i]);
	}
	EEPROM.write(EEPROM_WRITE_K_ADDR, EEPROM_WRITE_KEY);
}
void eePackRead(){ // чтение всех значений калибровки из EEPROM
	for(int i=0; i<4; i++){
	    calibr_T1_Mas[i] = EEPROM_float_read(0+i);
	    calibr_T2_Mas[i] = EEPROM_float_read(4+i);
	    calibr_CO2_Mas[i] = EEPROM_float_read(8+i);
	    calibr_O2_Mas[i] = EEPROM_float_read(12+i);
	    calibr_CO_Mas[i] = EEPROM_float_read(16+i);
	    calibr_Press_Mas[i] = EEPROM_float_read(20+i);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALIBRATION FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// microSD write F. функция записи на микро СД
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
 	NppStr = 0;

	if(globalFileIndex > DEFMaxFileToSD){
		return;
	}

	while(!SD.exists(indexToNameFileSD(globalFileIndex))){
		globalFileIndex ++;
	}

	dataString += "N 	DATE: 	TIME: 	O2% 	Temp 1 	Temp 2 	Pres inh kPa 	Pres exh kPa 	CO2% 	O2%";
	dataString += "\n";

	dataFile = SD.open(indexToNameFileSD(globalFileIndex), FILE_WRITE);
 	if(dataFile){
 	 	dataFile.println(dataString);
 	}else{
 	 	Serial1.print("error SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}

}

// ФУНКЦИЯ ЗАПИСИ ИНФОРМАЦИИ В ФАЙЛ НА ФЛЭШКЕ КАЖДУЮ МИНУТУ
void dataSetToFileWHL(){
	String dataString = "";
 	NppStr ++;

	if(globalFileIndex > DEFMaxFileToSD){
		return;
	}

	dataString += String(NppStr);
	dataString += " 	";
	dataString += String(realYear); dataString += ".";
	dataString += String(realMonth); dataString += ".";
	dataString += String(realDay);
	dataString += " 	";
	dataString += String(realHour); dataString += ":";
	dataString += String(realMinute);
	dataString += " 	";
	dataString += String(minVal_O2);
	dataString += " 	";
	dataString += String(maxVal_T1);
	dataString += " 	";
	dataString += String(maxVal_T2);
	dataString += " 	";
	dataString += String(maxVal_Press_inh);
	dataString += " 	";
	dataString += String(maxVal_Press_exh);
	dataString += " 	";
	dataString += String(maxVal_CO2);
	dataString += " 	";
	dataString += String(maxVal_CO);
	dataString += " 	";

 	if (dataFile){
 	 	dataFile.println(dataString);
 	}else{
 	 	Serial1.print("error SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}

 	//if(recordFlag == 0){dataFile.close();}
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
	val_Press_inh = 1;
	val_Press_exh = 1;
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
	val_Press_inh = flap(val_Press_inh, calibr_Press_Mas[0], calibr_Press_Mas[1], calibr_Press_Mas[2], calibr_Press_Mas[3]);
	val_Press_exh = flap(val_Press_exh, calibr_Press_Mas[0], calibr_Press_Mas[1], calibr_Press_Mas[2], calibr_Press_Mas[3]);
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
	realMinute = int(DateTime.minute);
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
		if(minVal_O2 > val_O2 && recordFlag){minVal_O2 = val_O2;}
		if(maxVal_T1 < val_T1 && recordFlag){maxVal_T1 = val_T1;}
		if(maxVal_T2 < val_T2 && recordFlag){maxVal_T2 = val_T2;}
		if(maxVal_Press_inh > val_Press_inh && recordFlag){maxVal_Press_inh = val_Press_inh;}
		if(maxVal_Press_exh < val_Press_exh && recordFlag){maxVal_Press_exh = val_Press_exh;}
		if(maxVal_CO2 < val_CO2 && recordFlag){maxVal_CO2 = val_CO2;}
		if(maxVal_CO < val_CO && recordFlag){maxVal_CO = val_CO;}

		// управление диодом и бузером
		if(recordFlag == 0){ // BLUE
		}
		else if(recordFlag && val_CO2 >= 3 || val_CO >= 170 || val_O2 < 17){ // RED, SIGNAL
		}
		else if(recordFlag && val_CO2 < 3 && val_CO < 170 && val_O2 < 21){ // YELOW
		}
		else if(recordFlag && val_CO2 < 3 && val_CO < 170 && val_O2 >= 21){ // GREEN
		}

		// отправляем значения за секунду по радио

		// сбрасываем макс значения за секунду
		val_O2 = 0;
		val_T1 = 0;
		val_T2 = 0;
		val_Press_inh = 0;
		val_Press_exh = 0;
		val_CO2 = 0;
		val_CO = 0;

	    // выполняем действие каждую минуту:
		if (realMinute != treckingMinute && recordFlag == 1){// Если знач минут отличается от знач в переменной отслежив
	    	treckingMinute = realMinute;      //  Меняем значение в переменной отслеживания минут на текущее

	    	// записываем макс значение за минуту на СД карту
	    	dataSetToFileWHL();

	    	// сбрасываем макс значение за минуту
	    	minVal_O2 = 0;
			maxVal_T1 = 0;
			maxVal_T2 = 0;
			maxVal_Press_inh = 0;
			maxVal_Press_exh = 0;
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
				Serial.println("start REC");
			}
			else{
				recordFlag = 0;
				dataFile.close();
				Serial.println("stop REC");
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

	eePackWrite();
	eePackRead();

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