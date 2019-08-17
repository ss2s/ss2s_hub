// bird house V4
// Arduino 1.8.1
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ВКЛЮЧЕНИЯ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "setting_config.h"  // файл с настройками

#include <EEPROM.h>

#include <LiquidCrystal.h>
LiquidCrystal lcd(DEF_LCD_INIT_PIN);  // инициализация дисплея с кнопками, настройки в файле конфигурации

#include <OneWire.h>
#include "DallasTemperature.h"
OneWire oneWire(DS18B20_DATA_PIN);    // Сообщаем библиотеке об устройстве, работающем по протоколу 1-Wire
DallasTemperature tempsensor(&oneWire);  // Связываем функции библиотеки DallasTemperature с нашим 1-Wire устройством (DS18B20)

#include <Wire.h>
#include "DS3231.h"

DS3231 clock;                 // Связываем объект clock с библиотекой DS3231
RTCDateTime DateTime;         // Определяем сущность структуры RTCDateTime (описанной в библиотеке DS3231) для хранения считанных с часов даты и времени
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// МАКРОСЫ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if RELE_SIGNAL_TIPE == 0
	#define RELE_HIGH 0
	#define RELE_LOW 1
#else
	#define RELE_HIGH 1
	#define RELE_LOW 0
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ПЕРЕМЕННЫЕ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// часы
int16_t rsecond;

int16_t rminute = 61;  // Переменная для отслеживания изменения минут

int16_t rhour;         // переменная для хранения часов

int16_t rday;
int8_t old_rday;

int16_t rmonth;

int16_t ryear;

// температура
float curent_temp = 0;

// параметры таблицы
/* температура 0, световой день 1, вентиляция ВКЛ 2, вентиляция ВЫКЛ 3 */
float table_temp = 24;
uint16_t table_light = 20;
uint16_t table_vent_on = 1;
uint16_t table_vent_off = 5;

// флаги
bool flag_day_changed_state = 0;  // флаг перезаписывается при смене дня в 1, и через 12 часов после смены дня в 0
bool flag_heat_on = 0;  // нагрев включен
bool flag_light_on = 0;  // свет включен
bool flag_cooler_on = 0;  // вентиляция включена по таймеру
bool flag_cooler_on_overheating = 0;  // вентиляция включена перегрев
bool flag_cooler_on_timer = 0;   // вентиляция включена по таймеру

bool start_flag_1 = 1;

int16_t termometer_error = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ПОЛЬЗОВАТЕЛЬСКИЕ СИМВОЛЫ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte customCharRightArrow3[8] = {  // стрелка вправо для дисплея
	0b00000,
	0b00000,
	0b00100,
	0b00010,
	0b11111,
	0b00010,
	0b00100,
	0b00000
};

byte customCharMenuArrow4[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100
};

