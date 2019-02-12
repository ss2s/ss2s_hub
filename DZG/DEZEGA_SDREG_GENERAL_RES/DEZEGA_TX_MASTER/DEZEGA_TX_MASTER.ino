
// MEGA 2560
// #    pragma message "MEGA 2560"

// #include "Arduino.h"         // Arduino lib

#include <SPI.h>                // SPI lib
#include <Wire.h>               // I2C lib
#include <EEPROM.h>             // EEPROM lib

#include <SD.h>                 // Micro SD lib

#include "HX711.h"              // ADC 24 BIT lib
#include "Adafruit_ADS1015.h"   // ads1115 ADC 16 BIT lib
#include "DS3231.h"             // ds3231 clock lib
#include "FastLED.h"            // WS2812B svetodiod lib 
#include "max6675.h"            // T2 termopara lib



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_PAS 111              // пароль для входа в сериал настройки. не больше 4 цифр. set 111
#define BAT_LVL_READ_TYPE 0      // тип считывателя уровня батареи: 0-аналоговый,  1-цифровой
#define SET_CLOK_FOR_PROG 0      // если 1 то установка часов будет при записи программы. если 0 то нет
#define SDCHEK 0                 // 1 ЕСЛИ ФЛЕШКИ НЕТ ТО НЕ СТАРТОВАТЬ. 0 СТАРТОВАТЬ В ЛЮБОМ СЛУЧАЕ
#define DEFMaxFileToSD 1000      // макс количество файлов на флешке. макс 65535

#define EEPROM_WRITE_KEY 114     // код перезаписи EEPROM < 255. если изменить то EEPROM перезапишется из оперативки
#define EEPROM_WRITE_K_ADDR 200  // адрес кода перезаписи EEPROM . 200 <= x < 500

#define BEEPER_FREQ 500  // частота аварийной пищалки в Герцах
#define BEEPER_DURATION 1200  // длительность сигнала аварийной пищалки в миллисекундах, не больше 1000

// разрешение опроса датчиков 1:0   опр:не опр
#define S_O2_ENABLE 1
#define S_T1_ENABLE 1
#define S_T2_ENABLE 1
#define S_PR_ENABLE 1
#define S_CO2_ENABLE 1
#define S_CO_ENABLE 1
#define S_BAT_ENABLE 1
// значение для возврата если сенсор запрещен
#define SENS_DISABLEREAD_VAL 3000



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА Arduino:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BAT_LEVEL_ANALOG_PIN A0  // пин подключения аналогового сигнала с батареи, если это предусмотрено в настр. выше
#define START_STOP_BUTTON_PIN 30  // кнопка включения выключения записи
#define BEEPER_PIN 4  // пин аварийной пищалки
#define LED_DATA_PIN 8 // led pin
#define CHIPSELEKT 49  // пин чип селект для флэшки: SPI 50 MISO 51 MOSI 52 SCK 49 CHIPSELEKT

// термопара t2
#define T2_TERMOPARA_SO_PIN 5
#define T2_TERMOPARA_CS_PIN 6
#define T2_TERMOPARA_SCK_PIN 7

// ADC 24 B HX711     O2 sensor ADC
#define HX711_GAIN 64  // усилитель HX711
#define HX711_DOUT_PIN A2
#define HX711_PD_SCK_PIN A3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА ads1115:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define PORT_0_O2_ADS1 0   // порт ADS1115 куда подключается датчик O2
#define PORT_1_CO2_ADS1 1  // порт ADS1115 куда подключается датчик CO2
#define PORT_2_CO_ADS1 2   // порт ADS1115 куда подключается датчик CO
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
// HX711 variables
long adc64 = 0;
float voltageO2 = 0;
// ads 1115 variables
int16_t adc1_CO2, adc2_CO;  // ads ADC read val
float multiplierADS = 0.1875F; // ADS1115 6.144V gain (16-Bit results). делитель для перевода показаний АЦП в вольты
float voltageCO2 = 0;
float voltageCO = 0;
// other
float voltageT1 = 0;
float voltagePressure = 0;
// значения сенсоров за секунду структура для хранения значений и передачи по радио
typedef struct transmiteStructure{
	float val_T1 = SENS_DISABLEREAD_VAL;
	float val_T2 = SENS_DISABLEREAD_VAL;
	float val_CCO2 = SENS_DISABLEREAD_VAL;
	float val_O2 = SENS_DISABLEREAD_VAL;
	short val_CO = SENS_DISABLEREAD_VAL;
	float val_Press_inh = SENS_DISABLEREAD_VAL;  // мин
	float val_Press_exh = SENS_DISABLEREAD_VAL;  // макс
	short minuteTest = 0;  // X>=<60  общее количество минут что идет тест
	byte val_BatteryLevel_TX = 50;  // уровень заряда батареи в %
	byte flagZapisiAndColorTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
};
transmiteStructure txStrctVal;

