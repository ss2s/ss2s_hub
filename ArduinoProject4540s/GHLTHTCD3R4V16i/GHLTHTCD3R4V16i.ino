////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//   ██████╗ ██████╗  ██████╗ ██╗    ██╗    ██╗  ██╗ █████╗  ██████╗██╗  ██╗███████╗██████╗     ██╗   ██╗ ██╗
//  ██╔════╝ ██╔══██╗██╔═══██╗██║    ██║    ██║  ██║██╔══██╗██╔════╝██║ ██╔╝██╔════╝██╔══██╗    ██║   ██║███║
//  ██║  ███╗██████╔╝██║   ██║██║ █╗ ██║    ███████║███████║██║     █████╔╝ █████╗  ██████╔╝    ██║   ██║╚██║
//  ██║   ██║██╔══██╗██║   ██║██║███╗██║    ██╔══██║██╔══██║██║     ██╔═██╗ ██╔══╝  ██╔══██╗    ╚██╗ ██╔╝ ██║
//  ╚██████╔╝██║  ██║╚██████╔╝╚███╔███╔╝    ██║  ██║██║  ██║╚██████╗██║  ██╗███████╗██║  ██║     ╚████╔╝  ██║
//   ╚═════╝ ╚═╝  ╚═╝ ╚═════╝  ╚══╝╚══╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝      ╚═══╝   ╚═╝
//                                                                                                         
//                                                                                                         
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GHLTHTCD3R4V16i GROW HUCKER LIGHT TEMP HUMI TIMER CONTROL DATAIN 3 RELEOUT 4 VERSION 16/2 i2c

//#include "Arduino.h"
#include "Dht11.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

// res
#include "lcdbyte.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// rele type
#define RELE_CONTROL_SET 0 // 0:1 зависит от того каким сигналом управляется реле
#if RELE_CONTROL_SET == 0
#define DW_H 0
#define DW_L 1
#else
#define DW_H 1
#define DW_L 0
#endif





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  ██████╗      █████╗     ███████╗    ██████╗     ██╗    ███╗   ██╗     ██████╗     ██╗   ██╗    ██╗  ██╗     █████╗ 
//  ██╔══██╗    ██╔══██╗    ██╔════╝    ██╔══██╗    ██║    ████╗  ██║    ██╔═══██╗    ██║   ██║    ██║ ██╔╝    ██╔══██╗
//  ██████╔╝    ███████║    ███████╗    ██████╔╝    ██║    ██╔██╗ ██║    ██║   ██║    ██║   ██║    █████╔╝     ███████║
//  ██╔══██╗    ██╔══██║    ╚════██║    ██╔═══╝     ██║    ██║╚██╗██║    ██║   ██║    ╚██╗ ██╔╝    ██╔═██╗     ██╔══██║
//  ██║  ██║    ██║  ██║    ███████║    ██║         ██║    ██║ ╚████║    ╚██████╔╝     ╚████╔╝     ██║  ██╗    ██║  ██║
//  ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚══════╝    ╚═╝         ╚═╝    ╚═╝  ╚═══╝     ╚═════╝       ╚═══╝      ╚═╝  ╚═╝    ╚═╝  ╚═╝
//                                                                                                                   
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// raspinovka
// DS3231, I2C Display 16/2, = A4 A5 I2C interface

// ENKODER;
#define IN_ENKODER_CLK_PIN 2
#define IN_ENKODER_BUTTON_PIN 3
#define IN_ENKODER_DT_PIN 4

// DHT 11;
#define IN_DHT_DATA_PIN 15 // A1
// EXTTENDED .... - VCC/ DHT 22/ SENSOR_POCVI/ GND;
#define EXTENDED_IN_DHT_DATA_PIN 16 // A2
#define EXTENDED_IN_DATCHIK_POCVI A0

// OUT RELE
#define OUT_SVETTIMER_RELE_PIN 5
#define OUT_POLIVTIMER_RELE_PIN 6
#define OUT_TEMPERATURA_RELE_PIN 7
#define OUT_VLAJNOST_RELE_PIN 8

#define OUT_YARKOST_DISPLEYA_PWM_PIN 9

// init
LiquidCrystal_I2C lcd(0x27, 16, 2); // установка адреса 0x27 и разрешения 16/2 дисплея
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                   







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  ███████╗ ███████╗ ██████╗  ███████╗ ███╗   ███╗ ███████╗ ██╗  ██╗ ██╗     ██╗ ██╗  ██╗ ███████╗          ▄▄███▄▄·
//  ██╔══██║ ██╔════╝ ██╔══██╗ ██╔════╝ ████╗ ████║ ██╔════╝ ██║  ██║ ██║     ██║ ██║  ██║ ██╔════╝          █ ███╔═╝
//  ██║  ██║ █████╗   ██████╔╝ █████╗   ██╔████╔██║ █████╗   ███████║ ██████╗ ██║ ███████║ █████╗            ███████╗
//  ██║  ██║ ██╔══╝   ██╔═══╝  ██╔══╝   ██║╚██╔╝██║ ██╔══╝   ██╔══██║ ██╔══██╗██║ ██╔══██║ ██╔══╝            ╚═███ █║
//  ██║  ██║ ███████╗ ██║      ███████╗ ██║ ╚═╝ ██║ ███████╗ ██║  ██║ ██████╔╝██║ ██║  ██║ ███████╗          ███████║
//  ╚═╝  ╚═╝ ╚══════╝ ╚═╝      ╚══════╝ ╚═╝     ╚═╝ ╚══════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝ ╚═╝  ╚═╝ ╚══════╝          ╚═▀▀▀══╝
//                                   
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  ГЛОБАЛЬНЫЕ ФЛАГИ
bool flagSvetocikla = 0;     // для хранения переменной день или ночь
bool flagSvetocikla2 = 0;    // для хранения переменной день или ночь 2