byte customCharChasi5[8] = {
	0b00000,
	0b00000,
	0b01110,
	0b10101,
	0b10111,
	0b10001,
	0b01110,
	0b00000
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИИ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ОПРЕДЕЛЕНИЯ ФУНКЦИЙ
void displayLoopStatic();

// СЕТАПЫ

// реле сетап
void releSetup()
{
	pinMode(RELE_HEATER_PIN, OUTPUT);
	pinMode(RELE_COOLER_PIN, OUTPUT);
	pinMode(RELE_LIGHTER_PIN, OUTPUT);

	digitalWrite(RELE_HEATER_PIN, RELE_LOW);
	digitalWrite(RELE_COOLER_PIN, RELE_LOW);
	digitalWrite(RELE_LIGHTER_PIN, RELE_LOW);
}

// сетап EEPROM
void eepromSetup()
{
	#if SET_DAY_CONTROL_VAL_EN == 1
	EEPROM.write(R_ADDR, day_control_val);
	#endif
	day_control_val = EEPROM.read(R_ADDR);
}

// сетап термометра
void termometerSetup()
{
	tempsensor.begin();                    // Запускаем библиотеку измерения температуры
	pinMode(DS18B20_VCC_PIN, OUTPUT);        // Определяем пин подключения питания датчика температуры
}

// сетап часов
void clockSetup()
{
	clock.begin();         // Инициализируем работу с объектом библиотеки DS3231
	#if SET_CLOK_FOR_PROG == 1
 	clock.setDateTime(__DATE__, __TIME__);      // Устанавливаем время на часах, основываясь на времени компиляции скетча
	//clock.setDateTime(2016, 9, 15, 0, 0, 0);      // Установка времени вручную (Год, Месяц, День, Час, Минута, Секунда)
	#endif
}

// сетап дисплея
void displaySetup()
{
	pinMode(DEF_LCD_LIGHT_PIN, INPUT);
	analogWrite(DEF_LCD_LIGHT_PIN, screen_brightnes);
	
	lcd.createChar(3, customCharRightArrow3);
  	lcd.createChar(4, customCharMenuArrow4);
  	lcd.createChar(5, customCharChasi5);
  	delay(200);
	lcd.begin(16, 2);  // initialize the LCD
	delay(200);
	lcd.clear();
	lcd.setCursor(3,0);
	lcd.print(F("BIRD HOUSE"));
	lcd.setCursor(3,1);
	lcd.print(F("controller"));
	delay(1000);
}

// ЛУПЫ

// термометр: функция запроса температуры
float temperature()
{
	// Измеряем температуру 10 секунд
	if(start_flag_1 == 0){delay(MEASUREMENT_DELAY);}  // Задержка, чтобы датчик не нагревался от частых измерений
	else{start_flag_1 = 0;}

	digitalWrite(DS18B20_VCC_PIN, HIGH);              // Включаем питание датчика температуры
	delay(100);                                       // Задержка перед первым измерением 
	tempsensor.requestTemperatures();                 // Запрос на измерение температуры (1-й ошибочный)
	delay(500);                                       // Задержка перед поторным измерением
	tempsensor.requestTemperatures();                 // Запрос на измерение температуры (повторный)
	float t = float(tempsensor.getTempCByIndex(0));   // Получаем значение температуры
	digitalWrite(DS18B20_VCC_PIN, LOW);               // Отключаем питание датчика температуры

	if(t == 85 || t == -127){
		termometer_error = t;

		analogWrite(DEF_LCD_LIGHT_PIN, 1);
		delay(1000);
		analogWrite(DEF_LCD_LIGHT_PIN, screen_brightnes);
		delay(500);
		analogWrite(DEF_LCD_LIGHT_PIN, 1);
		delay(500);
		analogWrite(DEF_LCD_LIGHT_PIN, screen_brightnes);
		delay(500);
		analogWrite(DEF_LCD_LIGHT_PIN, 1);
		delay(500);
		analogWrite(DEF_LCD_LIGHT_PIN, screen_brightnes);
		delay(500);
		analogWrite(DEF_LCD_LIGHT_PIN, 1);
		delay(1000);
		analogWrite(DEF_LCD_LIGHT_PIN, screen_brightnes);
	}
	else{termometer_error = 0;}

	return(t);                                        // Возвращаем значение температуры в место вызова функции
}

// часы: функция запроса времени
void chekVremya(){
	DateTime = clock.getDateTime();  // Считываем c часов текущие значения даты и времени в сущность DateTime
	rsecond = int(DateTime.second);
	rminute = int(DateTime.minute);  //  Меняем значение в переменной отслеживания минут на текущее
	rhour = int(DateTime.hour);
	rday = byte(DateTime.day);
	rmonth = int(DateTime.month);
	ryear = int(DateTime.year);
}

// функция запроса параметров из таблицы
void checkTableParam()
{
	/* температура 0, световой день 1, вентиляция ВКЛ 2, вентиляция ВЫКЛ 3 */
	table_temp = float(basic_schedule[day_control_val][0]);
	table_light = int(basic_schedule[day_control_val][1]);
	table_vent_on = uint32_t(basic_schedule[day_control_val][2]);
	table_vent_off = uint32_t(basic_schedule[day_control_val][3]);
}

// функция проверки кнопок
uint8_t key()
{   // 1-723, 2-482, 3-133, 4-310, 5-0;
	int16_t val = analogRead(A0);
	uint8_t keyVal = 0;
	if(val < 67) keyVal = 5;             // right
	else if(val < 221) keyVal = 4;       // up
	else if(val < 396) keyVal = 3;       // down
	else if(val < 602) keyVal = 2;       // left
	else if(val < 873) keyVal = 1;       // select // 873
	else if(val <= 1023) keyVal = 0;     // no press
	// Serial.print(keyVal);
	// Serial.print(" ");
	// Serial.println(val);
	return keyVal;
}

// сохранить в EEPROM
void saveToMemory()
{
	if(day_control_val != EEPROM.read(R_ADDR)){
		EEPROM.write(R_ADDR, day_control_val);
	}
}
void saveToMemory2()
{
	if(rday != EEPROM.read(R_ADDR_2)){
		EEPROM.write(R_ADDR_2, rday);
	}
}

// обработчик нажатий кнопок в цикле menu
bool buttonChekForMenuChangeDay()
{	
	uint8_t val = key();
	uint32_t buttonDelay = 300;  // задержка для меню

	if(val > 0){
		delay(buttonDelay);
	}

	if(val == 1){        // select
		saveToMemory();            // save to EEPROM
		return 0;                  // exit  
	}
	else if(val == 2){  // left
		day_control_val -= 1;
	}
	else if(val == 3){  // down
		// day_control_val -= 10;
	}
	else if(val == 4){  // up
		// day_control_val += 10;
	}
	else if(val == 5){  // right
		day_control_val += 1;
	}

	if(day_control_val > MAX_DAY_CONTROL_VAL){
		day_control_val = 1;
	}
	else if(day_control_val < 1){
		day_control_val = MAX_DAY_CONTROL_VAL;
	}

	return 1;
}

uint8_t prev_day_control_val = 99;
void displayMenuVal()
{
	if(day_control_val != prev_day_control_val){
		lcd.setCursor(9, 0);
		lcd.print("  ");
		lcd.setCursor(9, 0);
		lcd.print(day_control_val);
		prev_day_control_val = day_control_val;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ НАСТРОЙКИ ЧАСОВ ИЗ МЕНЮ
// функция рисования для функции настройки даты из меню
inline void drawDataSetForMenu(int _day = 27, int _month = 11, int _year =2017){

	int ddsDay = _day;
	int ddsMonth = _month;
	int ddsYear = _year;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(5));
	lcd.setCursor(2,0);
	lcd.print(ddsDay);
	lcd.setCursor(5,0);
	lcd.print(ddsMonth);
	lcd.setCursor(8,0);
	lcd.print(ddsYear);
	lcd.setCursor(14,0);
	lcd.print(F("OK"));
}

// функция рисования для функции настройки часов из меню
inline void drawTimeSetForMenu(int _hour = 18, int _minute = 30, int _second = 30){

	int dtsHour = _hour;
	int dtsMinute = _minute;
	int dtsSecond = _second;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.write(byte(5));
	lcd.setCursor(2,0);
	lcd.print(dtsHour);
	lcd.setCursor(5,0);
	lcd.print(dtsMinute);
	lcd.setCursor(8,0);
	lcd.print(dtsSecond);
	lcd.setCursor(12,0);
	lcd.print(F("SAVE"));
}

void timeSetForMenu(){
	// rday;
	// rmonth;
	// ryear;
	// rhour;
	// rminute;
	// rsecond;
	unsigned int buttonDelay = 200;  // задержка для меню
	bool tsCikl = 1;  // переменная для управления циклом while
	byte tsVirtualPos = 1;   // виртуальная позиция указателя меню
	byte tsRealPos = 0;   // реальная позиция указателя меню
	byte tsKey = 0;   // значение кнопок для обработки в цикле while

	chekVremya();
	drawDataSetForMenu(rday, rmonth, ryear);
	lcd.setCursor(0,1);
	lcd.write(byte(4));

	while(tsCikl){

		tsKey = key();
		if(tsKey > 0){

		    if(tsKey == 1){  // s
		    	if(tsVirtualPos == 1){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsCikl = 0;
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);

		    }else if(tsKey == 2){  // l
		    	tsVirtualPos --;
		    	if(tsVirtualPos < 1){tsVirtualPos = 1;}
		    	delay(buttonDelay);

		    }else if(tsKey == 3){  // d
		    	if(tsVirtualPos == 1){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 2){
		    		rday --;
		    		if(rday<1){rday = 31;}
		    	}else if(tsVirtualPos == 3){
		    		rmonth --;
		    		if(rmonth<1){rmonth = 12;}
		    	}else if(tsVirtualPos == 4){
		    		ryear --;
		    		if(ryear<2000){ryear = 2222;}
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsCikl = 0;
		    	}else if(tsVirtualPos == 7){
		    		rhour --;
		    		if(rhour<0){rhour = 23;}
		    	}else if(tsVirtualPos == 8){
		    		rminute --;
		    		if(rminute<0){rminute = 59;}
		    	}else if(tsVirtualPos == 9){
		    		rsecond --;
		    		if(rsecond<0){rsecond = 59;}
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);

		    }else if(tsKey == 4){  // u
		    	if(tsVirtualPos == 1){
		    		delay(buttonDelay);
					// ПЕРЕЙТИ НА УРОВЕНЬ ВЫШЕ
		    	}else if(tsVirtualPos == 2){
		    		rday ++;
		    		if(rday>31){rday = 1;}
		    	}else if(tsVirtualPos == 3){
		    		rmonth ++;
		    		if(rmonth>12){rmonth = 1;}
		    	}else if(tsVirtualPos == 4){
		    		ryear ++;
		    		if(ryear>2222){ryear = 2000;}
		    	}else if(tsVirtualPos == 5){
		    		tsVirtualPos = 6;
		    	}else if(tsVirtualPos == 6){
		    		tsVirtualPos = 1;
		    	}else if(tsVirtualPos == 7){
		    		rhour ++;
		    		if(rhour>23){rhour = 0;}
		    	}else if(tsVirtualPos == 8){
		    		rminute ++;
		    		if(rminute>59){rminute = 0;}
		    	}else if(tsVirtualPos == 9){
		    		rsecond ++;
		    		if(rsecond>59){rsecond = 0;}
		    	}else if(tsVirtualPos == 10){
		    		clock.setDateTime(ryear, rmonth, rday, rhour, rminute, rsecond);  // Установка времени (Г,М,Д,Ч,М,С)
		    		tsCikl = 0;
		    	}
		    	delay(buttonDelay);
		    	
		    }else if(tsKey == 5){  // r
		    	tsVirtualPos ++;
		    	if(tsVirtualPos > 10){tsVirtualPos = 10;}
		    }
		    delay(buttonDelay);

		    switch (tsVirtualPos){
			    case 1:
			      tsRealPos = 0;
			      break;
			    case 2:
			      tsRealPos = 2;
			      break;
			    case 3:
			      tsRealPos = 5;
			      break;
			    case 4:
			      tsRealPos = 9;
			      break;
			    case 5:
			      tsRealPos = 14;
			      break;
			    case 6:
			      tsRealPos = 0;
			      break;
			    case 7:
			      tsRealPos = 2;
			      break;
			    case 8:
			      tsRealPos = 5;
			      break;
			    case 9:
			      tsRealPos = 8;
			      break;
			    case 10:
			      tsRealPos = 13;
			      break;
			}

			if(tsVirtualPos<=5){
				drawDataSetForMenu(rday, rmonth, ryear);
			}else if(tsVirtualPos>5){
				drawTimeSetForMenu(rhour, rminute, rsecond);
			}
			lcd.setCursor(tsRealPos,1);
			lcd.write(byte(4));
			if(tsVirtualPos == 5){
				lcd.write(byte(3));
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void menuChangeDay()
{
	prev_day_control_val = 99;
	lcd.clear();
	lcd.setCursor(6, 0);
	lcd.print(F("MENU"));
	delay(5000);
	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print(F("set day"));
	lcd.setCursor(1, 1);
	lcd.print(F("select to save"));

	while(buttonChekForMenuChangeDay()){
		displayMenuVal();
	}

	lcd.clear();
	lcd.setCursor(5, 0);
	lcd.print(F("SAVED"));
	delay(2000);

	timeSetForMenu();

	displayLoopStatic();
}

// обработчик нажатий кнопок в цикле loop
void buttonChekForLoop()
{	
	uint8_t val = key();
	uint32_t buttonDelay = 200;  // задержка для меню

	if(val > 0){
		delay(buttonDelay);
	}

	if(val == 1){  // select
		menuChangeDay();
	}else if(val == 2){  // left
	}else if(val == 3){  // down
	}else if(val == 4){  // up
	}else if(val == 5){  // right
	}
}

// вывод информации на дисплей статик
void displayLoopStatic()
{
	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print(F("D"));

	lcd.setCursor(8, 0);
	lcd.print(F(":"));

	lcd.setCursor(11, 0);
	lcd.print(F("  HLC"));

	lcd.setCursor(0, 1);
	lcd.print(F("temp"));
}
// вывод информации на дисплей динамик
void displayLoop()
{
	// curent day
	lcd.setCursor(2, 0);
	lcd.print(F("  "));
	lcd.setCursor(2, 0);
	lcd.print(day_control_val);

	// time
	lcd.setCursor(6, 0);
	lcd.print(F("  "));
	lcd.setCursor(6, 0);
	if(rhour < 10){lcd.print(F("0"));}
	lcd.print(rhour);
	// :
	lcd.setCursor(9, 0);
	lcd.print(F("  "));
	lcd.setCursor(9, 0);
	if(rminute < 10){lcd.print(F("0"));}
	lcd.print(rminute);

	// HLC
	lcd.setCursor(11, 1);
	lcd.print(F("     "));
	lcd.setCursor(13, 1);
	lcd.print(flag_heat_on);
	lcd.print(flag_light_on);
	lcd.print(flag_cooler_on);

	// curent temp
	lcd.setCursor(6, 1);
	lcd.print(F("     "));
	lcd.setCursor(6, 1);
	if(termometer_error == 0){
		lcd.print(curent_temp);
	}
	else{
		lcd.print(F("er"));
		lcd.print(termometer_error);
	}
}

void heatControl()
{
	if(termometer_error != 0){
		digitalWrite(RELE_HEATER_PIN, RELE_LOW);
		flag_heat_on = 0;
		return;
	}

	if(day_control_val == 0){
		digitalWrite(RELE_HEATER_PIN, RELE_LOW);
		flag_heat_on = 0;
		return;
	}

	if(curent_temp < (table_temp - heat_histerezis)){
		// включить нагреватель
		digitalWrite(RELE_HEATER_PIN, RELE_HIGH);
		flag_heat_on = 1;
	}
	else if(curent_temp >= table_temp){
		// выключить нагреватель
		digitalWrite(RELE_HEATER_PIN, RELE_LOW);
		flag_heat_on = 0;
	}
}

void lightControl()
{
	if(day_control_val == 0){
		digitalWrite(RELE_LIGHTER_PIN, RELE_LOW);
		flag_light_on = 0;
		return;
	}

	uint8_t l_STARToff_time = 1;
	uint8_t l_STOPoff_time = (24 + 1) - table_light;
	if(rhour >= l_STARToff_time && rhour < l_STOPoff_time){
		digitalWrite(RELE_LIGHTER_PIN, RELE_LOW);
		flag_light_on = 0;
	}
	else{
		digitalWrite(RELE_LIGHTER_PIN, RELE_HIGH);
		flag_light_on = 1;
	}
}

uint32_t start_timer_1 = 0;
uint32_t timer_set_val = 0;
void ventControl()
{
	if(termometer_error != 0){
		digitalWrite(RELE_COOLER_PIN, RELE_LOW);
		flag_cooler_on = 0;
		// Serial.print(F("\nds_err != 0; return;\n"));
		// delay(1000);
		return;
	}

	if(day_control_val == 0){
		digitalWrite(RELE_COOLER_PIN, RELE_LOW);
		flag_cooler_on = 0;
		flag_cooler_on_overheating = 0;
		flag_cooler_on_timer = 0;
		// Serial.print(F("\nDay = 0; return;\n"));
		// delay(1000);
		return;
	}

	if(table_vent_on == 0){
		digitalWrite(RELE_COOLER_PIN, RELE_LOW);
		flag_cooler_on = 0;
		flag_cooler_on_overheating = 0;
	    flag_cooler_on_timer = 0;
	    // Serial.print(F("\nTable param = 0; return;\n"));
		// delay(1000);
	    return;
	}

	if(curent_temp > (table_temp + cooling_histerezis)){
		digitalWrite(RELE_COOLER_PIN, RELE_HIGH);
		flag_cooler_on = 1;
		flag_cooler_on_overheating = 1;
		flag_cooler_on_timer = 0;
		// Serial.print(F("\n*overheat; vent ON;\n"));
		// delay(1000);
	}
	else if(curent_temp <= table_temp){


		if(flag_cooler_on_overheating == 1){
			digitalWrite(RELE_COOLER_PIN, RELE_LOW);
			flag_cooler_on = 0;
			flag_cooler_on_overheating = 0;
		    flag_cooler_on_timer = 1;

		    start_timer_1 = millis();

		    // Serial.print(F("\n*not overheat; vent OFF;\n"));
			// delay(1000);
		}
		else if((flag_cooler_on_timer == 1) && (millis() - start_timer_1) >= (timer_set_val)){
			timer_set_val = table_vent_off * 1000UL * 60UL;
			digitalWrite(RELE_COOLER_PIN, RELE_LOW);
			flag_cooler_on = 0;
			flag_cooler_on_overheating = 0;
		    flag_cooler_on_timer = 0;

		    start_timer_1 = millis();

		    // Serial.print(F("\n_timer; vent OFF;\n"));

		    // Serial.print(F("_time = "));
		    // Serial.print(table_vent_off);
		    // Serial.print(F("mn : "));
		    // Serial.print(timer_set_val);
		    // Serial.print(F("ms;\n"));

			// delay(1000);
		}
		else if((flag_cooler_on_timer == 0) && (millis() - start_timer_1) >= (timer_set_val)){
			timer_set_val = table_vent_on * 1000UL * 60UL;
			digitalWrite(RELE_COOLER_PIN, RELE_HIGH);
			flag_cooler_on = 1;
			flag_cooler_on_overheating = 0;
		    flag_cooler_on_timer = 1;

		    start_timer_1 = millis();

		    // Serial.print(F("\n_timer; vent ON;\n"));

		    // Serial.print(F("_time = "));
		    // Serial.print(table_vent_on);
		    // Serial.print(F("mn : "));
		    // Serial.print(timer_set_val);
		    // Serial.print(F("ms;\n"));

			// delay(1000);
		}
		// Serial.print(F("\n_curent time = "));
		// Serial.print(millis() - start_timer_1);
		// Serial.print(F("ms;\n"));
	}
}

void changeDayControlSetup()
{
	chekVremya();
	old_rday = EEPROM.read(R_ADDR_2);
	if(rday != old_rday){
		EEPROM.write(R_ADDR_2, rday);

		day_control_val += 1;
		saveToMemory();
		old_rday = rday;

	}else if(day_control_val == 0){

		day_control_val += 1;
		saveToMemory();
	}
	
}
void changeDayControl()
{
	if(day_control_val == 0){return;}

	if(rday != old_rday){
		old_rday = rday;

		if(day_control_val != 0){
			day_control_val += 1;
			saveToMemory();
			saveToMemory2();
		}
		if(day_control_val > MAX_DAY_CONTROL_VAL){
			day_control_val = 0;
			saveToMemory();
			saveToMemory2();
		}
	}
}

// управление нагрузками
void generalControlForLoop()
{
	chekVremya();
	checkTableParam();
	curent_temp = temperature();

	heatControl();
	lightControl();
	ventControl();
	changeDayControl();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ГЛАВНЫЙ АЛГОРИТМ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
	releSetup();
	clockSetup();
	eepromSetup();
	displaySetup();
	termometerSetup();
	changeDayControlSetup();

	displayLoopStatic();

	// Serial.begin(9600);
	// Serial.print(F("\nRESTART\n"));
	// Serial.print(("Day "));
	// Serial.print(day_control_val);
	// Serial.print("\n");
}

void loop()
{
	buttonChekForLoop();
	displayLoop();
	generalControlForLoop();
	// delay(2000);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////