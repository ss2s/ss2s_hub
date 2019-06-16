// КОРМУШКА ДЛЯ РЫБ на Arduino Atmega328 + ESP8266 v1.0

// НАСТРОЙКИ В ФАЙЛЕ "setting_config.h"

// настройка ESP8266:
// настройка UART на 9600 в ESP8266 должна стоять заводская AT прошивка
// настройка осуществляется через AT команды при подключении ESP к компу через переходник на скорости 115200
// это команда для настройки: AT+UART_DEF=9600,8,1,0,0

/*************************************************************
WARNING!
It's very tricky to get it working. Please read this article:
http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware
*************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#include "setting_config.h"  // файл с настройками
#include "feeding_control.h" // файл с алгоритмом управления кормлением

// Hardware Serial on Mega, Leonardo, Micro...
//#define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

void setup()
{
	// Debug console
	Serial.begin(9600);

	delay(10);

	// Set ESP8266 baud rate
	EspSerial.begin(ESP8266_BAUD);
	delay(10);

	Blynk.begin(auth, wifi, ssid, pass);
}

void loop()
{
	Blynk.run();
}
