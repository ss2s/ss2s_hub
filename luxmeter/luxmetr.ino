/*
Connection:
 VCC-5v
 GND-GND
 SCL-SCL(analog pin 5)
 SDA-SDA(analog pin 4)
 ADD-NC or GND
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BH1750.h"

#define MAX_DIAPAZON_BAR 200  // максимальный диапазон прогресс бара 65535
#define REFRESH_DELEY 500     // задержка обновления

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 I2C
BH1750 lightMeter;  // light sensor init I2C

byte customChar0[8] = {
	0b11011,
	0b11011,
	0b11011,
	0b11011,
	0b11011,
	0b11011,
	0b11011,
	0b11011
};

void luxBar(uint16_t _lux)
{
	lcd.setCursor(0, 1);
	_lux = constrain(_lux, 0, MAX_DIAPAZON_BAR);
	_lux = map(_lux, 0, MAX_DIAPAZON_BAR, 0, 16);  // 65535
	for(int i=0; i<_lux; i++){
	    
		lcd.write(byte(0));
	}
	lcd.print("                ");
}

void setup()
{
	// Serial.begin(115200);
	// Serial.println("luxmetr...\n\n");
	
	lcd.begin();  // initialize the LCD
	delay(100);
	lcd.createChar(0, customChar0);  // create a new custom character
	delay(100);
	lcd.backlight();  // Turn on the blacklight and print a message.
	lcd.clear();
	lcd.print("    luxmetr");

	lightMeter.begin();

	delay(1000);

	lcd.clear();
}

void loop()
{
	uint16_t lux = lightMeter.readLightLevel();

	// Serial.print("Light: ");
	// Serial.print(lux);
	// Serial.println(" lux");

	lcd.setCursor(0, 0);
	lcd.print("Light: ");
	lcd.print(lux);
	lcd.print(" lux");
	lcd.print("     ");

	luxBar(lux);

	delay(REFRESH_DELEY);
}