bool flagPoliva1 = 0;        // для хранения переменной вкл/выкл
bool flagPoliva2 = 0;        // для хранения переменной вкл/выкл 2
bool flagPoliva3 = 0;        // для хранения переменной вкл/выкл 3

bool flagTermorele = 0;      // для хранения переменной вкл/выкл

bool flagHumidityrele = 0;   // для хранения переменной вкл/выкл

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ПЕРЕМЕННЫЕ НАВИГАЦИИ ПО МЕНЮ
int vertikalStep = 0;
int horizontalStep = 0;
byte menuVertikalPos = 0;
byte menuHorizontalPos = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ПЕРЕМЕННЫЕ С ДАННЫМИ
//char week[8][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
byte second, minute = 61, hour, dayOfWeek, dayOfMonth, month, year;

float TEMP = 0; // температура
float HMDT = 0; // влажность
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ПЕРЕМЕННЫЕ УПРАВЛЕНИЯ И НАСТРОЙКИ ПАРАМЕТРОВ

// СВЕТОЦИКЛ
byte svetociklType = 1;                    // 2 таймер досветки : 1 таймер светоцикла : 0 отключено
byte vremyaVkluchenieSveta[2] = {6,0};     // время включения реле света
byte vremyaVikluchenieSveta[2] = {18,0};   // время выключения реле света
byte vremyaVkluchenieSveta2[2] = {6,0};    // 2 время включения реле света
byte vremyaVikluchenieSveta2[2] = {18,0};  // 2 время выключения реле света



// АВТОПОЛИВ
byte polivaRejim = 1;                      // 3 от датчика влажности почвы : 2 по времени : 1 циклический : 0 отключено
byte vremyaRabotiNasosa[2] = {0,1};        // часов, минут работы насоса полива
byte vremyaViklucheniaNasosa[2] = {25,61}; // во столько выключится насос часов, минут (служебная переменная)
// в режиме 0 циклический
byte mejduPolivami[2] = {0,15};            // часов, минут между поливами
// в режиме 1 по заданому времени
byte vremyaPoliva1[2] = {6,15};            // время 1 полива часы, минуты
byte vremyaPoliva2[2] = {12,15};           // время 2 полива часы, минуты
byte vremyaPoliva3[2] = {18,15};           // время 3 полива часы, минуты

bool datchikPolivaTypeSet = 1;             // тип работы датчика: 0 аналоговый / 1 цифровой
bool datchikPolivaValSet = 1;              // какой сигнал ждать с датчика почвы: 0 - / 1 + 
byte datchikPolivaValAnalogSet = 50;       // точка сработки датчика полива от 0 до 100



// ТЕМПЕРАТУРА
byte tempRejim = 1;                        // 3 настраиваемый : 2 нагрев : 1 охлаждение : 0 отключено
float temperaturaUderjania = 23;           // температура которая будет поддерживатся
float temperaturaVkluchenia = 25;          // температура включения реле температуры
float temperaturaVikluchenia = 22;         // температура выключения реле температуры
float gisterezisTemperaturi = 2;           // гистерезис температуры



// ВЛАЖНОСТЬ
byte vlajnostRejim = 2;                    // 3 настраиваемый : 2 увлажнение : 1 сушка : 0 отключено
float vlajnostUderjania = 23;              // влажность которая будет поддерживатся
float vlajnostVkluchenia = 50;             // влажность включения реле влажности
float vlajnostVikluchenia = 60;            // влажность выключения реле влажности
float gisterezisVlajnosti = 10;            // гистерезис влажности



// ЯРКОСТЬ ДИСПЛЕЯ
byte yarkostDispleya = 255;                // яркость екрана 0 - 256





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  
//  ███████╗    ███╗   ██╗    ██╗  ██╗     ██████╗     ██████╗     ███████╗    ██████╗             ██╗   ██╗
//  ██╔════╝    ████╗  ██║    ██║ ██╔╝    ██╔═══██╗    ██╔══██╗    ██╔════╝    ██╔══██╗            ██║   ██║
//  █████╗      ██╔██╗ ██║    █████╔╝     ██║   ██║    ██║  ██║    █████╗      ██████╔╝            ██║   ██║
//  ██╔══╝      ██║╚██╗██║    ██╔═██╗     ██║   ██║    ██║  ██║    ██╔══╝      ██╔══██╗            ╚██╗ ██╔╝
//  ███████╗    ██║ ╚████║    ██║  ██╗    ╚██████╔╝    ██████╔╝    ███████╗    ██║  ██║             ╚████╔╝ 
//  ╚══════╝    ╚═╝  ╚═══╝    ╚═╝  ╚═╝     ╚═════╝     ╚═════╝     ╚══════╝    ╚═╝  ╚═╝              ╚═══╝  
//                                                                                                                                  
//                                                                                    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ПЕРЕМЕННЫЕ ЕНКОДЕРА