// txStrctVal.val_T1
// txStrctVal.val_T2
// txStrctVal.val_CCO2
// txStrctVal.val_O2
// txStrctVal.val_CO
// txStrctVal.val_Press_inh
// txStrctVal.val_Press_exh
// txStrctVal.minuteTest
// txStrctVal.val_BatteryLevel_TX
// txStrctVal.flagZapisiAndColorTransmite

// МАКС значения сенсоров за минуту
float minVal_O2 = 100;
float maxVal_T1 = -250;
float maxVal_T2 = -250;
float minVal_Press_inh = 10;  // мин
float maxVal_Press_exh = -10;  // макс
float maxVal_CCO2 = 0;
float maxVal_CO = 0;

// калибровка всех датчиков. значения: inMin, inMax, outMin, outMax
float calibr_O2_Mas[] = {1887436.8, 2726297.6, 0, 100};
float calibr_T1_Mas[] = {20, 60, 20, 60};
float calibr_T2_Mas[] = {20, 100, 20, 100};
float calibr_CO2_Mas[] = {2133, 10666.6, 0, 5};
float calibr_CO_Mas[] = {2133, 10666.6, 0, 2000};
float calibr_Press_Mas[] = {5242880, 8388608, -9.99, 9.99};  // ok

float pumpThresholdVal = -0.05;  // порог включения помпы на TX SLAVE PRESSURE
bool pumpThresholdValWriteOkFlag = 0;  // флаг успешной отправки значения запуска помпы

// флаг записи на флешку. если 1 то запись идет если 0 то нет
bool recordFlag = 0;
// номер файла для записи, подбирается автоматически
unsigned int globalFileIndex = 1;
// номер строки в файле по порядку
unsigned long NppStr = 0;

