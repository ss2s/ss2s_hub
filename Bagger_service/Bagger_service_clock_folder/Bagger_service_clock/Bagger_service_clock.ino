// Bagger_service_clock Arduino 1.8.6

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// НАСТРОЙКИ:

// сигнализация
#define ALARM_TEMP 90            // порог срабатывания по температуре
#define ALARM_CO 200             // порог срабатывания по СО
#define REALARM_DIVIDER 20       // коефициент перезапуска сигналки, чем меньше тем больше перезапуск, перезапуск = (ALARM_TEMP - (ALARM_TEMP / REALARM_DIVIDER))
#define ALARM_CHEK_DELAY 1000    // мс между запусками сигнализации 1c (влияет на частоту писков)
#define BUZER_FREQ 10000         // частота звука бузера 10 КГц
#define BUZER_TIME 500           // длительность самого писка в одной секунде не больше ALARM_CHEK_DELAY
#define STRT_ALARM_DELAY 10000   // задержка срабатывания сигнализации при старте в мс

// время
#define TIME_UPDATE_DELAY 500    // задержка для обновления часов, влияет на скорость мигания точек
#define FIRST_ZERO 0             // 0-первый 0 если меньше 12 часов : 1-ничего
#define WEEK_LANG 0              // язык дня недели: 0 - английский, 1 - русский (транслит)

// показания вольтметра
#define VOLTAGE_CHEK_DELAY 1000  // мс между измерениями напряжения
#define VOLTAGE_COEF 68.2        // коефициент напряжения, не может быть 0 (1023 / (разрешение делителя))

// температура
#define TEMP_CHEK_DELAY 10000    // мс между измерениями температуры, чтоб датчик не нагревался (10000+(10+ секунд))

// СО
#define CO_CHEK_DELAY 1000       // мс между измерениями CO

// заставка
#define ENABLE_START_SAVER 1     // 1-екранная заставка при старте; 0-без заставки
#define SAVER_DELAY 1000         // задержка для заставки

// доп настройки
#define BUTTON_HIGH 1            // тип кнопки
#define RESET_CLOCK 0            // 1-сброс часов на время загрузки прошивки (для модуля с несъёмной батарейкой). Не забудь поставить 0 и прошить ещё раз!
#define DISPLAY_ADDR 0x27        // адрес платы дисплея: 0x27 или 0x3f

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  РАСПИНОВКА:

// термометр
#define DS18B20_DATA_PIN 2
#define DS18B20_VCC_PIN 3

#define BUZER_PIN 5

// пины кнопок управления
#define BTN_SET_PIN 7  // кнопка установки
#define BTN_H_PIN 8    // кнопка часов
#define BTN_M_PIN 9    // кнопка минут

#define VOLTAGE_PIN A0
#define CO_SENSOR_PIN A2

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// БИБЛИОТЕКИ И ОБЪЕКТЫ:
#include "Wire.h"
// дисплей
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(DISPLAY_ADDR, 20, 4);
// часы
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;
// термометр
#include <OneWire.h>
#include "DallasTemperature.h"
OneWire oneWire(DS18B20_DATA_PIN);    // Сообщаем библиотеке об устройстве, работающем по протоколу 1-Wire
DallasTemperature tempsensor(&oneWire);  // Связываем функции библиотеки DallasTemperature с нашим 1-Wire устройством (DS18B20)

#include "Utimer.h"
Utimer t1_time(TIME_UPDATE_DELAY);        // t1 init 1000ms
Utimer t2_voltage(VOLTAGE_CHEK_DELAY);    // t4 init 500ms
Utimer t3_temp(TEMP_CHEK_DELAY);          // t3 init 10s
Utimer t4_co(CO_CHEK_DELAY);              // t4 init 500ms
Utimer t5_alarm(ALARM_CHEK_DELAY);        // t5 init 1000ms

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// VARIABLES:
int8_t hrs, mins, secs;

#if (WEEK_LANG == 0)
static const char *dayNames[]  = {
  "SU",
  "MO",
  "TU",
  "WE",
  "TH",
  "FR",
  "SA",
};
#else
static const char *dayNames[]  = {
  "VS",
  "PN",
  "VT",
  "SR",
  "4T",
  "PT",
  "SB",
};
#endif

// цифры
uint8_t LT[8] = {0b00111,  0b01111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t UB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};
uint8_t RT[8] = {0b11100,  0b11110,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111};
uint8_t LL[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b01111,  0b00111};
uint8_t LB[8] = {0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};
uint8_t LR[8] = {0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11111,  0b11110,  0b11100};
uint8_t UMB[8] = {0b11111,  0b11111,  0b11111,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111};
uint8_t LMB[8] = {0b11111,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b11111,  0b11111};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loadClockChar() {
	lcd.createChar(0, LT);
	lcd.createChar(1, UB);
	lcd.createChar(2, RT);
	lcd.createChar(3, LL);
	lcd.createChar(4, LB);
	lcd.createChar(5, LR);
	lcd.createChar(6, UMB);
	lcd.createChar(7, LMB);
}

