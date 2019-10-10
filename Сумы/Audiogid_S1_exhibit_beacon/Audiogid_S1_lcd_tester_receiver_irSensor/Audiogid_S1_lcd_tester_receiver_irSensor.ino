// ПРИЕМНИК ТЕСТЕР С ВЫВОДОМ ИНФОРМАЦИИ НА LCD  И В UART

#include <iarduino_IR_RX.h>                      // библиотека для работы с ИК-приёмником
#include <Wire.h>               // temp //
#include <LiquidCrystal_I2C.h>  // temp //

iarduino_IR_RX IR_RECEIVER(7);                            // объект IR_RECEIVER, с указанием вывода к которому подключён ИК-приёмник
LiquidCrystal_I2C lcd(0x27, 16, 2);  // temp //



uint16_t recCount = 0;
uint16_t mRecCount = 0;
uint16_t gCount = 0;

int16_t prevExhibitNumber = 0;

void setup()
{
	Serial.begin(115200);
	
	IR_RECEIVER.begin();                                    // Инициируем работу с ИК-приёмником
	IR_RECEIVER.protocol("AeQQV~zK]Kp^KJp[@@@@@@@Bp");      // протокол передачи данных от пульта ELENBERG

	lcd.begin();  // initialize the LCD
	delay(100);
	lcd.backlight();  // Turn on the blacklight and print a message.
	lcd.clear();
}

void loop()
{
	if(IR_RECEIVER.check()){                                // Если в буфере имеются данные, принятые с маячка
		Serial.println(IR_RECEIVER.data);                   // Выводим номер маячка
		Serial.println(IR_RECEIVER.length);                 // Выводим количество бит в коде

		recCount ++;
		if(IR_RECEIVER.data != prevExhibitNumber){
			mRecCount ++;
			prevExhibitNumber = IR_RECEIVER.data;
		}
		lcd.clear();                                        // Вывод на lcd 16*2
		lcd.print("n ");
		lcd.print(IR_RECEIVER.data);
		lcd.setCursor(7, 0);
		lcd.print("nc ");
		lcd.print(recCount);
		lcd.setCursor(7, 1);
		lcd.print("mc ");
		lcd.print(mRecCount);
		lcd.setCursor(0, 1);
		lcd.print("len ");
		lcd.print(IR_RECEIVER.length);
	}

	lcd.setCursor(15, 1);
	lcd.print(gCount);
	gCount ++;
	if(gCount > 9){
	    gCount = 0;
		lcd.setCursor(15, 1);
		lcd.print("0");
	}
}