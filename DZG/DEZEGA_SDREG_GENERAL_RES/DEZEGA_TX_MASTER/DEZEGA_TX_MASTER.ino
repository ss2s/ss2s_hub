
// MEGA 2560
// #    pragma message "SS2S MEGA 2560"  // SS2S

#include "Arduino.h"            // Arduino lib
#include <SPI.h>                // SPI lib
#include <Wire.h>               // I2C lib
#include <EEPROM.h>             // EEPROM lib

#include "Adafruit_ADS1015.h"   // ads1115 ADC 16 BIT lib
#include "max6675.h"            // T2 termopara lib
#include "DS3231.h"             // ds3231 clock lib
#include <SD.h>                 // Micro SD lib
#include "nRF24L01.h"           // NRF radio config
#include "RF24.h"               // NRF radio lib
#include "FastLED.h"            // WS2812B svetodiod lib 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_CLOK_FOR_PROG 0      // если 1 то установка часов будет при записи программы. если 0 то нет
#define SDCHEK 1                 // 1 ЕСЛИ ФЛЕШКИ НЕТ ТО НЕ СТАРТОВАТЬ. 0 СТАРТОВАТЬ В ЛЮБОМ СЛУЧАЕ
#define DEFMaxFileToSD 1000      // макс количество файлов на флешке. макс 65535
#define EEPROM_WRITE_KEY 123     // код перезаписи EEPROM < 255. если изменить то EEPROM перезапишется из оперативки
#define EEPROM_WRITE_K_ADDR 200  // адрес кода перезаписи EEPROM . 200 <= x < 500
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define BEEPER_FREQ 5000  // частота аварийной пищалки в Герцах
#define BEEPER_DURATION 1000  // длительность сигнала аварийной пищалки в миллисекундах, не больше 1000
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА Arduino:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIPSELEKT 53  // пин чип селект для флэшки: SPI 50 MISO 51 MOSI 52 SCK 49 CHIPSELEKT
#define START_STOP_BUTTON_PIN 30  // кнопка включения выключения записи. замыкать на землю
# define BEEPER_PIN 4  // пин аварийной пищалки

// термопара t2
#define T2_TERMOPARA_SO_PIN 5
#define T2_TERMOPARA_CS_PIN 6
#define T2_TERMOPARA_SCK_PIN 7
#define LED_DATA_PIN 8 // led pin

