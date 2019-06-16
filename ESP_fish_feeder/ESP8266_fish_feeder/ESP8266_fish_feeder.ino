// КОРМУШКА ДЛЯ РЫБ на Arduino Atmega328 + ESP8266 v1.0

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

// You should get Auth Token in the Blynk App.
char auth[] = "";

// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

uint16_t basic_table[200];

/*Скетч использует 17004 байт (52%) памяти устройства. Всего доступно 32256 байт.
Глобальные переменные используют 791 байт (38%) динамической памяти, оставляя 1257 байт для локальных переменных. Максимум: 2048 байт.*/

/*Скетч использует 17044 байт (52%) памяти устройства. Всего доступно 32256 байт.
Глобальные переменные используют 1191 байт (58%) динамической памяти, оставляя 857 байт для локальных переменных. Максимум: 2048 байт.*/

void btfF(){
	for(int i=0; i<200; i++){
	    basic_table[i] = 255;
	}
}

// Hardware Serial on Mega, Leonardo, Micro...
// #define EspSerial Serial1

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

void setup()
{
	btfF();

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
	int test_val = basic_table[5] + 1;
	basic_table[1] = test_val;
}