// распиновка
int pinA = IN_ENKODER_CLK_PIN;          //Пин прерывания сигнальной линии енкодера
int pinButton = IN_ENKODER_BUTTON_PIN;  //Пин прерывания нажатия кнопки
int pinB = IN_ENKODER_DT_PIN;           //другой пин енкодера

long timeButtonPressed = 1000;    // Долгое удержание кнопки после 1 секунд. настройка поведения ниже ↓
#define LONG_PRESS_CONTINUES 1    // 1 будет выполнятся одно событие, 0 будут повторятся события пока нажата кнопка

volatile int state = 0;                 // Переменная хранящая статус вращения

// Переменные хранящие состояние действия до его выполнения
volatile bool flagCW            = false;     // Было ли вращение по часовой стрелке
volatile bool flagCCW           = false;     // Было ли вращение против часовой стрелки
volatile bool flagButton        = false;     // Было ли нажатие кнопки
volatile bool flagButtonLong    = false;     // Было ли долгое удержание кнопки

volatile long timeButtonDown    = 0;         // Переменная хранящая время нажатия кнопки
volatile bool isButtonDown      = false;     // Переменная хранящая время нажатия кнопки
volatile bool longPressReleased = false;     // Переменная для фиксации срабатывания долгого нажатия





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  ███████╗    ███╗   ██╗    ██╗  ██╗     ██████╗     ██████╗     ███████╗    ██████╗             ███████╗
//  ██╔════╝    ████╗  ██║    ██║ ██╔╝    ██╔═══██╗    ██╔══██╗    ██╔════╝    ██╔══██╗            ██╔════╝
//  █████╗      ██╔██╗ ██║    █████╔╝     ██║   ██║    ██║  ██║    █████╗      ██████╔╝            █████╗  
//  ██╔══╝      ██║╚██╗██║    ██╔═██╗     ██║   ██║    ██║  ██║    ██╔══╝      ██╔══██╗            ██╔══╝  
//  ███████╗    ██║ ╚████║    ██║  ██╗    ╚██████╔╝    ██████╔╝    ███████╗    ██║  ██║            ██║     
//  ╚══════╝    ╚═╝  ╚═══╝    ╚═╝  ╚═╝     ╚═════╝     ╚═════╝     ╚══════╝    ╚═╝  ╚═╝            ╚═╝     
//                                                                                                       
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ФУНКЦИИ ЕНКОДЕРА

void A(){

  int pinAValue = digitalRead(pinA);        // Получаем состояние пинов A и B
  int pinBValue = digitalRead(pinB);

  cli();                                    // Запрещаем обработку прерываний, чтобы не отвлекаться
  if (!pinAValue &&  pinBValue) state = 1;  // Если при спаде линии А на линии B единица, то вращение в одну сторону
  if (!pinAValue && !pinBValue) state = -1; // Если при спаде линии А на линии B ноль, то вращение в другую сторону
  if (pinAValue && state != 0) {
    if (state == 1 && !pinBValue || state == -1 && pinBValue) { // Если на линии А снова единица, фиксируем шаг
      if (state == 1)   flagCW = true;      // Флаг вращения по часовой стрелке
      if (state == -1) flagCCW = true;      // Флаг вращения против часовой стрелки
      state = 0;
    }
  }
  sei();                                    // Разрешаем обработку прерываний
}