void drawDig(byte dig, byte x, byte y) {
  switch (dig) {
    case 0:
      lcd.setCursor(x, y); // set cursor to column 0, line 0 (first row)
      lcd.write(0);  // call each segment to create
      lcd.write(1);  // top half of the number
      lcd.write(2);
      lcd.setCursor(x, y + 1); // set cursor to colum 0, line 1 (second row)
      lcd.write(3);  // call each segment to create
      lcd.write(4);  // bottom half of the number
      lcd.write(5);
      break;
    case 1:
      lcd.setCursor(x, y);
      lcd.write(32);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(5);
      break;
    case 2:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(7);
      break;
    case 3:
      lcd.setCursor(x, y);
      lcd.write(6);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 4:
      lcd.setCursor(x, y);
      lcd.write(3);
      lcd.write(4);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(5);
      break;
    case 5:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(7);
      lcd.write(7);
      lcd.write(5);
      break;
    case 6:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(6);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 7:
      lcd.setCursor(x, y);
      lcd.write(1);
      lcd.write(1);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(0);
      lcd.write(32);
      break;
    case 8:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(3);
      lcd.write(7);
      lcd.write(5);
      break;
    case 9:
      lcd.setCursor(x, y);
      lcd.write(0);
      lcd.write(6);
      lcd.write(2);
      lcd.setCursor(x, y + 1);
      lcd.write(4);
      lcd.write(4);
      lcd.write(5);
      break;
    case 10:
      lcd.setCursor(x, y);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      lcd.setCursor(x, y + 1);
      lcd.write(32);
      lcd.write(32);
      lcd.write(32);
      break;
	}
}

void drawdots(byte x, byte y, boolean state = 1) {
	if (state){	
		lcd.setCursor(x, y);
		lcd.write(4);
		lcd.setCursor(x, y + 1);
		lcd.write(4);
	}
	else{	
		lcd.setCursor(x, y);
		lcd.write(32);
		lcd.setCursor(x, y + 1);
		lcd.write(32);
	}
}

void drawClock(byte hours, byte minutes, byte x, byte y, boolean dotState) {

	//if (hours > 23 || minutes > 59) return;
	if (hours / 10 == 0) drawDig(FIRST_ZERO * 10, x, y);
	else drawDig(hours / 10, x, y);
	drawDig(hours % 10, x + 4, y);

	// точки. Отдельной функцией
	drawdots(x+8, y, dotState);

	drawDig(minutes / 10, x + 10, y);
	drawDig(minutes % 10, x + 14, y);
}

void drawData(byte x, byte y) {
	lcd.setCursor(x, y);
	if (now.day() < 10) lcd.print(0);
	lcd.print(now.day());
	lcd.print(".");
	if (now.month() < 10) lcd.print(0);
	lcd.print(now.month());
	// lcd.print(".");
	// lcd.print(now.year());
	lcd.print(" ");
	int dayofweek = now.dayOfTheWeek();
	// lcd.print(dayofweek);
	lcd.print(dayNames[dayofweek]);
}

void drawLogo(){
	lcd.setCursor(1, 0); // (символ(0-19), строка(0-3))
	lcd.print(F("NISSAN"));
}

boolean dots_flag = 0;
boolean dots_counter_flag = 0;
void drawDataTime(){
	now = rtc.now();
	secs = now.second();
	mins = now.minute();
	hrs = now.hour();

	// drawData(0, 0);
	drawClock(hrs, mins, 1, 1, dots_flag);
	dots_counter_flag = !dots_counter_flag;
	if(dots_counter_flag){dots_flag = !dots_flag;}
}

float voltage_val = 0;
float moving_average[] = {6, 6, 6, 6, 6};
float voltage_sum = 0;
void drawVoltage(byte x = 13, byte y = 0) {
	voltage_sum = 0;
	voltage_val = analogRead(VOLTAGE_PIN);
	voltage_val = voltage_val / VOLTAGE_COEF;
	for(int i=0; i<5; i++){
		if(i<4){
	    	moving_average[i] = moving_average[i+1];
	    	voltage_sum += moving_average[i];
		}
		else{
			moving_average[i] = voltage_val;
			voltage_sum += moving_average[i];
		}
	}
	voltage_val = voltage_sum / 5;
	lcd.setCursor(x, y);
	if(voltage_val < 10){lcd.print(" ");}
	lcd.print(voltage_val, 1);
	lcd.print(" ");
	lcd.setCursor(x+5, y);
	lcd.print("V");
}

void drawVoltage2(){
	drawVoltage();
}

boolean alarm_latch_flag = 1;
boolean temp_alarm_latch_flag = 0;
boolean co_alarm_latch_flag = 0;

void alarmSignal(){
	if(millis() < STRT_ALARM_DELAY){
	    alarm_latch_flag = 0;
	}
	if(alarm_latch_flag == 1 && (temp_alarm_latch_flag == 1 || co_alarm_latch_flag == 1)){
		tone(BUZER_PIN, BUZER_FREQ, BUZER_TIME);
	}
}