RF24 radio(48,49);  // nrf init CE, CSN  nrf init:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА ads1115:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PORT_0_O2_ADS1 0   // порт ADS1115 куда подключается датчик O2
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
// значения сенсоров за секунду
int16_t adc0_O2, adc1_CO2, adc2_CO;
float multiplierADS = 0.1875F; // ADS1115 6.144V gain (16-Bit results). делитель для перевода показаний АЦП в вольты
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
float minVal_Press_inh = 0;  // мин
float maxVal_Press_exh = 0;  // макс
float maxVal_CO2 = 0;
float maxVal_CO = 0;
// уровень заряда батареи в %
byte val_BatteryLevel_TX = 50;
// калибровка всех датчиков. значения: inMin, outMin, inMax, outMax
float calibr_O2_Mas[] = {0, 1000, 0, 1000};
float calibr_T1_Mas[] = {0, 1000, 0, 1000};
float calibr_T2_Mas[] = {0, 1000, 0, 1000};
float calibr_CO2_Mas[] = {0, 1000, 0, 1000};
float calibr_CO_Mas[] = {0, 1000, 0, 1000};
float calibr_Press_Mas[] = {0, 1000, 0, 1000};

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
// ads 1115 init:
Adafruit_ADS1115 ads;         /* Use this for the 16-bit version */
// clock init:
DS3231 clock;                // Связываем объект clock с библиотекой DS3231
// clock dataType
RTCDateTime DateTime;        // Определяем переменную DateTime, как описанную структурой RTCDateTime
// microSD  dataType
File dataFile;               // переменная для работы с флэшкой
// Define the Array of leds, LED dataType 1 светодиод
CRGB iLed[1];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NRF rx and tx F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioNrfSetup(){
	radio.begin();                           // активировать модуль
	radio.setAutoAck(1);                     // режим подтверждения приёма, 1 вкл 0 выкл
	radio.setRetries(0,15);                  // (время между попыткой достучаться, число попыток)
	radio.enableAckPayload();                // разрешить отсылку данных в ответ на входящий сигнал
	radio.setPayloadSize(32);                // размер пакета, в байтах
	radio.openWritingPipe(addressNRF[1]);    // передаем по трубе 1
	radio.openReadingPipe(1,addressNRF[0]);  // хотим слушать трубу 0
	radio.setChannel(0x60);                  // выбираем канал (в котором нет шумов!)
	radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS);        // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	                                         // при самой низкой скорости самуf высокуf чувствительность и дальность!!
	radio.powerUp();                         // начать работу
	radio.startListening();                  // начинаем слушать эфир, приёмный модуль
	// radio.stopListening();                // First, stop listening so we can talk.
}
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
	dataString += String(minVal_Press_inh);
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
inline void poolO2(){
	// resive and convert O2 values
	adc0_O2 = ads.readADC_SingleEnded(PORT_0_O2_ADS1);
	val_O2 = adc0_O2 * multiplierADS / 1000.0;  // Volt
	val_O2 = flap(val_O2, 0, 1.6, 0, 100);  // %
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос быстрой ТЕРМОПАРЫ1 через сериал1
inline void poolTermoparaFast1(){
	Serial1.println('t');
	while(!Serial1.available()){}
	char s_d = Serial1.read();
	if(s_d == 'm'){
		val_T1 = Serial1.parseFloat();
	}else{
		val_T1 = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос медленной ТЕРМОПАРЫ2
inline void poolTermoparaSlow2(){
	val_T2 = thermocouple2.readCelsius();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика ДАВЛЕНИЯ через сериал2
inline void poolPressure(){
	Serial2.println('p');
	while(!Serial2.available()){}
	char s_d = Serial2.read();
	if(s_d == 'i'){
		val_Press_inh = Serial2.parseFloat();
	}else{
		val_Press_inh = 0;
	}
	while(s_d != 'e'){
	}
	s_d = Serial2.read();
	if(s_d == 'e'){
		val_Press_exh = Serial2.parseFloat();
	}else{
		val_Press_exh = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO2
inline void poolCO2(){
	// resive and convert CO2 values
	adc1_CO2 = ads.readADC_SingleEnded(PORT_1_CO2_ADS1);
	val_CO2 = adc1_CO2 * multiplierADS / 1000.0;  // Volt & %
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос датчика CO
inline void poolCO(){
	// resive and convert CO values
	adc2_CO = ads.readADC_SingleEnded(PORT_0_O2_ADS1);
	val_CO = adc2_CO * multiplierADS / 1000.0;  // Volt
	val_CO = flap(val_CO, 0, 3, 0, 2000);
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
		if(minVal_Press_inh > val_Press_inh && recordFlag){minVal_Press_inh = val_Press_inh;}
		if(maxVal_Press_exh < val_Press_exh && recordFlag){maxVal_Press_exh = val_Press_exh;}
		if(maxVal_CO2 < val_CO2 && recordFlag){maxVal_CO2 = val_CO2;}
		if(maxVal_CO < val_CO && recordFlag){maxVal_CO = val_CO;}

		// управление диодом и бузером
		if(recordFlag == 0){ // BLUE
			iLed[0] = CRGB::Blue;
			FastLED.show();
		}
		else if(recordFlag && (val_CO2 >= 5 || val_CO >= 170 || val_O2 < 17)){ // RED, SIGNAL
			iLed[0] = CRGB::Red;
			FastLED.show();
			if(beepFlag == 1){
				tone(BEEPER_PIN, BEEPER_FREQ, BEEPER_DURATION);
			}else{
				noTone(BEEPER_PIN);
			}
			beepFlag = !beepFlag;
		}
		else if(recordFlag && (val_CO2 >= 3 || val_CO >= 26 || val_O2 < 21)){ // YELOW
			iLed[0] = CRGB(255, 255, 0);
			FastLED.show();
		}
		else if(recordFlag && (val_CO2 < 3 && val_CO < 26 && val_O2 >= 21)){ // GREEN
			iLed[0] = CRGB::Green;
			FastLED.show();
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
			minVal_Press_inh = 0;
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

	// serial init
	Serial.begin(250000);    // serial
	Serial1.begin(250000);   // T1 sensor
	Serial2.begin(250000);   // Pressure sensor
	delay(100);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(iLed, 1); // 1 светодиод WS2812B

	radioNrfSetup();  // NRF setup

	eePackWrite();  // 1st EEPROM write all calibration val
	eePackRead();   // EEPROM read all calibration val

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

  	// ads setup
  	ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	ads.begin();
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