void Button(){
  
  if (millis() - timeButtonDown < 50) return;
  int pinButValue = digitalRead(pinButton);   // Получаем состояние пина кнопки

  cli();                                      // Запрещаем обработку прерываний, чтобы не отвлекаться
  timeButtonDown = millis();                  // Запоминаем время нажатия/отжатия

  if (!pinButValue) {                         // При нажатии подается инвертированный сигнал
    isButtonDown = true;                      // Устанавливаем флаг нажатия кнопки
  }
  else if (isButtonDown) {                    // Если кнопка отжата, смотрим не было ли выполнено действие
    if (!longPressReleased) {                 // Если долгое нажатие не было ни разу отработано, то...
      flagButton = true;                      // Если не было удержания, ставим флаг события обычного нажатия
    }
    isButtonDown = false;                     // Сбрасываем флаг нажатия
    longPressReleased = false;                // Сбрасываем флаг длительного удержания
  }
  sei();                                      // Разрешаем обработку прерываний
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  ██╗  ██╗     █████╗      ██████╗    ██╗     ██╗
//  ██║  ██║    ██╔══██╗    ██╔════╝    ██║     ██║
//  ╚██████║    ███████║    ██║         ██████╗ ██║
//   ╚═══██║    ██╔══██║    ██║         ██╔══██╗██║
//       ██║    ██║  ██║    ╚██████╗    ██████╔╝██║
//       ╚═╝    ╚═╝  ╚═╝     ╚═════╝    ╚═════╝ ╚═╝
// 
//                                                    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///// часы DS3231 ..
byte decToBcd(byte val){
  	return ( (val/10*16) + (val%10) );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

byte bcdToDec(byte val){
  	return ( (val/16*10) + (val%16) );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setDateDs1307(byte second,        // 0-59
                   byte minute,        // 0-59
                   byte hour,          // 1-23
                   byte dayOfWeek,     // 1-7
                   byte dayOfMonth,    // 1-28/29/30/31
                   byte month,         // 1-12
                   byte year)          // 0-99
{
   	Wire.beginTransmission(0x68);
   	Wire.write(0);
   	Wire.write(decToBcd(second));    
   	Wire.write(decToBcd(minute));
   	Wire.write(decToBcd(hour));     
   	Wire.write(decToBcd(dayOfWeek));
   	Wire.write(decToBcd(dayOfMonth));
   	Wire.write(decToBcd(month));
   	Wire.write(decToBcd(year));
   	Wire.endTransmission();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getDateDs1307(byte *second,
          		   byte *minute,
          		   byte *hour,
          		   byte *dayOfWeek,
          		   byte *dayOfMonth,
          		   byte *month,
          		   byte *year)
{

  	Wire.beginTransmission(0x68);
  	Wire.write(0);
  	Wire.endTransmission();

  	Wire.requestFrom(0x68, 7);

  	*second     = bcdToDec(Wire.read() & 0x7f);
  	*minute     = bcdToDec(Wire.read());
  	*hour       = bcdToDec(Wire.read() & 0x3f); 
  	*dayOfWeek  = bcdToDec(Wire.read());
  	*dayOfMonth = bcdToDec(Wire.read());
  	*month      = bcdToDec(Wire.read());
  	*year       = bcdToDec(Wire.read());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
///// температура DS3231 ..
float get3231Temp(){

  	byte tMSB, tLSB; 
  	float temp3231;

  	Wire.beginTransmission(0x68);
  	Wire.write(0x11);
  	Wire.endTransmission();
  	Wire.requestFrom(0x68, 2);

  	if(Wire.available()) {

    	tMSB = Wire.read(); //2's complement int portion
    	tLSB = Wire.read(); //fraction portion

    	temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    	temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  	}else{
    	//oh noes, no data!
  	}

  	return temp3231;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getDhtData(float *TE, float *HU){
	
  	static Dht11 sensor(IN_DHT_DATA_PIN);

  	switch (sensor.read()) {

    	case Dht11::OK:
        //lcd.clear(); // очистить дисплей
        *TE = sensor.getTemperature();
        *HU = sensor.getHumidity();      
        break;

    	case Dht11::ERROR_CHECKSUM:    
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Checksum error");
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;

    	case Dht11::ERROR_TIMEOUT:
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Timeout error");
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;

    	default:
        wdt_reset();
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Unknown error"); 
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getDhtDataByte(void){
	
  	static Dht11 sensor(IN_DHT_DATA_PIN);

  	switch (sensor.read()) {

    	case Dht11::OK:
        //lcd.clear(); // очистить дисплей
        TEMP = sensor.getTemperature();
        HMDT = sensor.getHumidity();      
        break;

    	case Dht11::ERROR_CHECKSUM:    
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Checksum error");
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;

    	case Dht11::ERROR_TIMEOUT:
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Timeout error");
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;

    	default:
        wdt_reset();
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Unknown error"); 
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;
    }
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                
//                                                                
//                                                                 ██████████╗ 
//  ███╗   ███╗    ███████╗    ███╗   ██╗    ██╗   ██╗            ███      ███╗
//  ████╗ ████║    ██╔════╝    ████╗  ██║    ██║   ██║            ██  ████  ██║
//  ██╔████╔██║    █████╗      ██╔██╗ ██║    ██║   ██║            ██  ████  ██║
//  ██║╚██╔╝██║    ██╔══╝      ██║╚██╗██║    ██║   ██║            ███      ███║
//  ██║ ╚═╝ ██║    ███████╗    ██║ ╚████║    ╚██████╔╝            ╚██████████╔╝
//  ╚═╝     ╚═╝    ╚══════╝    ╚═╝  ╚═══╝     ╚═════╝              ╚═════════╝ 
//                                                                
//                                                                
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekMenuLoop(int count = 1){

	horizontalStep = 0;
	vertikalStep = 0;
	sei();                                      // Разрешаем обработку прерываний

	for(int i=0; i<count; i++){

		// chek enkoder
		delay(200);                                 // задержка 200 мс
		enkoderChek();                              // проверка переменных енкодера
	 		if(horizontalStep > 0){
	 		menuLoop();
	 	}
	}

	cli();                                      // Запрещаем обработку прерываний, чтобы не отвлекаться
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayToMenuPos(int menuPos){

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayToMenuXY(){

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void menuLoop(){

	sei();
	cli();
}

////////////////////////////////////////////////////////////////

// MAIN::
	// ВКЛ : / ВЫКЛ ::     ПОДСВЕТКУ ДИСПЛЕЯ +

////////////////////////////////////////////////////////////////

// РУЧНОЕ УПРАВЛЕНИЕ ВКЛ/ВЫКЛ РЕЛЕ
	// НАЗАД ← / СВЕТ РЕЛЕ / ТЕРМО РЕЛЕ / ВЛАЖНОСТИ РЕЛЕ / ПОЛИВА РЕЛЕ:
															// ВКЛ/ВЫКЛ
															// ВКЛ. НА n МИНУТ

////////////////////////////////////////////////////////////////

// НАСТРОЙКА СВЕТОЦИКЛА:
	// НАЗАД ←
	/// 3 НЕДЕЛЬНЫЙ ТАЙМЕР(ПРОТОТИП ФУНКЦИИ)?
		// ?
	/// 2 ДОСВЕТКА:
			// 1 ВРЕМЯ ВКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// 1 ВРЕМЯ ВЫКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// 2 ВРЕМЯ ВКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// 2 ВРЕМЯ ВЫКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// СОХРАНИТЬ*
			// НАЗАД ←
	/// 1 СВЕТОЦИКЛ:
			// ВРЕМЯ ВКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// ВРЕМЯ ВЫКЛЮЧЕНИЯ РЕЛЕ:
				// НАСТРОЙКА+-:
			// СОХРАНИТЬ*
			// НАЗАД ←
	/// 0 ВЫКЛЮЧЕН РУЧНОЙ РЕЖИМ:
			// УСТАНОВИТЬ ТИП ТАЙМЕРА 0. СОХРАНИТЬ*
			// НАЗАД ←

////////////////////////////////////////////////////////////////

// НАСТРОЙКА РЕЛЕ ТЕМПЕРАТУРЫ
	// 3 НАСТРАИВАЕМЫЙ
		// ТЕМПЕРАТУРА ВКЛ/ ТЕМПЕРАТУРА ВЫКЛ/ СОХРАНИТЬ/ ОТМЕНА
	// 2 НАГРЕВ
		// ЖЕЛАЕМАЯ ТЕМПЕРАТУРА/ ГИСТЕРЕЗИС/ СОХРАНИТЬ/ ОТМЕНА
	// 1 ОХЛАЖДЕНИЕ
		// ЖЕЛАЕМАЯ ТЕМПЕРАТУРА/ ГИСТЕРЕЗИС/ СОХРАНИТЬ/ ОТМЕНА
	// 0 ОТКЛЮЧЕНО

////////////////////////////////////////////////////////////////

// НАСТРОЙКА РЕЛЕ ВЛАЖНОСТИ
	// 3 НАСТРАИВАЕМЫЙ
		// ВЛАЖНОСТЬ ВКЛ/ ВЛАЖНОСТЬ ВЫКЛ/ СОХРАНИТЬ/ ОТМЕНА
	// 2 УВЛАЖНЕНИЕ
		// ЖЕЛАЕМАЯ ВЛАЖГОСТЬ/ ГИСТЕРЕЗИС/ СОХРАНИТЬ/ ОТМЕНА
	// 1 СУШКА
		// ЖЕЛАЕМАЯ ВЛАЖНОСТЬ/ ГИСТЕРЕЗИС/ СОХРАНИТЬ/ ОТМЕНА
	// 0 ОТКЛЮЧЕНО

////////////////////////////////////////////////////////////////

// НАСТРОЙКА РЕЛЕ АВТОПОЛИВА
	// 3 ОТ ДАТЧИКА ВЛАЖНОСТИ
		// 0, 1, 2-АНАЛОГОВЫЙ/ ПОРОГ СРАБОТКИ АНАЛОГОВОГО ДАТЧИКА/ СОХРАНИТЬ/ ОТМЕНА
	// 2 ПО ВРЕМЕНИ
		// ВРЕМЯ 1/ ВРЕМЯ 2/ ВРЕМЯ 3/ ВРЕМЯ РАБОТЫ
	// 1 ЦИКЛИЧЕСКИЙ
		// ВРЕМЯ ОЖИДАНИЯ/ ВРЕМЯ РАБОТЫ
	// 0 ОТКЛЮЧЕНО

////////////////////////////////////////////////////////////////

// НАСТРОЙКА ЯРКОСТИ ДИСПЛЕЯ
	// + - НАСТРОЙКА/ СОХРАНИТЬ/ ОТМЕНА

////////////////////////////////////////////////////////////////

// НАСТРОЙКА ЧАСОВ, ВРЕМЯ И ДАТА
	// ГОД/ МЕСЯЦ/ ДЕНЬ/ ДЕНЬ НЕДЕЛИ/ ЧАСЫ/ МИНУТЫ/ СЕКУНДЫ/ СОХРАНИТЬ

////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  ███████╗    ███╗   ██╗    ██╗  ██╗     ██████╗     ██████╗     ███████╗    ██████╗                 ██╗   
//  ██╔════╝    ████╗  ██║    ██║ ██╔╝    ██╔═══██╗    ██╔══██╗    ██╔════╝    ██╔══██╗                ██║   
//  █████╗      ██╔██╗ ██║    █████╔╝     ██║   ██║    ██║  ██║    █████╗      ██████╔╝                ██║   
//  ██╔══╝      ██║╚██╗██║    ██╔═██╗     ██║   ██║    ██║  ██║    ██╔══╝      ██╔══██╗                ╚═╝   
//  ███████╗    ██║ ╚████║    ██║  ██╗    ╚██████╔╝    ██████╔╝    ███████╗    ██║  ██║             ██▀██▀██╗
//  ╚══════╝    ╚═╝  ╚═══╝    ╚═╝  ╚═╝     ╚═════╝     ╚═════╝     ╚══════╝    ╚═╝  ╚═╝             ╚═╝╚═╝╚═╝
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
// USER ENKODER F

void enkoderChek(){

  	if (millis() - timeButtonDown > timeButtonPressed && isButtonDown) { // Время длительного удержания наступило
  	  	flagButtonLong = true;
  	}

  	if (flagCW) {                             // Шаг вращения по часовой стрелке
  	  	// ....
  	  	horizontalStep ++;
  	  	Serial.println("turn_right");
  	  	flagCW = false;                       // Действие обработано - сбрасываем флаг
  	}
  	if (flagCCW) {                            // Шаг вращения против часовой стрелки
  	  	// ...
  	  	horizontalStep --;
  	  	Serial.println("turn_left");
  	  	flagCCW = false;                      // Действие обработано - сбрасываем флаг
  	}
  	if (flagButton) {                         // Кнопка нажата
  	  	// .
  	  	vertikalStep ++;
  	  	Serial.println("short_press");
  	  	flagButton = false;                   // Действие обработано - сбрасываем флаг
  	}
  	if (flagButtonLong && isButtonDown) {   // Кнопка удерживается
  	  	if (!digitalRead(pinButton) && millis() - timeButtonDown > timeButtonPressed) { //Защита от ложного срабатывания
  	    // ..
  	  		vertikalStep --;
  	    	Serial.println("long_press");
  	  	}

  	  	//================================= Настраиваем реакцию на долгое удержание кнопки =============================
  	  	// Для того, чтобы изменить это поведение, измените настройки в блоке ENKODER V
  	  	#if LONG_PRESS_CONTINUES == 0
  	  	// при долгом удержании кнопки, событие long_press повторяется каждые 1,5 секунды
  	  	timeButtonDown = millis(); // Сбрасываем таймер
  	  	longPressReleased = true;  // Флаг срабатывания держания, чтоб отсечь обычное нажатие при отпускании кнопки
  	  	#else
  	  	// при долгом удержании кнопки, событие long_press повторялось 1 раз
  	  	isButtonDown = false;                   // Программно "отжимаем" кнопку
  	  	#endif
  	  	//==============================================================================================================

  	  	flagButtonLong = false;               // Действие обработано - сбрасываем флаг
  	}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
//  
//  ██████╗     ██╗    ███████╗    ██████╗     ██╗          █████╗     ██╗   ██╗     ████████████████████████████████╗
//  ██╔══██╗    ██║    ██╔════╝    ██╔══██╗    ██║         ██╔══██╗    ╚██╗ ██╔╝     ██╔═══════════════════════════██║
//  ██║  ██║    ██║    ███████╗    ██████╔╝    ██║         ███████║     ╚████╔╝      ██║        GROW HACKER        ██║
//  ██║  ██║    ██║    ╚════██║    ██╔═══╝     ██║         ██╔══██║      ╚██╔╝       ██║                           ██║
//  ██████╔╝    ██║    ███████║    ██║         ███████╗    ██║  ██║       ██║        ████████████████████████████████║
//  ╚═════╝     ╚═╝    ╚══════╝    ╚═╝         ╚══════╝    ╚═╝  ╚═╝       ╚═╝        ╚═══════════════════════════════╝
//                                                                              
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekReleToPrint( byte rele){  // функция печатает на дисплее состояние выбранного выхода

	if(digitalRead(rele) == DW_H){
		lcd.write(byte(1));
	}else{
		lcd.write(byte(0));
	}
}

void displayMain(){

	getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени
  	getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

  	lcd.clear();
  	lcd.print("CBET ");
  	chekReleToPrint(OUT_SVETTIMER_RELE_PIN);
  	lcd.setCursor(8, 0);
  	lcd.print("TEMP ");
  	lcd.print(TEMP);
  	chekReleToPrint(OUT_TEMPERATURA_RELE_PIN);
  	lcd.setCursor(0, 1);
  	lcd.print("POLIV");
  	chekReleToPrint(OUT_POLIVTIMER_RELE_PIN);
  	lcd.setCursor(8, 1);
  	lcd.print("VLAJ");
  	lcd.print(HMDT);
  	chekReleToPrint(OUT_VLAJNOST_RELE_PIN);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayMainLight(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayMainTemp(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayMainHumidity(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayMainTimer(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayMainDateTime(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//   ██████╗    ██╗  ██╗    ███████╗    ██╗  ██╗            ██████╗      █████╗     ██████╗      █████╗     ███╗   ███╗
//  ██╔════╝    ██║  ██║    ██╔════╝    ██║ ██╔╝            ██╔══██╗    ██╔══██╗    ██╔══██╗    ██╔══██╗    ████╗ ████║
//  ██║         ███████║    █████╗      █████╔╝             ██████╔╝    ███████║    ██████╔╝    ███████║    ██╔████╔██║
//  ██║         ██╔══██║    ██╔══╝      ██╔═██╗             ██╔═══╝     ██╔══██║    ██╔══██╗    ██╔══██║    ██║╚██╔╝██║
//  ╚██████╗    ██║  ██║    ███████╗    ██║  ██╗            ██║         ██║  ██║    ██║  ██║    ██║  ██║    ██║ ╚═╝ ██║
//   ╚═════╝    ╚═╝  ╚═╝    ╚══════╝    ╚═╝  ╚═╝            ╚═╝         ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚═╝     ╚═╝
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// main function
void chekLightLoop(){

	if(svetociklType == 0){  // off

		return;
	}else if(svetociklType == 1){  // svetocikl

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaVkluchenieSveta[0]) 
			&& (minute >= vremyaVkluchenieSveta[1]) 
			&& (flagSvetocikla == 0) 
			&& ((hour < vremyaVikluchenieSveta[0]) 
				|| ((hour == vremyaVikluchenieSveta[0]) 
					&& (minute < vremyaVikluchenieSveta[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla = 1;
		}
		else if((hour == vremyaVikluchenieSveta[0]) 
			&& (minute >= vremyaVikluchenieSveta[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla = 0;
		}
	}else if(svetociklType == 2){  // dosvetka

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaVkluchenieSveta[0]) 
			&& (minute >= vremyaVkluchenieSveta[1]) 
			&& (flagSvetocikla == 0) 
			&& (flagSvetocikla2 == 0)
			&& ((hour < vremyaVikluchenieSveta[0]) 
				|| ((hour == vremyaVikluchenieSveta[0]) 
					&& (minute < vremyaVikluchenieSveta[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla = 1;
		}
		else if((hour == vremyaVikluchenieSveta[0]) 
			&& (minute >= vremyaVikluchenieSveta[1]) 
			&& (flagSvetocikla == 1)
			&& (flagSvetocikla2 == 0)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla = 0;
		}else if((hour == vremyaVkluchenieSveta2[0]) 
			&& (minute >= vremyaVkluchenieSveta2[1]) 
			&& (flagSvetocikla2 == 0)
			&& (flagSvetocikla == 0) 
			&& ((hour < vremyaVikluchenieSveta2[0]) 
				|| ((hour == vremyaVikluchenieSveta2[0]) 
					&& (minute < vremyaVikluchenieSveta2[1])))){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_H);
			flagSvetocikla2 = 1;
		}
		else if((hour == vremyaVikluchenieSveta2[0]) 
			&& (minute >= vremyaVikluchenieSveta2[1]) 
			&& (flagSvetocikla2 == 1)
			&& (flagSvetocikla == 0)){

			digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
			flagSvetocikla2 = 0;
		}
	}
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void vremyaViklucheniaNasosaFind(byte tHour, byte tMinute, byte addHour, byte addMinute){

	byte resVVNF[2] = {0, 0};

	resVVNF[0] = tHour;
	resVVNF[1] = tMinute;

	resVVNF[1] += addMinute;
	if(resVVNF[1] > 60){
		resVVNF[1] -= 60;
		resVVNF[0] += 1;
	}
	resVVNF[0] += addHour;
	if(resVVNF[0] > 24){
		resVVNF [0] -= 24;
	}

	vremyaViklucheniaNasosa[0] = resVVNF[0];
	vremyaViklucheniaNasosa[1] = resVVNF[1];
}

void chekPolivLoop(){

	if(polivaRejim == 0){ // otklucheno

		return;
	}else if(polivaRejim == 1){ // ciklicheskiy

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaPoliva1[0]) 
			&& (minute >= vremyaPoliva1[1]) 
			&& (flagPoliva1 == 0) 
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva1 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
		}
	}else if(polivaRejim == 2){ // po vremeni
		
		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if((hour == vremyaPoliva1[0])                               // 1
			&& (minute >= vremyaPoliva1[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva1 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 1)
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
		}else if((hour == vremyaPoliva2[0])                         // 2
			&& (minute >= vremyaPoliva2[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva2 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 0)
			&& (flagPoliva2 == 1)
			&& (flagPoliva3 == 0)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva2 = 0;
		}else if((hour == vremyaPoliva3[0])                         // 3
			&& (minute >= vremyaPoliva3[1]) 
			&& (flagPoliva1 == 0) 
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 0)
			&& ((hour < vremyaViklucheniaNasosa[0]) 
				|| ((hour == vremyaViklucheniaNasosa[0]) 
					&& (minute < vremyaViklucheniaNasosa[1])))){

			flagPoliva3 = 1;
			vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
		}
		else if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagPoliva1 == 0)
			&& (flagPoliva2 == 0)
			&& (flagPoliva3 == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva3 = 0;
		}
	}else if(polivaRejim == 3){ // ot datchika

		getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени

		if(datchikPolivaTypeSet == 1){                             // цифровой датчик  почвы

			bool datchikPochviVal = digitalRead(EXTENDED_IN_DATCHIK_POCVI);

			if((datchikPochviVal == datchikPolivaValSet) && (flagPoliva1 == 0)){

				flagPoliva1 = 1;
				vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
				digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
			}

			if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
			}
		}else{                                                     // аналоговый датчик почвы

			int datchikPochviVal = analogRead(EXTENDED_IN_DATCHIK_POCVI);

			if((datchikPochviVal >= datchikPolivaValAnalogSet) && (flagPoliva1 == 0)){

				flagPoliva1 = 1;
				vremyaViklucheniaNasosaFind(hour, minute, vremyaRabotiNasosa[0], vremyaRabotiNasosa[1]);
				digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_H);
			}

			if((hour == vremyaViklucheniaNasosa[0]) 
			&& (minute >= vremyaViklucheniaNasosa[1]) 
			&& (flagSvetocikla == 1)){

			digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);
			flagPoliva1 = 0;
			}
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekTempLoop(){

	if(tempRejim == 0){        // off

		return;
	}else if(tempRejim == 1){  // если охлаждение

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float temperaturaUderjaniaAndGisterezis = temperaturaUderjania + gisterezisTemperaturi;

		if(TEMP >= temperaturaUderjaniaAndGisterezis){
			flagTermorele = 1;
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
		}else if(TEMP < temperaturaUderjania){
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
	}else if(tempRejim == 2){  // если нагрев    

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float temperaturaUderjaniaAndGisterezis = temperaturaUderjania - gisterezisTemperaturi;

		if(TEMP <= temperaturaUderjaniaAndGisterezis){
			flagTermorele = 1;
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
		}else if(TEMP > temperaturaUderjania){
			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
	}else if(tempRejim == 3){  // если настраиваемый

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		if(temperaturaVkluchenia > temperaturaVikluchenia){

			if(TEMP >= temperaturaVkluchenia){
				flagTermorele = 1;
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
			}else if(TEMP < temperaturaVikluchenia){
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
				flagTermorele = 0;
			}
		}
		else if(temperaturaVkluchenia == temperaturaVikluchenia){

			digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
			flagTermorele = 0;
		}
		else if(temperaturaVkluchenia < temperaturaVikluchenia){

			if(TEMP <= temperaturaVkluchenia){
				flagTermorele = 1;
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_H);
			}else if(TEMP > temperaturaVikluchenia){
				digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
				flagTermorele = 0;
			}
		}
	}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void chekHumidityLoop(){

	if(vlajnostRejim == 0){        // off

		return;
	}else if(vlajnostRejim == 1){  // если сушка

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float vlajnostUderjaniaAndGisterezis = vlajnostUderjania + gisterezisVlajnosti;

		if(HMDT >= vlajnostUderjaniaAndGisterezis){
			flagHumidityrele = 1;
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
		}else if(HMDT < vlajnostUderjania){
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
	}else if(vlajnostRejim == 2){  // если увлажнение   

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		float vlajnostUderjaniaAndGisterezis = vlajnostUderjania - gisterezisVlajnosti;

		if(HMDT <= vlajnostUderjaniaAndGisterezis){
			flagHumidityrele = 1;
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
		}else if(HMDT > vlajnostUderjania){
			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
	}else if(vlajnostRejim == 3){  // если настраиваемый   

		getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности

		if(vlajnostVkluchenia > vlajnostVikluchenia){

			if(HMDT >= vlajnostVkluchenia){
				flagHumidityrele = 1;
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
			}else if(HMDT < vlajnostVikluchenia){
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
				flagHumidityrele = 0;
			}
		}
		else if(vlajnostVkluchenia == vlajnostVikluchenia){

			digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
			flagHumidityrele = 0;
		}
		else if(vlajnostVkluchenia < vlajnostVikluchenia){

			if(HMDT <= vlajnostVkluchenia){
				flagHumidityrele = 1;
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_H);
			}else if(HMDT > vlajnostVikluchenia){
				digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
				flagHumidityrele = 0;
			}
		}
	}
}



	
void chekParam(){

	// chek param
	chekLightLoop();
	chekPolivLoop();
	chekTempLoop();
	chekHumidityLoop();
}	





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//   █████╗     ██████╗     ██████╗     ██╗   ██╗    ██╗    ███╗   ██╗     ██████╗             ███████╗
//  ██╔══██╗    ██╔══██╗    ██╔══██╗    ██║   ██║    ██║    ████╗  ██║    ██╔═══██╗            ██╔════╝
//  ███████║    ██████╔╝    ██║  ██║    ██║   ██║    ██║    ██╔██╗ ██║    ██║   ██║            █████╗  
//  ██╔══██║    ██╔══██╗    ██║  ██║    ██║   ██║    ██║    ██║╚██╗██║    ██║   ██║            ██╔══╝  
//  ██║  ██║    ██║  ██║    ██████╔╝    ╚██████╔╝    ██║    ██║ ╚████║    ╚██████╔╝            ██║     
//  ╚═╝  ╚═╝    ╚═╝  ╚═╝    ╚═════╝      ╚═════╝     ╚═╝    ╚═╝  ╚═══╝     ╚═════╝             ╚═╝     
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Arduino
void setup() {

	pinMode(OUT_SVETTIMER_RELE_PIN, OUTPUT);
	pinMode(OUT_TEMPERATURA_RELE_PIN, OUTPUT);
	pinMode(OUT_VLAJNOST_RELE_PIN, OUTPUT);
	pinMode(OUT_POLIVTIMER_RELE_PIN, OUTPUT);

	digitalWrite(OUT_SVETTIMER_RELE_PIN, DW_L);
	digitalWrite(OUT_TEMPERATURA_RELE_PIN, DW_L);
	digitalWrite(OUT_VLAJNOST_RELE_PIN, DW_L);
	digitalWrite(OUT_POLIVTIMER_RELE_PIN, DW_L);

	pinMode(pinA, INPUT);           // Пины в режим приема INPUT
  	pinMode(pinB, INPUT);           // Пины в режим приема INPUT
  	pinMode(pinButton, INPUT);      // Пины в режим приема INPUT

	Wire.begin();

    lcd.createChar(0, off0);  // create a new custom character
    lcd.createChar(1, on1);   // create a new custom character
    lcd.createChar(2, strelkaVibora2);   // create a new custom character
    lcd.createChar(3, strelkaNazad3);   // create a new custom character
    // lcd.write(byte(0));
    // lcd.write(byte(1));
    // lcd.write(byte(2));
    // lcd.write(byte(3));

	lcd.begin(); // иниализация дисплея LCD 16/2
  	lcd.clear(); // очистить дисплей


 	// УСТАНОВКА ЧАСОВ:↓
 /*
  	second = 30;
  	minute = 0;
  	hour = 12;
  	dayOfWeek = 3; // день недели
  	dayOfMonth = 21; // день
  	month = 3;
  	year = 18;

  	setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
 */

  	attachInterrupt(0, A, CHANGE);        // Настраиваем обработчик прерываний по изменению сигнала на линии A
  	attachInterrupt(1, Button, CHANGE);   // Настраиваем обработчик прерываний по изменению сигнала нажатия кнопки

	Serial.begin(9600);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

	displayMain();    // osnovnoy displey

	chekMenuLoop(1);  // proverka vhoda v menu s enkodera (raz)

	chekParam();      // chek param
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