// термометр: функция запроса температуры
int16_t termometer_error = 0;
float temperature(){
	// Измеряем температуру 10 секунд

	digitalWrite(DS18B20_VCC_PIN, HIGH);              // Включаем питание датчика температуры
	delay(100);                                       // Задержка перед первым измерением 

	t1_time.run(drawDataTime);

	tempsensor.requestTemperatures();                 // Запрос на измерение температуры (1-й ошибочный)
	delay(500);                                       // Задержка перед поторным измерением

	t1_time.run(drawDataTime);

	tempsensor.requestTemperatures();                 // Запрос на измерение температуры (повторный)
	float t = float(tempsensor.getTempCByIndex(0));   // Получаем значение температуры
	digitalWrite(DS18B20_VCC_PIN, LOW);               // Отключаем питание датчика температуры

	if(t == 85 || t == -127){termometer_error = t;}
	else{termometer_error = 0;}

	if(termometer_error != 0){t = -128;}

	return(t);                                        // Возвращаем значение температуры в место вызова функции
}

void processingTemp(){
	int _temp = temperature();
	lcd.setCursor(1, 3);
	lcd.print("t ");
	lcd.print(_temp);
	lcd.print(" ");
	if(_temp >= ALARM_TEMP){
		temp_alarm_latch_flag = 1;
	}else if(_temp < (ALARM_TEMP - (ALARM_TEMP / REALARM_DIVIDER))){
		temp_alarm_latch_flag = 0;
		if(co_alarm_latch_flag == 0){
			alarm_latch_flag = 1;
		}
	}
}

void processing_CO(){
	int _CO = analogRead(CO_SENSOR_PIN);
	lcd.setCursor(13, 3);
	lcd.print("CO ");
	lcd.print(_CO);
	lcd.print(" ");
	if(_CO >= ALARM_CO){
		co_alarm_latch_flag = 1;
	}else if(_CO < (ALARM_CO - (ALARM_CO / REALARM_DIVIDER))){
		co_alarm_latch_flag = 0;
		if(temp_alarm_latch_flag == 0){
			alarm_latch_flag = 1;
		}
	}
}

// This line sets the RTC with an explicit date & time, for example to set
// January 21, 2014 at 3am you would call:
// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
void setTime(char _t){
	int _h = now.hour();
	int _m = now.minute();
	if(_t == 'h'){
		_h += 1;
		if(_h > 23){_h = 0;}
	}
	else if(_t == 'm'){
		_m += 1;
		if(_m > 59){_m = 0;}
	}
	rtc.adjust(DateTime(now.year(), now.month(), now.day(), _h, _m, now.second()));
}

boolean btn_set = 0;
boolean btn_h = 0;
boolean btn_m = 0;

void processing_button(){
	btn_set = digitalRead(BTN_SET_PIN);
	btn_h = digitalRead(BTN_H_PIN);
	btn_m = digitalRead(BTN_M_PIN);

	if(btn_set == !BUTTON_HIGH && btn_h == BUTTON_HIGH && btn_m == BUTTON_HIGH){  // отключаем сигнализацию
		alarm_latch_flag = 0;
	}

	if(btn_set == BUTTON_HIGH){
	    if(btn_h == BUTTON_HIGH && btn_m == !BUTTON_HIGH){  // h
	    	setTime('h');
	    }
	    else if(btn_h == !BUTTON_HIGH && btn_m == BUTTON_HIGH){  // m
	    	setTime('m');
	    }
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup(){

	pinMode(DS18B20_DATA_PIN, INPUT);
	pinMode(DS18B20_VCC_PIN, OUTPUT);
	pinMode(BUZER_PIN, OUTPUT);
	pinMode(BTN_SET_PIN, INPUT);
	pinMode(BTN_H_PIN, INPUT);
	pinMode(BTN_M_PIN, INPUT);

	// дисплей
	lcd.begin();
	lcd.backlight();
	lcd.clear();
	loadClockChar();
	delay(5);

	// заставка
	#if ENABLE_START_SAVER == 1
	lcd.setCursor(4, 1); // (символ(0-19), строка(0-3))
	lcd.print(F("N I S S A N"));
	delay(SAVER_DELAY);
	lcd.clear();
	#endif

	// часы
	rtc.begin();
	if (RESET_CLOCK || rtc.lostPower())
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

	// термометр
	pinMode(DS18B20_VCC_PIN, OUTPUT);  // Определяем пин подключения питания датчика температуры
	tempsensor.begin();                // Запускаем библиотеку измерения температуры

	drawLogo();
	drawDataTime();
	processingTemp();
	drawVoltage();
	processing_CO();
}

void loop(){
	t1_time.run(drawDataTime);
	t2_voltage.run(drawVoltage2);
	t3_temp.run(processingTemp);
	t4_co.run(processing_CO);
	t5_alarm.run(alarmSignal);

	processing_button();
}