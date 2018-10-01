// термопара t1
#define T1_TERMOPARA_SENSOR_SO_PIN 5
#define T1_TERMOPARA_SENSOR_CS_PIN 6
#define T1_TERMOPARA_SENSOR_SCK_PIN 7
// термопара t2
#define T2_TERMOPARA_SENSOR_SO_PIN 8
#define T2_TERMOPARA_SENSOR_CS_PIN 9
#define T2_TERMOPARA_SENSOR_SCK_PIN 10

int t1TermoparaSetMaxTemp = 200;
int t2TermoparaSetMaxTemp = 200;
float t1TermoparaCurentTemp = 0;
float t2TermoparaCurentTemp = 0;

#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x3F, 20, 4); // установка адреса 0x3F и разрешения 20/4

#include "max6675.h"     // https://github.com/adafruit/MAX6675-library

MAX6675 thermocouple1(T1_TERMOPARA_SENSOR_SCK_PIN, T1_TERMOPARA_SENSOR_CS_PIN, T1_TERMOPARA_SENSOR_SO_PIN);
MAX6675 thermocouple2(T2_TERMOPARA_SENSOR_SCK_PIN, T2_TERMOPARA_SENSOR_CS_PIN, T2_TERMOPARA_SENSOR_SO_PIN);

// проверка температуры Т1
void t1TermoparaCheckTemp(){
	t1TermoparaCurentTemp = thermocouple1.readCelsius();
}
// проверка температуры Т1
void t2TermoparaCheckTemp(){
	t2TermoparaCurentTemp = thermocouple2.readCelsius();
}

void setup(){
	lcd.begin(); // иниализация дисплея LCD 16/2
	lcd.clear(); // очистить дисплей
	lcd.backlight(); // включение подсветки
	lcd.setCursor(0,1);
	lcd.print("      SOLDERING");
	lcd.setCursor(0,2);
	lcd.print("       STATION");

	while(1){}

	lcd.clear(); // очистить дисплей
}

void loop(){
}