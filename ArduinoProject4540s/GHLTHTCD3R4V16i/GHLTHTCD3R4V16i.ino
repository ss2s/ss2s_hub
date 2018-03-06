// GHLTHTCD3R4V16i GROW HUCKER LIGHT TEMP HUMI TIMER CONTROL DATAIN 3 RELEOUT 4 VERSION 16/2 i2c

//#include "Arduino.h"
#include "Dht11.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

// rele type
#define RELE_CONTROL_SET 0
#if RELE_CONTROL_SET == 0
#define HHH 0
#define LLL 1
#else
#define HHH 1
#define LLL 0
#endif

// raspinovka
// IN DATA: RTC DS3231; SENSOR DHT 11, DHT 22;
#define IN_DHT_DATA_PIN 5
// OUT RELE
#define OUT_SVETTIMER_RELE_PIN 4
#define OUT_TEMPERATURA_RELE_PIN 6
#define OUT_VLAJNOST_RELE_PIN 7
#define OUT_POLIVTIMER_RELE_PIN 8

// переменные
//char week[8][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
byte second, minute = 61, hour, dayOfWeek, dayOfMonth, month, year;
byte TEMP = 0; // температура
byte HMDT = 0; // влажность

LiquidCrystal_I2C lcd(0x27, 16, 2); // установка адреса 0x27 и разрешения 16/2 дисплея

///// часы DS3231 ..
byte decToBcd(byte val){
  	return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val){
  	return ( (val/16*10) + (val%16) );
}

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

void getDhtData(byte *TE, byte *HU){
	
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

void setup() {

	pinMode(OUT_SVETTIMER_RELE_PIN, OUTPUT);
	pinMode(OUT_TEMPERATURA_RELE_PIN, OUTPUT);
	pinMode(OUT_VLAJNOST_RELE_PIN, OUTPUT);
	pinMode(OUT_POLIVTIMER_RELE_PIN, OUTPUT);

	digitalWrite(OUT_SVETTIMER_RELE_PIN, LLL);
	digitalWrite(OUT_TEMPERATURA_RELE_PIN, LLL);
	digitalWrite(OUT_VLAJNOST_RELE_PIN, LLL);
	digitalWrite(OUT_POLIVTIMER_RELE_PIN, LLL);

	Serial.begin(9600);
	Wire.begin();
	lcd.begin(); // иниализация дисплея LCD 16/2
  	lcd.clear(); // очистить дисплей

 /*  	// установка часов
  	byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  	second = 30;
  	minute = 0;
  	hour = 14;
  	dayOfWeek = 3; // день недели
  	dayOfMonth = 1; // день
  	month = 4;
  	year = 14;

  	setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
 */
}

void loop() {
	
  	getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);  // запрос текущего времени
  	getDhtData(&TEMP, &HMDT);  // запрос текущей температуры и влажности


}