// BEEP flag для реализации посекундной пищалки = 1
bool beepFlag = 1;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dataTypes and init:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// T2 termopara init:
MAX6675 thermocouple2(T2_TERMOPARA_SCK_PIN, T2_TERMOPARA_CS_PIN, T2_TERMOPARA_SO_PIN);
// nrf init:
byte addressNRF[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
// HX711 24 BIT ADC init
HX711 hx711Obj;
// ads 1115 16 BIT ADC init:
Adafruit_ADS1115 ads;         /* Use this for the 16-BIT version */
// clock init:
DS3231 clock;                // Связываем объект clock с библиотекой DS3231
// clock dataType
RTCDateTime DateTime;        // Определяем переменную DateTime, как описанную структурой RTCDateTime
// Define the Array of leds, LED dataType 1 светодиод
CRGB iLed[1];
// microSD  dataType
File dataFile;               // переменная для работы с флэшкой
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
void eeSingleWriteOfRam(byte addrName){ // запись значений калибровки в EEPROM
	for(int i=0; i<4; i++){	
		switch (addrName){
		    case 0:
			EEPROM_float_write(0+i, calibr_T1_Mas[i]);
		    break;
		    case 1:
			EEPROM_float_write(1+i, calibr_T2_Mas[i]);
		    break;
		    case 2:
			EEPROM_float_write(2+i, calibr_CO2_Mas[i]);
		    break;
		    case 3:
			EEPROM_float_write(3+i, calibr_O2_Mas[i]);
		    break;
		    case 4:
			EEPROM_float_write(4+i, calibr_CO_Mas[i]);
		    break;
		    case 5:
			EEPROM_float_write(5+i, calibr_Press_Mas[i]);
		    break;
		    case 6:
			EEPROM_float_write(6, pumpThresholdVal);
		    break;
		}
	}
}

void eePackWrite(){ // запись всех значений калибровки в EEPROM
	for(int i=0; i<4; i++){
	    EEPROM_float_write(0+i, calibr_T1_Mas[i]);
	    EEPROM_float_write(1+i, calibr_T2_Mas[i]);
	    EEPROM_float_write(2+i, calibr_CO2_Mas[i]);
	    EEPROM_float_write(3+i, calibr_O2_Mas[i]);
	    EEPROM_float_write(4+i, calibr_CO_Mas[i]);
	    EEPROM_float_write(5+i, calibr_Press_Mas[i]);
	}
	EEPROM_float_write(6, pumpThresholdVal);
}
void eePackRead(){ // чтение всех значений калибровки из EEPROM
	for(int i=0; i<4; i++){
	    calibr_T1_Mas[i] = EEPROM_float_read(0+i);
	    calibr_T2_Mas[i] = EEPROM_float_read(1+i);
	    calibr_CO2_Mas[i] = EEPROM_float_read(2+i);
	    calibr_O2_Mas[i] = EEPROM_float_read(3+i);
	    calibr_CO_Mas[i] = EEPROM_float_read(4+i);
	    calibr_Press_Mas[i] = EEPROM_float_read(5+i);
	}
	pumpThresholdVal = EEPROM_float_read(6);
}
void eePackOneInitWrite(){ // запись всех значений калибровки в EEPROM (ОДНОРАЗОВАЯ ФУНКЦИЯ ДЛЯ ЗАПИСИ EEPROM ПРИ 1 ПРОШИВКЕ)
	if(EEPROM.read(EEPROM_WRITE_K_ADDR) == EEPROM_WRITE_KEY){return;}  // если код перезаписи совпадает то не перезаписывать
	eePackWrite();
	EEPROM.write(EEPROM_WRITE_K_ADDR, EEPROM_WRITE_KEY);
}
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

	while(SD.exists(indexToNameFileSD(globalFileIndex))){
		globalFileIndex ++;
	}

	dataString += "N 	DATE: 	    TIME: 	O2% 	Temp 1 	Temp 2 	Pres inh kPa 	Pres exh kPa 	CO2% 	CO%";
	dataString += "\n";

	dataFile = SD.open(indexToNameFileSD(globalFileIndex), FILE_WRITE);
 	if(dataFile){
 	 	dataFile.println(dataString);
 	 	Serial.println("\n START RECORDING TO SD\n");
 	}else{
 	 	Serial1.print("error START SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}
 	chekVremya();

	txStrctVal.minuteTest = 0;
}

// ФУНКЦИЯ ЗАПИСИ ИНФОРМАЦИИ В ФАЙЛ НА ФЛЭШКЕ КАЖДУЮ МИНУТУ
void dataSetToFileWHL(){
	String dataString = "";
 	NppStr ++;

	if(globalFileIndex > DEFMaxFileToSD){
		return;
	}

	dataString += String(NppStr); dataString += " 	";
	dataString += String(realYear); dataString += "."; dataString += String(realMonth); dataString += "."; dataString += String(realDay); dataString += " 	";
	dataString += String(realHour); dataString += ":"; dataString += String(realMinute); dataString += " 	";
	dataString += String(minVal_O2); dataString += " 	";
	dataString += String(maxVal_T1); dataString += " 	";
	dataString += String(maxVal_T2); dataString += " 	";
	dataString += String(minVal_Press_inh); dataString += " 	        ";
	dataString += String(maxVal_Press_exh); dataString += " 	        ";
	dataString += String(maxVal_CCO2); dataString += " 	";
	dataString += String(maxVal_CO); dataString += " 	";

 	if (dataFile){
 	 	dataFile.println(dataString);
 	 	Serial.println("\n WRITE DATA TO SD ON WHL\n");
 	}else{
 	 	Serial1.print("error WHL SD write ");
 	 	Serial1.println(indexToNameFileSD(globalFileIndex));
 	}

 	txStrctVal.minuteTest ++;  // +1 minute test
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ОПРОС ВСЕХ ДАТЧИКОВ И КАЛИБРОВКА
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// float Map
float flap(float fX, float fY = 0, float fZ = 1000, float fA = 0, float fB = 1000){
	return (fA*fX-fB*fX+fB*fY-fA*fZ)/(fY-fZ);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика O2
static float poolO2(){  // 0 - 100 (0.1) 0v - 1.6v  (9-13mV)
	// resive and convert O2 values

  	hx711Obj.power_up();
  	delay(50);
	adc64 = hx711Obj.read();
	voltageO2 = flap(adc64, -8388608, 8388608, -0.04, 0.04);  // Volt
	// txStrctVal.val_O2 = flap(voltageO2, 0.009, 0.013, 0, 100);  // %
	txStrctVal.val_O2 = adc64;
	hx711Obj.power_down();			        // put the ADC in sleep mode
	Serial.print("O2  V ");
	Serial.println(voltageO2, 7);

	return txStrctVal.val_O2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос быстрой ТЕРМОПАРЫ1 через сериал1
float poolTermoparaFast1(){  // -250 - 750 (0.1)
	unsigned long t1StrtErrTime = millis();
	char s_d = 'x';
	Serial1.print('t');
	while(1){
		if (Serial1.available() > 0) { 
			s_d = Serial1.read();
			if(s_d == 'm'){
				txStrctVal.val_T1 = Serial1.parseFloat();
				break;
			}
		}
		if(millis() - t1StrtErrTime > 100){
			Serial.println(" get T1 serial timeaut ERROR");
			break;
		}
	}
	voltageT1 = txStrctVal.val_T1 * 0.005 + 1.25;
	Serial.print("T1  V ");
	Serial.println(voltageT1, 7);

	return txStrctVal.val_T1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос медленной ТЕРМОПАРЫ2
float poolTermoparaSlow2(){  // -250 - 750 (0.25)
	txStrctVal.val_T2 = thermocouple2.readCelsius();
	return txStrctVal.val_T2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика ДАВЛЕНИЯ через сериал2
float poolPressure(){  // -9.99 - 9.99 (0.01) 50mv - 80mv
	unsigned long prStrtErrTime = millis();
	char s_d = 'x';
	if(pumpThresholdValWriteOkFlag == 0){
		Serial2.println('f');
	}
	else{
		Serial2.println('p');
	}
	while(s_d != 'i'){
		if (Serial2.available() > 0) {
			s_d = Serial2.read();
		}
		if(s_d == 'i'){
			txStrctVal.val_Press_inh = Serial2.parseInt();
		}
		if(millis() - prStrtErrTime > 100){
			Serial.println(" get Pressure serial timeaut ERROR ");
			break;
		}
	}
	while(s_d != 'e'){
		if (Serial2.available() > 0) {
			s_d = Serial2.read();
		}
		if(s_d == 'e'){
			txStrctVal.val_Press_exh = Serial2.parseInt();  // ADC
		}
		if(millis() - prStrtErrTime > 200){
			Serial.println(" get Pressure serial timeaut ERROR ");
			break;
		}
	}
	if(pumpThresholdValWriteOkFlag == 0){
		long vppv = flap(pumpThresholdVal, calibr_Press_Mas[2], calibr_Press_Mas[3], calibr_Press_Mas[0], calibr_Press_Mas[1]);
		Serial2.print('n');
		Serial2.println(vppv);
		while(s_d != 'n'){
			if (Serial2.available() > 0) {
				s_d = Serial2.read();
			}
			if(s_d == 'n'){
				vppv = Serial2.parseInt();  // PUMP val
				if(vppv == flap(pumpThresholdVal, calibr_Press_Mas[2], calibr_Press_Mas[3], calibr_Press_Mas[0], calibr_Press_Mas[1])){
					pumpThresholdValWriteOkFlag = 1;
				}
			}
			if(millis() - prStrtErrTime > 300){
				Serial.println(" get PUMP val serial timeaut ERROR ");
				break;
			}
		}
	}
	// if(txStrctVal.val_Press_inh = 8388608){Serial2.print('r');}
	voltagePressure = flap(txStrctVal.val_Press_exh, -8388608, 8388608, -0.08, 0.08);
	Serial.print("Press V ");
	Serial.println(voltagePressure, 7);
	return (txStrctVal.val_Press_inh + txStrctVal.val_Press_exh) / 2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO2
float poolCO2(){  // 0 - 5 (0.01) 0v - 4v
	// resive and convert CO2 values
	adc1_CO2 = ads.readADC_SingleEnded(PORT_1_CO2_ADS1);
	voltageCO2 = adc1_CO2 * multiplierADS / 1000.0;  // Volt
	// txStrctVal.val_CCO2 = adc1_CO2 * multiplierADS / 1000.0;  // Volt
	Serial.print("CO2 V ");
	Serial.println(voltageCO2, 7);
	// txStrctVal.val_CCO2 = flap(voltageCO2, 0.4, 2, 0, 5);          // %
	txStrctVal.val_CCO2 = adc1_CO2;
	return txStrctVal.val_CCO2;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO
static float poolCO(){  // 2000 (1) 0v - 3v
	// resive and convert CO values
	adc2_CO = ads.readADC_SingleEnded(PORT_2_CO_ADS1);
	voltageCO = adc2_CO * multiplierADS / 1000.0;  // Volt
	Serial.print("CO  V ");
	Serial.println(voltageCO, 7);
	// txStrctVal.val_CO = flap(voltageCO, 0.4, 2, 0, 2000);       // ppm
	txStrctVal.val_CO = adc2_CO;
	return txStrctVal.val_CO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос уровня заряда батареи 
float poolBatteryLevel(){
	if(BAT_LVL_READ_TYPE == 0){  // analog LVL BAT read
		int promBatLvlVal;
		promBatLvlVal = analogRead(BAT_LEVEL_ANALOG_PIN);
		promBatLvlVal = map(promBatLvlVal, 0, 1023, 0, 500);
		promBatLvlVal = constrain(promBatLvlVal, 370, 420);
		promBatLvlVal = map(promBatLvlVal, 370, 420, 0, 100);
		txStrctVal.val_BatteryLevel_TX = promBatLvlVal;
	}
	else{  // digital LVL BAT read
	}
	return txStrctVal.val_BatteryLevel_TX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// общий опрос всех датчиков
void poolAllSensors(){
	if(S_O2_ENABLE){poolO2();}
	if(S_T1_ENABLE){poolTermoparaFast1();}
	if(S_T2_ENABLE){poolTermoparaSlow2();}
	if(S_PR_ENABLE){poolPressure();}
	if(S_CO2_ENABLE){poolCO2();}
	if(S_CO_ENABLE){poolCO();}
	if(S_BAT_ENABLE){poolBatteryLevel();}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// калибровка всех датчиков
void calibrationAllSensors(){
	if(S_CO_ENABLE){txStrctVal.val_CO = flap(txStrctVal.val_CO, calibr_CO_Mas[0], calibr_CO_Mas[1], calibr_CO_Mas[2], calibr_CO_Mas[3]);}
	if(S_T1_ENABLE){txStrctVal.val_T1 = flap(txStrctVal.val_T1, calibr_T1_Mas[0], calibr_T1_Mas[1], calibr_T1_Mas[2], calibr_T1_Mas[3]);}
	if(S_T2_ENABLE){txStrctVal.val_T2 = flap(txStrctVal.val_T2, calibr_T2_Mas[0], calibr_T2_Mas[1], calibr_T2_Mas[2], calibr_T2_Mas[3]);}
	if(S_PR_ENABLE){
		txStrctVal.val_Press_inh = flap(txStrctVal.val_Press_inh, calibr_Press_Mas[0], calibr_Press_Mas[1], calibr_Press_Mas[2], calibr_Press_Mas[3]);
		txStrctVal.val_Press_exh = flap(txStrctVal.val_Press_exh, calibr_Press_Mas[0], calibr_Press_Mas[1], calibr_Press_Mas[2], calibr_Press_Mas[3]);}
	if(S_CO2_ENABLE){txStrctVal.val_CCO2 = flap(txStrctVal.val_CCO2, calibr_CO2_Mas[0], calibr_CO2_Mas[1], calibr_CO2_Mas[2], calibr_CO2_Mas[3]);}
	if(S_O2_ENABLE){txStrctVal.val_O2 = flap(txStrctVal.val_O2, calibr_O2_Mas[0], calibr_O2_Mas[1], calibr_O2_Mas[2], calibr_O2_Mas[3]);}
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

	// ВЫПОЛНЯЕМ ДЕЙСТВИЕ КАЖДУЮ СЕКУНДУ:
	if (realSecond != treckingSecond){    // Если значение секунд отличается от значения в переменной отслеживания
    	treckingSecond = realSecond;      //  Меняем значение в переменной отслеживания секунд на текущее

		// опрашиваем датчики и калибруем значения датчиков
		finalPoolAndCalibrationAllSensors();

		// записываем макс значения за секунду в макс значения за минуту
		if(minVal_O2 > txStrctVal.val_O2 && recordFlag){minVal_O2 = txStrctVal.val_O2;}
		if(maxVal_T1 < txStrctVal.val_T1 && recordFlag){maxVal_T1 = txStrctVal.val_T1;}
		if(maxVal_T2 < txStrctVal.val_T2 && recordFlag){maxVal_T2 = txStrctVal.val_T2;}
		if(minVal_Press_inh > txStrctVal.val_Press_inh && recordFlag){minVal_Press_inh = txStrctVal.val_Press_inh;}
		if(maxVal_Press_exh < txStrctVal.val_Press_exh && recordFlag){maxVal_Press_exh = txStrctVal.val_Press_exh;}
		if(maxVal_CCO2 < txStrctVal.val_CCO2 && recordFlag){maxVal_CCO2 = txStrctVal.val_CCO2;}
		if(maxVal_CO < txStrctVal.val_CO && recordFlag){maxVal_CO = txStrctVal.val_CO;}

		// управление диодом и бузером
		if(recordFlag == 0){ // BLUE
			txStrctVal.flagZapisiAndColorTransmite = 0;  // rec 0
			iLed[0] = CRGB(0, 0, 255);  // GRB blue
			FastLED.show();
			digitalWrite(BEEPER_PIN, HIGH);
		}
		else if(recordFlag && (txStrctVal.val_CCO2 >= 5 || txStrctVal.val_CO >= 170 || txStrctVal.val_O2 < 17)){ // RED, SIGNAL
			txStrctVal.flagZapisiAndColorTransmite = 3;  // rec 1
			iLed[0] = CRGB(0, 255, 0);  // GRB red
			FastLED.show();
			if(beepFlag == 1){
				tone(BEEPER_PIN, BEEPER_FREQ, BEEPER_DURATION);
				// tone(BEEPER_PIN, 500, 500);
			}else{
				noTone(BEEPER_PIN);
				digitalWrite(BEEPER_PIN, HIGH);
				// tone(BEEPER_PIN, 500, 500);
			}
			beepFlag = !beepFlag;
		}
		else if(recordFlag && (txStrctVal.val_CCO2 >= 3 || txStrctVal.val_CO >= 26 || txStrctVal.val_O2 < 21)){ // YELOW
			txStrctVal.flagZapisiAndColorTransmite = 2;  // rec 1
			iLed[0] = CRGB(255, 255, 0);  // GRB yelow
			FastLED.show();
			digitalWrite(BEEPER_PIN, HIGH);
		}
		else if(recordFlag && (txStrctVal.val_CCO2 < 3 && txStrctVal.val_CO < 26 && txStrctVal.val_O2 >= 21)){ // GREEN
			txStrctVal.flagZapisiAndColorTransmite = 1;  // rec 1
			iLed[0] = CRGB(255, 0, 0);  // GRB green
			FastLED.show();
			digitalWrite(BEEPER_PIN, HIGH);
		}

		// отправляем значения за секунду по сериал3 радио
		Serial3.println("snd");
		Serial3.print(" "); Serial3.print(txStrctVal.val_T1);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.val_T2);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.val_CCO2);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.val_O2);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.val_CO);  // short
		Serial3.print(" "); Serial3.print(txStrctVal.val_Press_inh);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.val_Press_exh);  // float
		Serial3.print(" "); Serial3.print(txStrctVal.minuteTest);  // short
		Serial3.print(" "); Serial3.print(txStrctVal.val_BatteryLevel_TX);  // byte
		Serial3.print(" "); Serial3.print(txStrctVal.flagZapisiAndColorTransmite);  // byte

		Serial.println("\nserial radio write per sec\n");

		// send record flag to serial
		Serial.print("record flag = ");
		Serial.println(recordFlag);

		// выводим значения в сериал
		Serial.println("DATE:    TIME:      O2%    Temp 1    Temp 2    Pres inh    Pres exh    CO2    CO%");
		Serial.print(realDay); Serial.print("."); Serial.print(realMonth); Serial.print("      ");
		Serial.print(realHour); Serial.print(":"); Serial.print(realMinute); Serial.print(":"); Serial.print(realSecond); Serial.print("    ");
		Serial.print(txStrctVal.val_O2); Serial.print("   ");
		Serial.print(txStrctVal.val_T1); Serial.print("     ");
		Serial.print(txStrctVal.val_T2); Serial.print("     ");
		Serial.print(txStrctVal.val_Press_inh); Serial.print("       ");
		Serial.print(txStrctVal.val_Press_exh); Serial.print("       ");
		Serial.print(txStrctVal.val_CCO2); Serial.print("   ");
		Serial.print(txStrctVal.val_CO);  // Serial.print("    ");
		Serial.println("\n");

		// сбрасываем макс значения за секунду
		txStrctVal.val_O2 = 100;
		txStrctVal.val_T1 = -250;
		txStrctVal.val_T2 = -250;
		txStrctVal.val_Press_inh = 10;
		txStrctVal.val_Press_exh = -10;
		txStrctVal.val_CCO2 = 0;
		txStrctVal.val_CO = 0;

	    // ВЫПОЛНЯЕМ ДЕЙСТВИЕ КАЖДУЮ МИНУТУ:
		if (realMinute != treckingMinute && recordFlag == 1){// Если знач минут отличается от знач в переменной отслежив
	    	treckingMinute = realMinute;      //  Меняем значение в переменной отслеживания минут на текущее

	    	// записываем макс значение за минуту на СД карту
	    	dataSetToFileWHL();

	    	// сбрасываем макс значение за минуту
	    	minVal_O2 = 100;
			maxVal_T1 = -250;
			maxVal_T2 = -250;
			minVal_Press_inh = 10;
			maxVal_Press_exh = -10;
			maxVal_CCO2 = 0;
			maxVal_CO = 0;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// START STOP RECORDING BUTTON:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void changeRecOperatingF(){  // функция отслеживания и изменения состояния записи
	iLed[0] = CRGB(255, 255, 255);  // GRB white
	FastLED.show();
	noTone(BEEPER_PIN);
	digitalWrite(BEEPER_PIN, HIGH);
	if(recordFlag == 0){
		recordFlag = 1;
		Serial.println();
		Serial.println("changed REC");
		dataSetToFileSTRT();
		Serial.println("start REC OK");
		Serial.println();
	}
	else{
		recordFlag = 0;
		Serial.println();
		Serial.println("changed REC");
		dataFile.close();
		Serial.println("stop REC OK");
		Serial.println();
	}
	digitalWrite(BEEPER_PIN, LOW);
	delay(100);
	digitalWrite(BEEPER_PIN, HIGH);
	delay(100);
	digitalWrite(BEEPER_PIN, LOW);
	delay(100);
	digitalWrite(BEEPER_PIN, HIGH);
	delay(100);
	digitalWrite(BEEPER_PIN, LOW);
	delay(100);
	digitalWrite(BEEPER_PIN, HIGH);
	delay(100);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void buttonChangeREC(){
	if(digitalRead(START_STOP_BUTTON_PIN)){  // если кнопка нажата
		delay(50);
		if(digitalRead(START_STOP_BUTTON_PIN) == HIGH){
			Serial.println("button pressed");
			changeRecOperatingF();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// serial3 NRF ПРИЕМ ДАННЫХ СТАРТ СТОП:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void srf3changeButChek(){
	if(Serial3.available() > 0){
		char s_d = 'y';
		String s_str = "";
		for(int i=0; i<3; i++){
		    s_d = Serial3.read();
		    s_str += s_d;
		}
		if(s_str == "bch"){
			Serial.println("NRF button pressed");
			changeRecOperatingF();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// serial calibration F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serCalibrator(){
	if(Serial.available() > 0){
		unsigned long scStrtErrTime = millis();
		char s_d = 'y';
		String s_str = "";
		int s_pint;
		float s_pfcv;
		for(int i=0; i<3; i++){
		    s_d = Serial.read();
		    s_str += s_d;
		}
		s_pint = Serial.parseInt();
		if(s_str == "set" && s_pint == SET_PAS){  // ВХОД
			while(Serial.available() > 0){Serial.read();}

		    if(dataFile){dataFile.close();Serial.println("\n\nstop recording");}  // отключить запись на флешку

		    Serial.println("\n\n\n");Serial.println();Serial.print(s_str);Serial.println(" OK");Serial.println(" setting ok:\n");
		    while(1){
		    	if(Serial.available() > 0){
		    		String s_str = "";
		    		for(int i=0; i<3; i++){
				    	s_d = Serial.read();
				    	s_str += s_d;
					}
		    		Serial.println(s_str);

					if(s_str[2] == 'n' || s_str[2] == 'x' || s_str[2] == 'v'){s_pfcv = Serial.parseFloat();}

					if(s_str == "end"){  // ВЫХОД
						Serial.println();Serial.print(s_str);Serial.println(" OK");
						break;
					}

					else if(s_str == "ton"){  // T1 MIN
						poolTermoparaFast1();delay(100);calibr_T1_Mas[0] = poolTermoparaFast1();calibr_T1_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "tox"){  // T1 MAX
						poolTermoparaFast1();delay(100);calibr_T1_Mas[1] = poolTermoparaFast1();calibr_T1_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "ttn"){  // T2 MIN
						calibr_T2_Mas[0] = poolTermoparaSlow2();calibr_T2_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "ttx"){  // T2 MAX
						calibr_T2_Mas[1] = poolTermoparaSlow2(); calibr_T2_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "ctn"){  // CO2 MIN
						calibr_CO2_Mas[0] = poolCO2();calibr_CO2_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "ctx"){  // CO2 MAX
						calibr_CO2_Mas[1] = poolCO2();calibr_CO2_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "con"){  // CO MIN
						calibr_CO_Mas[0] = poolCO();calibr_CO_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "cox"){  // CO MAX
						calibr_CO_Mas[1] = poolCO();calibr_CO_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "otn"){  // O2 MIN
						calibr_O2_Mas[0] = poolO2();calibr_O2_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "otx"){  // O2 MAX
						calibr_O2_Mas[1] = poolO2();calibr_O2_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "prn"){  // Press MIN
						poolPressure();delay(100);calibr_Press_Mas[0] = poolPressure();calibr_Press_Mas[2] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "prx"){  // Press MAX
						poolPressure();delay(100);calibr_Press_Mas[1] = poolPressure();calibr_Press_Mas[3] = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");
					}
					else if(s_str == "psv"){  // PUMP start val
						pumpThresholdVal = s_pfcv;Serial.println();Serial.print(s_str);Serial.println(" OK");pumpThresholdValWriteOkFlag = 0;
					}

					else if(s_str == "tms"){  // date time:  tms dy dm dd th tm ts
						int ryear = Serial.parseInt();
						int rmonth = Serial.parseInt();
						int rday = Serial.parseInt();
						int rhour = Serial.parseInt();
						int rrminute = Serial.parseInt();
						int rsecond = Serial.parseInt();
						clock.setDateTime(ryear, rmonth, rday, rhour, rrminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
						Serial.println();Serial.print(s_str);Serial.println(" OK");
					}

					else if(s_str == "tst"){  // test
						Serial.print(s_str);Serial.println(" OK");
					}
		    	}

		    	if(millis() - scStrtErrTime > 300000){  // авто выход из настроек через 5 minut
					Serial.println(" setting timeout");
					break;
				}
		    }
		    eePackWrite();
		    Serial.println("\nsetting end\n\n");
		}
		else if(s_str == "rec"){  // changed rec state
			Serial.println("uart changed rec");
			changeRecOperatingF();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

	pinMode(START_STOP_BUTTON_PIN, INPUT);
	pinMode(BEEPER_PIN, OUTPUT);
	digitalWrite(BEEPER_PIN, HIGH);

	// serial init
	Serial.begin(250000);    // serial
	Serial1.begin(250000);   // T1 sensor
	Serial2.begin(250000);   // Pressure sensor
	Serial3.begin(250000);   // nrf tranciever
	delay(50);
	Serial.println(" START SETUP");

	FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(iLed, 1); // 1 светодиод WS2812B
	iLed[0] = CRGB(255, 255, 255);  // GRB white
	FastLED.show();

	delay(50);

	eePackOneInitWrite();  // 1st EEPROM write all calibration val
	eePackRead();   // EEPROM read all calibration val

	// ЧАСЫ: init
	Wire.begin();
	Serial.println(" clock init...");
	// Инициализируем работу с объектом библиотеки DS3231
	if(!clock.begin()){
    	Serial.println(" initialization failed clock not detected!");
	}else{
		Serial.println(" clock init ok");
	}
	#if SET_CLOK_FOR_PROG == 1
 	clock.setDateTime(__DATE__, __TIME__);     // Устанавливаем время на часах, основываясь на времени компиляции скетча
 	Serial.println(" set time ok");
	//clock.setDateTime(2016, 9, 15, 0, 0, 0);     // Установка времени вручную (Год, Месяц, День, Час, Минута, Секунда)
	#endif
	Serial.println();


	// MicroSD: init
	delay(500);
    Serial.println(" microSD init...");
	if (!SD.begin(CHIPSELEKT)) {
    	Serial.println(" initialization failed SD not detected!");
		delay(5000);
    	if(SDCHEK == 1){
    		while(1){
    		}
    	}
  	}else{
		Serial.println(" microSD init ok");
  	}

  	// HX711 setup
  	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN, HX711_GAIN);

  	// ADS1115 setup
  	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	ads.begin();

	delay(400);
	
	Serial.println();
	Serial.println(" END SETUP. ALL SETUP OK");
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
	while(1){
		// опрос датчиков без записи: выполнение посекундно || запись: выполнение посекундного и поминутного блоков
		finalCheckChangeSecondOrMinuteAndPerformAction();

		// отслеживание кнопки на случай старта стопа записи
		buttonChangeREC();//старт стоп записи: созд файла для записи на СД карте, запись заголовков || закрытие файла

		// прослушивание эфира на случай старта стопа записи serial3 nrf
		srf3changeButChek();

		// прослушивание сериал порта
		serCalibrator();
	}
}
