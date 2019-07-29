// КОРМУШКА ДЛЯ РЫБ на Arduino Atmega328 + ESP8266 v1.0

// НАСТРОЙКИ В ФАЙЛЕ "setting_config.h"

// настройка ESP8266:
// настройка UART на 9600, в ESP8266 должна стоять заводская AT прошивка
// настройка осуществляется через AT команды при подключении ESP к компу через переходник на скорости 115200
// это команда для настройки: AT+UART_DEF=9600,8,1,0,0

/*************************************************************
WARNING!
It's very tricky to get it working. Please read this article:
http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware
*************************************************************/

#define BLYNK_NOWHILE_PATCH  // эта строка отключает зависание блинка при отсутствии сети (в модифицированой библиотеке)

/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// #include "setting_config.h"  // файл с настройками

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// Hardware Serial on Uno, Nano...
// #define EspSerial Serial

// or Software Serial on Uno, Nano...
// #include <SoftwareSerial.h>
// SoftwareSerial EspSerial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blynk objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetLED B_LED_bunkerCondition(V21);  // blynk led widget состояние питающего бункера
WidgetRTC B_rtc;
ESP8266 wifi(&EspSerial);
BlynkTimer timersynctime;
BlynkTimer timergfedlop;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #include "feeding_control.h" // файл с алгоритмом управления кормлением

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <EEPROM.h>
#include <Servo.h>
#include "libraries/Utimer/Utimer.h"

#include "setting_config.h"  // файл с настройками
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pinout:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// esp01 connect hard Serial1 pin
#define HARD_SERIAL1_RX_PIN 19
#define HARD_SERIAL1_TX_PIN 18

// часы
#define DS3231_DATA_PIN 20
#define DS3231_CLK_PIN 21

// STEPPER PINOUT
// stepper step dir driver
#define STEPPER_EN_PIN 34    // шаговый двигатель en pin
#define STEPPER_STEP_PIN 36  // шаговый двигатель step pin
#define STEPPER_DIR_PIN 38   // шаговый двигатель dir pin
//  OR
// stepper l298 driver
#define C1_EN_PIN 34
#define C1_A_PIN 36
#define C1_B_PIN 38
#define C2_A_PIN 40
#define C2_B_PIN 42
#define C2_EN_PIN 44

// servo
#define SERVO_PIN 33

// реле
#define RELE_SPREADER_PIN 35  // реле разбрасывателя
#define RELE_VIBRATOR_PIN 37  // реле вибратора

// весы
#define HX711_DOUT_PIN 39  // ВЕСЫ DT
#define HX711_PD_SCK_PIN 41  // ВЕСЫ SCK

// светодиоды
#define GREEN_LED_PIN 5
#define RED_LED_PIN 7

// физические кнопки
#define FEED_BUTTON_PIN 46                // кнопка покормить
#define RESUME_BUTTON_PIN 47              // кнопка возобновления работы после аварийной остановки
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM adres (хватит примерно на 45 лет, желательно заменить через 40 лет, к каждому адресу добавить последний адрес + 4)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EEPROM_SETUP_KEY_ADDR 0           // ключ перезаписи
#define GENERAL_CONTROL_DAY_ADDR 4        // день контроллера
#define REMAINING_WEIGHT_ADDR 8           // остаточный вес в бункере при ошибке пустой бункер
#define CLOUD_FEED_WEIGHT_ADDR 16         // вес одного кормления из облака
#define CALIBRATION_FACTOR_ADDR 20        // калибровочный фактор для весов
#define CALIBRATION_WEIGHT_ADDR 24        // калибровочный вес для автокалибровки
#define FEED_BUNKER_CONDITION_ADDR 28     // состояние бункера: 0-пустой, 1-полный
#define OLD_DAY_ADDR 32                   // предыдущий день, для отслеживания изменения дня
#define FED_FOR_TODAY_ADDR 36             // скормлено за сегодня
#define TO_TABLE_CLOUD_WEIGHT_EN_ADDR 40  // разрешить взять вес из таблицы при изменении дня
#define NOTIFY_EN_ADDR 44                 // разрешить отправку уведомления пустой бункер

#define FEEDING_TIME_1_ADDR 48             // время кормления 1
#define FEEDING_TIME_2_ADDR 52             // время кормления 2
#define FEEDING_TIME_3_ADDR 56             // время кормления 3
#define FEEDING_TIME_4_ADDR 60             // время кормления 4
#define FEEDING_TIME_5_ADDR 64             // время кормления 5
#define FEEDING_TIME_6_ADDR 68             // время кормления 6
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SET_CLOK_FOR_PROG 0  // запускать часы при прошивке: 1 да, 0 нет. должен быть 0. <<1 при первой прошивке>>

#define STD_E_MAIL_ADDR "std_email@blynk.com"  // системный адрес почты, для проверки введен ли адрес
// "std_email@blynk.com"  // стандартный эмаил что использовался при регистрации в блинке, если использовать его то письмо уйдет на стандартный адрес
#define E_MAIL_ADDR "std_email@blynk.com"  // почта для уведомления пустой бункер (стандартный адрес Blynk)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void feedingParamUpdate();
void lcdDisplay();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// us blynk
bool reconnect_en = 1;
uint16_t reconnect_count = 1;
uint16_t connected_count = 0;
uint16_t global_reconnect_count = 1;
uint16_t test_loop_counter = 1;
uint32_t reconnect_start_timer = 0;
// us blynk
bool set_ds3231_inet_time_en = 1;
bool flag_blynk_connected = 0;
// us blynk

uint8_t general_control_day = GENERAL_CONTROL_DAY;  // упровляющий день контроллера

// переменные часов
uint8_t ds_second, ds_minute = 61, ds_hour, ds_dayOfWeek, ds_day, ds_month, ds_year;  // переменные часов
uint8_t old_ds_day;  // переменная для отслеживания переключения дня

// переменные весов
int32_t val_weight = 0;  // текущий вес на всеах + остаточный вес в фидере
int32_t val_weight_no_remaining = 0;  // текущий вес на всеах без учета остаточного веса
int32_t val_weight_signed = 0;  // текущий вес на всеах без учета остаточного веса со знаком

// переменные фидера
int32_t remaining_bunker_weight = 0;  // остаточный вес при ошибке пустой бункер
int32_t cloud_feed_weight = 0;  // вес из облака
int32_t old_cloud_feed_weight = 0;  // предыдущий вес из облака
uint32_t feeding_stepper_timer = 0;  // старт таймера кормления
uint32_t fed_for_today = 0;  // скормлено за сегодня
uint8_t feeding_state = 0;  // состояние кормления: 1-кормление подготовка. 2-подготовка пройдена. 0-ожидание
uint8_t feeder_responce = 2;  // 1-weight limit, 0-time limit
uint8_t feed_bunker_condition = 2;  // 0-пустой бункер. 1-полный бункер
uint32_t estimated_weight_per_day = 0;  // расчетный вес за день
uint8_t feeding_left = 0;  // осталось кормлений по времени

uint32_t just_a_day = 17500;  // всего за день
uint8_t number_of_feedings = 6;   // количество кормлеений
uint16_t feeding_portion = just_a_day / number_of_feedings;  // 1 порция кормления

String setting_responce = "";
uint32_t setting_responce_int = 1;

// flags
bool scale_restart = 1;
bool notify_en = 1;
bool to_table_cloud_weight_en = 0;
bool flag_feeding_time_en = 1;
bool flag_tare = 0;
bool blynk_weight_display_en = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Servo servo;  // create servo object to control a servo
Utimer timer1(10000);   // t1 init 10s
HX711 scale(HX711_DOUT_PIN, HX711_PD_SCK_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // установка адреса 0x27 и разрешения 16/2 дисплея
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
void eeSetup(){  // write and reed EEPROM setup settings...
	// write
	uint8_t _temp_ee_key;
	EEPROM.get(EEPROM_SETUP_KEY_ADDR, _temp_ee_key);
	if(_temp_ee_key != EEPROM_SETUP_KEY){
		// записать начальные значения в EEPROM
		EEPROM.put(EEPROM_SETUP_KEY_ADDR, EEPROM_SETUP_KEY);

		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
		EEPROM.put(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		EEPROM.put(CALIBRATION_FACTOR_ADDR, calibration_factor);
		EEPROM.put(CALIBRATION_WEIGHT_ADDR, calibration_Weight);
		EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		EEPROM.put(NOTIFY_EN_ADDR, notify_en);

		EEPROM.put(FEEDING_TIME_1_ADDR, feeding_time_1);
		EEPROM.put(FEEDING_TIME_2_ADDR, feeding_time_2);
		EEPROM.put(FEEDING_TIME_3_ADDR, feeding_time_3);
		EEPROM.put(FEEDING_TIME_4_ADDR, feeding_time_4);
		EEPROM.put(FEEDING_TIME_5_ADDR, feeding_time_5);
		EEPROM.put(FEEDING_TIME_6_ADDR, feeding_time_6);
	}

	// dev
	// EEPROM.put(CALIBRATION_FACTOR_ADDR, calibration_factor);
	// EEPROM.put(FEEDING_TIME_1_ADDR, feeding_time_1);
	// EEPROM.put(FEEDING_TIME_2_ADDR, feeding_time_2);
	// EEPROM.put(FEEDING_TIME_3_ADDR, feeding_time_3);
	// EEPROM.put(FEEDING_TIME_4_ADDR, feeding_time_4);
	// EEPROM.put(FEEDING_TIME_5_ADDR, feeding_time_5);
	// EEPROM.put(FEEDING_TIME_6_ADDR, feeding_time_6);

	// reed
	EEPROM.get(GENERAL_CONTROL_DAY_ADDR, general_control_day);
	EEPROM.get(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
	EEPROM.get(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	EEPROM.get(CALIBRATION_FACTOR_ADDR, calibration_factor);
	EEPROM.get(CALIBRATION_WEIGHT_ADDR, calibration_Weight);
	EEPROM.get(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
	EEPROM.get(FED_FOR_TODAY_ADDR, fed_for_today);
	EEPROM.get(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
	EEPROM.get(NOTIFY_EN_ADDR, notify_en);

	if(set_time_in_the_app == 0){
		EEPROM.put(FEEDING_TIME_1_ADDR, feeding_time_1);
		EEPROM.put(FEEDING_TIME_2_ADDR, feeding_time_2);
		EEPROM.put(FEEDING_TIME_3_ADDR, feeding_time_3);
		EEPROM.put(FEEDING_TIME_4_ADDR, feeding_time_4);
		EEPROM.put(FEEDING_TIME_5_ADDR, feeding_time_5);
		EEPROM.put(FEEDING_TIME_6_ADDR, feeding_time_6);
		return;
	}
	EEPROM.get(FEEDING_TIME_1_ADDR, feeding_time_1);
	EEPROM.get(FEEDING_TIME_2_ADDR, feeding_time_2);
	EEPROM.get(FEEDING_TIME_3_ADDR, feeding_time_3);
	EEPROM.get(FEEDING_TIME_4_ADDR, feeding_time_4);
	EEPROM.get(FEEDING_TIME_5_ADDR, feeding_time_5);
	EEPROM.get(FEEDING_TIME_6_ADDR, feeding_time_6);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// КОНТРОЛЬ ДНЕЙ
void changeDayControlSetup(){
	// timeUpdate();
	EEPROM.get(OLD_DAY_ADDR, old_ds_day);
	if(ds_day != old_ds_day){
		old_ds_day = ds_day;
		general_control_day += 1;
		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
		EEPROM.put(OLD_DAY_ADDR, ds_day);

		if(general_control_day > MAX_GENERAL_CONTROL_DAY){
			general_control_day = 0;
			EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
			EEPROM.put(OLD_DAY_ADDR, ds_day);
		}

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		to_table_cloud_weight_en = 1;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		feedingParamUpdate();
	}else if(general_control_day == 0){
		general_control_day += 1;
		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
		old_ds_day = ds_day;
		EEPROM.put(OLD_DAY_ADDR, ds_day);

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		to_table_cloud_weight_en = 1;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		feedingParamUpdate();
	}
}
void changeDayControl(){
	// timeUpdate();  //...
	if(general_control_day == 0){return;}

	if(ds_day != old_ds_day){
		old_ds_day = ds_day;
		general_control_day += 1;
		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
		EEPROM.put(OLD_DAY_ADDR, ds_day);

		if(general_control_day > MAX_GENERAL_CONTROL_DAY){
			general_control_day = 0;
			EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
			EEPROM.put(OLD_DAY_ADDR, ds_day);
		}

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		if(set_time_in_the_app == 0){  // ...
			EEPROM.get(FEEDING_TIME_1_ADDR, feeding_time_1);
			EEPROM.get(FEEDING_TIME_2_ADDR, feeding_time_2);
			EEPROM.get(FEEDING_TIME_3_ADDR, feeding_time_3);
			EEPROM.get(FEEDING_TIME_4_ADDR, feeding_time_4);
			EEPROM.get(FEEDING_TIME_5_ADDR, feeding_time_5);
			EEPROM.get(FEEDING_TIME_6_ADDR, feeding_time_6);
		}

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		to_table_cloud_weight_en = 1;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		feedingParamUpdate();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// часы DS3231 ..
byte decToBcd(byte val){
  	return ( (val/10*16) + (val%10) );
}
////////////////////////////////////////////////////////////
byte bcdToDec(byte val){
  	return ( (val/16*10) + (val%16) );
}
////////////////////////////////////////////////////////////
// day of week function
const uint8_t daysArray [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const uint8_t dowArray[] PROGMEM = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
uint8_t dow(uint16_t y, uint8_t m, uint8_t d){
    uint8_t dow;
    y -= m < 3;
    dow = ((y + y/4 - y/100 + y/400 + pgm_read_byte(dowArray+(m-1)) + d) % 7);
    if (dow == 0){
        return 7;
    }
    return dow;
}
// #define DS3231_ADDRESS              (0x68)
////////////////////////////////////////////////////////////
uint8_t conv2d(const char* p){
    uint8_t v = 0;

    if ('0' <= *p && *p <= '9')
    {
        v = *p - '0';
    }

    return 10 * v + *++p - '0';
}
////////////////////////////////////////////////////////////
void setDateDs3231(
	// byte ds_year,          // 0-99
	// byte ds_month,         // 1-12
	// byte ds_day,    // 1-28/29/30/31
	// byte ds_hour,          // 0-23
	// byte ds_minute,        // 0-59
	// byte ds_second         // 0-59

	byte _ds_second,        // 0-59
	byte _ds_minute,        // 0-59
	byte _ds_hour,          // 1-23
	byte _ds_day,    // 1-28/29/30/31
	byte _ds_month,         // 1-12
	byte _ds_year           // 0-99
)
{
	if(_ds_year >= 2000){_ds_year = _ds_year - 2000;}

   	Wire.beginTransmission(0x68);

   	Wire.write(0);
   	Wire.write(decToBcd(_ds_second));    
   	Wire.write(decToBcd(_ds_minute));
   	Wire.write(decToBcd(_ds_hour));     
   	Wire.write(decToBcd(dow(_ds_year, _ds_month, _ds_day)));
   	Wire.write(decToBcd(_ds_day));
   	Wire.write(decToBcd(_ds_month));
   	Wire.write(decToBcd(_ds_year));

   	Wire.endTransmission();
}
////////////////////////////////////////////////////////////
void setDateDs3231(const char* date, const char* time){
    uint16_t _ds_year;
    uint8_t _ds_month;
    uint8_t _ds_day;
    uint8_t _ds_hour;
    uint8_t _ds_minute;
    uint8_t _ds_second;

    _ds_year = conv2d(date + 9);

    switch (date[0])
    {
        case 'J': _ds_month = date[1] == 'a' ? 1 : _ds_month = date[2] == 'n' ? 6 : 7; break;
        case 'F': _ds_month = 2; break;
        case 'A': _ds_month = date[2] == 'r' ? 4 : 8; break;
        case 'M': _ds_month = date[2] == 'r' ? 3 : 5; break;
        case 'S': _ds_month = 9; break;
        case 'O': _ds_month = 10; break;
        case 'N': _ds_month = 11; break;
        case 'D': _ds_month = 12; break;
    }

    _ds_day = conv2d(date + 4);
    _ds_hour = conv2d(time);
    _ds_minute = conv2d(time + 3);
    _ds_second = conv2d(time + 6);

    setDateDs3231(_ds_second, _ds_minute, _ds_hour, _ds_day, _ds_month, _ds_year);
}
////////////////////////////////////////////////////////////
void getDateDs3231(
	byte *__ds_second,
    byte *__ds_minute,
    byte *__ds_hour,
    byte *__ds_dayOfWeek,
    byte *__ds_day,
    byte *__ds_month,
    byte *__ds_year
)
{
  	Wire.beginTransmission(0x68);
  	Wire.write(0);
  	Wire.endTransmission();

  	Wire.requestFrom(0x68, 7);

  	*__ds_second     = bcdToDec(Wire.read() & 0x7f);
  	*__ds_minute     = bcdToDec(Wire.read());
  	*__ds_hour       = bcdToDec(Wire.read() & 0x3f); 
  	*__ds_dayOfWeek  = bcdToDec(Wire.read());
  	*__ds_day = bcdToDec(Wire.read());
  	*__ds_month      = bcdToDec(Wire.read());
  	*__ds_year       = bcdToDec(Wire.read());
  	// if(*__ds_hour == 0){*__ds_hour = 24;}
}
////////////////////////////////////////////////////////////
void timeUpdate(){
	getDateDs3231(&ds_second, &ds_minute, &ds_hour, &ds_dayOfWeek, &ds_day, &ds_month, &ds_year);  // запрос текущего времени
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ledState(){
	if(feed_bunker_condition == 1){
		analogWrite(GREEN_LED_PIN, GREEN_LED_BRIGHTNES);
		// digitalWrite(GREEN_LED_PIN, HIGH);
		digitalWrite(RED_LED_PIN, LOW);
	}
	else{
		analogWrite(RED_LED_PIN, RED_LED_BRIGHTNES);
		// digitalWrite(RED_LED_PIN, HIGH);
		digitalWrite(GREEN_LED_PIN, LOW);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void feedingParamUpdate(){

	timeUpdate();

	// general_control_day = 
	// EEPROM.get(GENERAL_CONTROL_DAY_ADDR, general_control_day);

	just_a_day = basic_feeding_table[general_control_day];  // всего за день из таблицы

	number_of_feedings = bool(feeding_time_1) 
	+ bool(feeding_time_2)
	+ bool(feeding_time_3)
	+ bool(feeding_time_4)
	+ bool(feeding_time_5)
	+ bool(feeding_time_6);   // количество кормлеений из настроек
	if(number_of_feedings < 1){number_of_feedings = 1;}

	feeding_portion = just_a_day / number_of_feedings;  // 1 порция кормления из таблицы

	estimated_weight_per_day = 0;

	uint8_t _estimated_counter = number_of_feedings;
	if(((feeding_time_1) && (feeding_time_1 < ds_hour)) || ((feeding_time_1) && (feeding_time_1 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	if(((feeding_time_2) && (feeding_time_2 < ds_hour)) || ((feeding_time_2) && (feeding_time_2 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	if(((feeding_time_3) && (feeding_time_3 < ds_hour)) || ((feeding_time_3) && (feeding_time_3 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	if(((feeding_time_4) && (feeding_time_4 < ds_hour)) || ((feeding_time_4) && (feeding_time_4 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	if(((feeding_time_5) && (feeding_time_5 < ds_hour)) || ((feeding_time_5) && (feeding_time_5 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	if(((feeding_time_6) && (feeding_time_6 < ds_hour)) || ((feeding_time_6) && (feeding_time_6 == ds_hour) && (ds_minute >= FEED_UP_TO))){_estimated_counter --;}
	estimated_weight_per_day = fed_for_today + (cloud_feed_weight * _estimated_counter);

	feeding_left = _estimated_counter;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t getWeight(){
	scale.power_up();  // включить весы	
	delay(10);
	int32_t _val_weight;
	_val_weight = scale.get_units(MEASURE_QUANTITY);
	scale.power_down();  // выключить весы
	return _val_weight;
}
////////////////////////////////////////////////////////////
int32_t weightUpdate(){
	val_weight = getWeight();
	Serial.print("\nweight_1 = " + String(val_weight) + "\n");
	val_weight_signed = val_weight;
	val_weight_no_remaining = val_weight;
	if(scale_restart == 1){val_weight = val_weight + remaining_bunker_weight;}
	if(val_weight < 0){val_weight = 0;}
	Serial.print("weight_2 = " + String(val_weight) + "\n");
	Serial.print("calibration_factor = " + String(calibration_factor,5) + "\n");
	
	return val_weight;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cnc driwer
unsigned long step_low_delay = 
((1000000/((1/((360 / STEPS_PER_RONUD)/STEP_DIVIDER))*speed_deg_per_sec))-step_high_delay);  // микросекунд между шагами

uint8_t curent_step_val = 1;

void stepperRun(int16_t _steps = STEPS_WITHOUT_WEIGHT, bool _dir = forward_dir){

	#if DRIVER_SET == 0
	digitalWrite(STEPPER_DIR_PIN, _dir);
	for(int i=0; i<_steps; i++){
	    digitalWrite(STEPPER_STEP_PIN, HIGH);
	    delayMicroseconds(step_high_delay);
	    digitalWrite(STEPPER_STEP_PIN, LOW);
	    delayMicroseconds(step_low_delay);
	}

	#elif DRIVER_SET == 1 || DRIVER_SET == 11
	for(int i=0; i<_steps; i++){
		switch (curent_step_val) {
		    case 1:
		    digitalWrite(C1_A_PIN, HIGH);
		    digitalWrite(C2_A_PIN, LOW);
		    digitalWrite(C1_B_PIN, LOW);
		    digitalWrite(C2_B_PIN, LOW);

		    digitalWrite(C1_EN_PIN, HIGH);
			digitalWrite(C2_EN_PIN, LOW);
		    break;
		    case 2:
		    digitalWrite(C1_A_PIN, LOW);
		    digitalWrite(C2_A_PIN, HIGH);
		    digitalWrite(C1_B_PIN, LOW);
		    digitalWrite(C2_B_PIN, LOW);

		    digitalWrite(C1_EN_PIN, LOW);
			digitalWrite(C2_EN_PIN, HIGH);
		    break;
		    case 3:
		    digitalWrite(C1_A_PIN, LOW);
		    digitalWrite(C2_A_PIN, LOW);
		    digitalWrite(C1_B_PIN, HIGH);
		    digitalWrite(C2_B_PIN, LOW);

		    digitalWrite(C1_EN_PIN, HIGH);
			digitalWrite(C2_EN_PIN, LOW);
		    break;
		    case 4:
		    digitalWrite(C1_A_PIN, LOW);
		    digitalWrite(C2_A_PIN, LOW);
		    digitalWrite(C1_B_PIN, LOW);
		    digitalWrite(C2_B_PIN, HIGH);

		    digitalWrite(C1_EN_PIN, LOW);
			digitalWrite(C2_EN_PIN, HIGH);
		    break;
		    // default:
		    // break;
		}
		delayMicroseconds(step_high_delay);
		#if DRIVER_SET == 11
		digitalWrite(C1_EN_PIN, LOW);
		digitalWrite(C2_EN_PIN, LOW);
		#endif
		delayMicroseconds(step_low_delay);
		if(_dir == 1){
			curent_step_val ++;
			if(curent_step_val > 4){curent_step_val = 1;}
		}
		else{
			curent_step_val --;
			if(curent_step_val < 1){curent_step_val = 4;}
		}
	}
	digitalWrite(C1_EN_PIN, LOW);
	digitalWrite(C2_EN_PIN, LOW);

	digitalWrite(C1_A_PIN, LOW);
	digitalWrite(C2_A_PIN, LOW);
	digitalWrite(C1_B_PIN, LOW);
	digitalWrite(C2_B_PIN, LOW);

	#elif DRIVER_SET == 2 || DRIVER_SET == 22
	digitalWrite(C1_EN_PIN, HIGH);
	digitalWrite(C2_EN_PIN, HIGH);
	for(int i=0; i<_steps; i++){
		switch (curent_step_val) {
		    case 1:
		    digitalWrite(C1_A_PIN, HIGH);
		    digitalWrite(C2_A_PIN, HIGH);
		    digitalWrite(C1_B_PIN, LOW);
		    digitalWrite(C2_B_PIN, LOW);
		    break;
		    case 2:
		    digitalWrite(C1_A_PIN, LOW);
		    digitalWrite(C2_A_PIN, HIGH);
		    digitalWrite(C1_B_PIN, HIGH);
		    digitalWrite(C2_B_PIN, LOW);
		    break;
		    case 3:
		    digitalWrite(C1_A_PIN, LOW);
		    digitalWrite(C2_A_PIN, LOW);
		    digitalWrite(C1_B_PIN, HIGH);
		    digitalWrite(C2_B_PIN, HIGH);
		    break;
		    case 4:
		    digitalWrite(C1_A_PIN, HIGH);
		    digitalWrite(C2_A_PIN, LOW);
		    digitalWrite(C1_B_PIN, LOW);
		    digitalWrite(C2_B_PIN, HIGH);
		    break;
		    // default:
		    // break;
		}
		delayMicroseconds(step_high_delay);
		#if DRIVER_SET == 22
		digitalWrite(C1_EN_PIN, LOW);
		digitalWrite(C2_EN_PIN, LOW);
		#endif
		delayMicroseconds(step_low_delay);
		if(_dir == 1){
			curent_step_val ++;
			if(curent_step_val > 4){curent_step_val = 1;}
		}
		else{
			curent_step_val --;
			if(curent_step_val < 1){curent_step_val = 4;}
		}
	}
	digitalWrite(C1_EN_PIN, LOW);
	digitalWrite(C2_EN_PIN, LOW);

	digitalWrite(C1_A_PIN, LOW);
	digitalWrite(C2_A_PIN, LOW);
	digitalWrite(C1_B_PIN, LOW);
	digitalWrite(C2_B_PIN, LOW);
	#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void runSpreader(){
	delay(AC_DELAY_TIME);
	digitalWrite(RELE_SPREADER_PIN, RELE_HIGH);
	digitalWrite(RELE_VIBRATOR_PIN, RELE_HIGH);

	delay(SERVO_DELAY_TIME);
	servo.write(servo_open_angle);
	 
	delay(SPREADER_RUNING_TIME);
	digitalWrite(RELE_SPREADER_PIN, RELE_LOW);
	digitalWrite(RELE_VIBRATOR_PIN, RELE_LOW);
	servo.write(servo_close_angle);

	// вес набран, все нормально, отправить вес в облако
	fed_for_today += remaining_bunker_weight;
	EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

	// сбросить остаточный вес
	remaining_bunker_weight = 0;
  	EEPROM.put(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
	    
	if(flag_tare == 1){
	    delay(2000);
		scale.power_up();      // включить весы
		scale.tare();          // тара
		scale.power_down();    // выключить весы
		flag_tare = 0;
	}
	scale_restart = 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void notifyFunc(){
	timeUpdate();
	String _ds_time_string = "";
	if(ds_minute < 10){_ds_time_string = " \n__time:__" + String(ds_hour) + ":0" + String(ds_minute) + "__" + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "__\n";}
	else{_ds_time_string = " \n__time:__" + String(ds_hour) + ":" + String(ds_minute) + "__" + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "__\n";}

	String _head = "Кормушка номер [ " + String(FEEDER_INDEX_NUMBER) + " ]";
	String _body = "Кормушка номер [ " + String(FEEDER_INDEX_NUMBER) + " ] \n" + String(_ds_time_string) + " \n__ПУСТОЙ БУНКЕР__";


	// delay(1000);

	Serial.print("\nnotify send\n");
	// отпрпавить уведомление
	Blynk.notify("Кормушка номер [ " + String(FEEDER_INDEX_NUMBER) + " ] \n" + String(_ds_time_string) + " \n__ПУСТОЙ БУНКЕР__");

	Serial.print("\nemail send\n");
	// отправить e-mail
	// Blynk.email(E_MAIL_ADDR, _head, _body);

	if(E_MAIL_ADDR == STD_E_MAIL_ADDR){
		Blynk.email(_head, _body);
	}
	else{
		Blynk.email(E_MAIL_ADDR, _head, _body);
	}

	// delay(1000);

	Serial.print("\nend notify and email send\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool feedingProcessing(){
	if(feed_bunker_condition == 0){
		Serial.print("\npustoy bunker. feeding disable\n");

		// Serial.print("\n\nrun spreader\n");
		// runSpreader();

		return 0;
	}

	feedingParamUpdate();  //обновить параметры кормления


	bool _in_feeder_responce;  // 1-weight limit, 0-time limit
	uint16_t _this_feeding_portion;

	Serial.print(F("\nSTART FEEDING\n"));

	if(cloud_feed_weight != feeding_portion){
		_this_feeding_portion = cloud_feed_weight;
		Serial.print(F("\ncloud feed weight  "));
		Serial.print(_this_feeding_portion);
		Serial.print(F("\n"));
	}
	else{
		_this_feeding_portion = feeding_portion;
		Serial.print(F("\ntable feed weight  "));
		Serial.print(_this_feeding_portion);
		Serial.print(F("\n"));
	}
	
	weightUpdate();

	int32_t _previous_val_weight = val_weight;

	lcd.clear(); // очистить дисплей
	if(cloud_feed_weight != feeding_portion){lcd.print(F("cloud w "));}
	else{lcd.print(F("table w "));}
  	lcd.print(_this_feeding_portion);
  	lcd.setCursor(0, 1);
  	lcd.print(F("curent w "));
  	lcd.setCursor(9, 1);
  	lcd.print(val_weight);

  	uint8_t _first_steps = FIRST_STEPS_DEF;

	feeding_stepper_timer = millis();

	while(1){
		stepperRun(STEPS_WITHOUT_WEIGHT, forward_dir);
		weightUpdate();

		lcd.setCursor(9, 1);
		lcd.print(F("      "));
		lcd.setCursor(9, 1);
  		lcd.print(val_weight);

		
		if(val_weight >= _this_feeding_portion){
		    delay(500);
		    weightUpdate();
		    lcd.setCursor(9, 1);
			lcd.print(F("      "));
			lcd.setCursor(9, 1);
  			lcd.print(val_weight);

		    if(val_weight >= _this_feeding_portion){
			    // вес набран
			    _in_feeder_responce = 1;  // вес набран
			    Serial.print("\nWEIGHT GAINED\n");
			    lcd.clear();
			    lcd.print(F("WEIGHT GAINED"));
			    lcd.setCursor(0, 1);
  				lcd.print(val_weight);
			    lcd.print(F(" feeding..."));
  				
  				remaining_bunker_weight = val_weight;
  				EEPROM.put(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
  				scale_restart = 0;

			    break;
		    }
		}
		else if(millis() - feeding_stepper_timer >= STEPPER_ROTATION_TIME){
			// время выщло // пустой питающий бункер
			_in_feeder_responce = 0;  // время вышло
			Serial.print("\nTIME IS OWER\n");
			lcd.clear();
			lcd.print(F("TIME IS OWER"));
			lcd.setCursor(0, 1);
			lcd.print(F(" EMPTY FEED !!!"));

			feed_bunker_condition = 0;
			EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);

			remaining_bunker_weight = val_weight;
  			EEPROM.put(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
  			scale_restart = 0;

  			// отправить уведомление в облако
  			notify_en = 1;  // установить флаг отправки
			EEPROM.put(NOTIFY_EN_ADDR, notify_en);

			ledState();
			delay(3000);

			break;
		}
  		else if(val_weight < _previous_val_weight + WEIGHT_DIFFERENCE){
  			if(_first_steps > 0){_first_steps --;}
  			else{stepperRun(STEPS_WITHOUT_WEIGHT, !forward_dir);}
  			
  		}
	}

	if(_in_feeder_responce > 0){  // если вес набран RUN SPREADER

		runSpreader();
	}

	Serial.print("\nFEEDING END\n");
	lcd.clear(); // очистить дисплей
	lcdDisplay();

	return _in_feeder_responce;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void lcdDisplay(){
	if(feed_bunker_condition == 0){
		lcd.setCursor(0, 0);
		lcd.print(F(" EMPTY FEED !!!"));
  		lcd.setCursor(0, 1);
  		lcd.print(F("last weight "));
  		lcd.print(remaining_bunker_weight);
		return;
	}

	lcd.setCursor(0, 0);
	lcd.print(F("Fdr_n "));
  	lcd.print(FEEDER_INDEX_NUMBER);
  	lcd.setCursor(9, 0);
  	lcd.print(F("dAY "));
  	lcd.print(general_control_day);
  	lcd.setCursor(0, 1);
  	lcd.print(F("Total feed "));
  	lcd.setCursor(11, 1);
  	lcd.print(F("     "));
  	lcd.setCursor(11, 1);
  	lcd.print(fed_for_today);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool feedTimeDetector(){
	Blynk.setProperty(V32, "offLabel", "online ?");
	blynk_weight_display_en = 1;
	if(to_table_cloud_weight_en == 1){
		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		to_table_cloud_weight_en = 0;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
	}
	else if(old_cloud_feed_weight != cloud_feed_weight){
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		// feedingParamUpdate();
	}
	
	timeUpdate();
	changeDayControl();
	lcdDisplay();
	String _ds_time_string = "";
	if(ds_minute < 10){_ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	else{_ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	Serial.print(_ds_time_string);

	if(ds_minute > FEED_UP_TO && flag_feeding_time_en == 0){
	    flag_feeding_time_en = 1;
	}
	if(flag_feeding_time_en == 0){
		return 0;
	}
	if(
	   ((ds_hour == feeding_time_1) && (feeding_time_1 != 0) && (ds_minute < FEED_UP_TO)) 
	|| ((ds_hour == feeding_time_2) && (feeding_time_2 != 0) && (ds_minute < FEED_UP_TO)) 
	|| ((ds_hour == feeding_time_3) && (feeding_time_3 != 0) && (ds_minute < FEED_UP_TO)) 
	|| ((ds_hour == feeding_time_4) && (feeding_time_4 != 0) && (ds_minute < FEED_UP_TO)) 
	|| ((ds_hour == feeding_time_5) && (feeding_time_5 != 0) && (ds_minute < FEED_UP_TO)) 
	|| ((ds_hour == feeding_time_6) && (feeding_time_6 != 0) && (ds_minute < FEED_UP_TO))
	|| ((ds_hour == 0) && (
							   feeding_time_1 == 24 
							|| feeding_time_2 == 24 
							|| feeding_time_3 == 24 
							|| feeding_time_4 == 24 
							|| feeding_time_5 == 24 
							|| feeding_time_6 == 24 
							) && (ds_minute < FEED_UP_TO))
	){
		flag_feeding_time_en = 0;
		return 1;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// button
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autoCalibrationScale(uint32_t _calibration_weight = calibration_Weight){
	lcd.clear();
	lcd.print("  CALIBRATION");
	lcd.setCursor(0, 1);
	lcd.print("    SCALE ?");
	delay(BUTTON_PRESS_DELAY);
	if((!digitalRead(FEED_BUTTON_PIN)) && (!digitalRead(RESUME_BUTTON_PIN))){  // если нажаты кнопки покормить и возобновить
	}
	else{
		lcd.clear();
		lcd.print("     ERROR");
		lcd.setCursor(0, 1);
		lcd.print("  SCALE NO SET");
		delay(2000);
		return;
	}

	// digitalWrite(GREEN_LED_PIN, HIGH);
	// digitalWrite(RED_LED_PIN, HIGH);
	analogWrite(GREEN_LED_PIN, GREEN_LED_BRIGHTNES);
	analogWrite(RED_LED_PIN, RED_LED_BRIGHTNES);

	scale.power_up();

	scale.set_scale();
	scale.tare();

	scale.power_down();

	lcd.clear();
	lcd.print("put ");
	lcd.print(calibration_Weight);
	lcd.print(" g");
	lcd.setCursor(0, 1);
	lcd.print("and press");
	// lcd.print("and press FEED");

	while((!digitalRead(FEED_BUTTON_PIN)) || (!digitalRead(RESUME_BUTTON_PIN))){
		delay(1000);
	}

	lcd.print(" FEED");
	delay(1000);

	while(1){
		if(!digitalRead(FEED_BUTTON_PIN)){
			float _temporaryScale;

			scale.power_up();

			_temporaryScale = scale.get_units(MEASURE_QUANTITY);
			calibration_factor = _temporaryScale/calibration_Weight;
			scale.set_scale(calibration_factor);

			scale.power_down();

			EEPROM.put(CALIBRATION_FACTOR_ADDR, calibration_factor);

			delay(500);
			digitalWrite(RED_LED_PIN, LOW);
			digitalWrite(GREEN_LED_PIN, LOW);

			weightUpdate();

			lcd.clear();
			lcd.print(" curent weight");
			lcd.setCursor(0, 1);
			lcd.print("     ");
			lcd.print(val_weight);

			delay(5000);

			break;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void resetDayTo1(uint8_t _day_val = 1){
	lcd.clear();
	lcd.print("  RESET DAY ?");
	delay(BUTTON_PRESS_DELAY);
	if(!digitalRead(RESUME_BUTTON_PIN)){  // если нажата физическая кнопка возобновить
		general_control_day = _day_val;
		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
		lcd.clear();
		lcd.print("       OK");
		lcd.setCursor(0, 1);
		lcd.print(" RESET DAY TO 1");
		delay(2000);
	}
	else{
		lcd.clear();
		lcd.print("     ERROR");
		lcd.setCursor(0, 1);
		lcd.print("  DAY NO RESET");
		delay(2000);
		return;
	}

	fed_for_today = 0;
	EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

	feedingParamUpdate();
	cloud_feed_weight = feeding_portion;
	old_cloud_feed_weight = cloud_feed_weight;
	EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	to_table_cloud_weight_en = 1;
	EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
	feedingParamUpdate();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void manualSetDay(){
	lcd.clear();
	lcd.print("   SET DAY ?");
	delay(BUTTON_PRESS_DELAY);
	if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
	}
	else{
		lcd.clear();
		lcd.print("     ERROR");
		lcd.setCursor(0, 1);
		lcd.print("  DAY NO SET");
		delay(2000);
		return;
	}

	lcd.clear();
	lcd.print("       OK");

	while(!digitalRead(FEED_BUTTON_PIN)){
		delay(1000);
	}

	lcd.clear();
	lcd.print("    SET DAY");
	lcd.setCursor(0, 1);
	lcd.print("   R-  ");
	lcd.print(general_control_day);
	lcd.print("  +F  ");
	delay(1000);
	uint32_t _start_time_msd = millis();
	while(1){
	    if(!digitalRead(RESUME_BUTTON_PIN)){  // -
	    	general_control_day --;
	    	if(general_control_day < 1){general_control_day = MAX_GENERAL_CONTROL_DAY;}

	    	lcd.setCursor(0, 1);
			lcd.print("   R-  ");
			lcd.print(general_control_day);
			lcd.print("  +F  ");

	    	delay(200);
	    	_start_time_msd = millis();
	    }
	    else if(!digitalRead(FEED_BUTTON_PIN)){  // +
	    	general_control_day ++;
	    	if(general_control_day > MAX_GENERAL_CONTROL_DAY){general_control_day = 1;}

	    	lcd.setCursor(0, 1);
			lcd.print("   R-  ");
			lcd.print(general_control_day);
			lcd.print("  +F  ");

	    	delay(200);
	    	_start_time_msd = millis();
	    }
	    else if(millis() - _start_time_msd >= 10000){  // delay 10s to auto save
	    	EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
	    	lcd.clear();
	    	lcd.print("     SAVED!");
	    	delay(2000);
  			break;
	    }
	}

	fed_for_today = 0;
	EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

	feedingParamUpdate();
	cloud_feed_weight = feeding_portion;
	old_cloud_feed_weight = cloud_feed_weight;
	EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	to_table_cloud_weight_en = 1;
	EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
	feedingParamUpdate();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkButtonForSetup(){  // обработка кнопок при старте, настроить день контроллера, калибровка весов
	if((!digitalRead(FEED_BUTTON_PIN)) && (!digitalRead(RESUME_BUTTON_PIN))){  // если нажаты кнопки покормить и возобновить
		// автокалибровка весов
		delay(100);  // задержка пол секунды
		if((!digitalRead(FEED_BUTTON_PIN)) && (!digitalRead(RESUME_BUTTON_PIN))){
			autoCalibrationScale();
		}
	}
	else if(!digitalRead(RESUME_BUTTON_PIN)){  // если нажата физическая кнопка возобновить
		// reset day
		delay(100);  // задержка пол секунды
		if(!digitalRead(RESUME_BUTTON_PIN)){
			resetDayTo1(1);
		}
	}
	else if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
		// настроить день контроллера
		delay(100);  // задержка пол секунды
		if(!digitalRead(FEED_BUTTON_PIN)){
			manualSetDay();
		}
	}
	else{
		return;
	}
	lcd.clear();
	lcd.print("  PRESS RESET");
	lcd.setCursor(0, 1);
	lcd.print("  TO CONTINUE");
	while(1){}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkButtonForLoop(){  // обработка кнопок в цикле, покормить и сбросить пустой бункер

	if((!digitalRead(FEED_BUTTON_PIN)) && (feed_bunker_condition == 1)){  // если нажата физическая кнопка покормить
		lcd.clear(); // очистить дисплей
  		lcd.print(F("FEEDING ?"));
		delay(BUTTON_PRESS_DELAY);  // задержка пол секунды
		if(!digitalRead(FEED_BUTTON_PIN)){  // если до сих пор нажата физическая кнопка покормить
			lcd.clear(); // очистить дисплей
  			lcd.print(F("       OK"));
  			delay(1000);
	    	feedingProcessing();
		}
		lcd.clear(); // очистить дисплей
		lcdDisplay();
	}
	else if(!digitalRead(FEED_BUTTON_PIN)){
		lcd.clear(); // очистить дисплей
  		lcd.print(F("first press the"));
  		lcd.setCursor(0, 1);
  		lcd.print(F("RESUME button"));
  		delay(5000);
  		lcd.clear(); // очистить дисплей
		lcdDisplay();
	}

	else if((!digitalRead(RESUME_BUTTON_PIN)) && (feed_bunker_condition == 0)){  // если нажата физическая кнопка возобновить
		lcd.clear(); // очистить дисплей
  		lcd.print(F("RESUME ?"));
		delay(BUTTON_PRESS_DELAY);  // задержка пол секунды
		if(!digitalRead(RESUME_BUTTON_PIN)){  // если до сих пор нажата физическая кнопка возобновить
			lcd.clear(); // очистить дисплей
  			lcd.print(F("       OK"));

			feed_bunker_condition = 1;
			EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);

			ledState();
  			delay(1000);
		}
		lcd.clear(); // очистить дисплей
		lcdDisplay();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generalFeedingSetup(){

	#if DRIVER_SET == 0
	pinMode(STEPPER_STEP_PIN, OUTPUT);
	pinMode(STEPPER_DIR_PIN, OUTPUT);
	digitalWrite(STEPPER_STEP_PIN, LOW);
	digitalWrite(STEPPER_DIR_PIN, forward_dir);
	#else
	pinMode(C1_EN_PIN, OUTPUT);
	pinMode(C1_A_PIN, OUTPUT);
	pinMode(C1_B_PIN, OUTPUT);
	pinMode(C2_EN_PIN, OUTPUT);
	pinMode(C2_A_PIN, OUTPUT);
	pinMode(C2_B_PIN, OUTPUT);
	digitalWrite(C1_EN_PIN, LOW);
	digitalWrite(C1_A_PIN, LOW);
	digitalWrite(C1_B_PIN, LOW);
	digitalWrite(C2_EN_PIN, LOW);
	digitalWrite(C2_A_PIN, LOW);
	digitalWrite(C2_B_PIN, LOW);
	#endif

	pinMode(RELE_SPREADER_PIN, OUTPUT);
	pinMode(RELE_VIBRATOR_PIN, OUTPUT);
	digitalWrite(RELE_SPREADER_PIN, RELE_LOW);
	digitalWrite(RELE_VIBRATOR_PIN, RELE_LOW);

	pinMode(FEED_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RESUME_BUTTON_PIN, INPUT_PULLUP);

	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	digitalWrite(GREEN_LED_PIN, LOW);
	digitalWrite(RED_LED_PIN, LOW);

	eeSetup();

	if(remaining_bunker_weight > 0){
		flag_tare = 1;
	}

	Wire.begin();

	// УСТАНОВКА ЧАСОВ:↓
	#if SET_CLOK_FOR_PROG == 1
 	setDateDs3231(__DATE__, __TIME__);       // Устанавливаем время на часах, основываясь на времени компиляции скетча
 	
	// byte set_second = 30, set_minute = 0, set_hour = 22, set_day = 9, set_month = 7, set_year = 19;
	// setDateDs3231(set_second, set_minute, set_hour, set_day, set_month, set_year);
	#endif

	lcd.begin(); // иниализация дисплея LCD 16/2

  	
  	lcd.clear(); // очистить дисплей
  	lcd.print(F("  FISH FEEDER"));
  	lcd.setCursor(0, 1);
  	lcd.print(F("  V_1  ESP_01"));
  	Serial.print("\nRESTART!\n");

	scale.set_scale(calibration_factor);  //Применяем калибровку
	scale.tare(); 
	delay(50);
	scale.power_down();
	// scale.power_up();

	servo.attach(SERVO_PIN);
	
	servo.write(servo_close_angle);  // закрыть серво

	// feedingParamUpdate();

	timeUpdate();

	String _ds_time_string = "";
	if(ds_minute < 10){_ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	else{_ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	Serial.print(_ds_time_string);
	
	changeDayControlSetup();

	checkButtonForSetup();

	feedingParamUpdate();

	ledState();

  	delay(1000);
  	lcd.clear(); // очистить дисплей
  	lcdDisplay();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generalFeedingLoop(){
	if(timer1.over(61000)){  // каждые 61 сек
		Serial.print("\n\n\nFeed time scan\n");
		if(feedTimeDetector()){
			Serial.print("\n\n\nFeed time DETECTED. Feeding now\n\n\n");
			feeder_responce = feedingProcessing();
		}
	}

	ledState();

	checkButtonForLoop();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// us F
void setClockFromInternetTime(){  // установка часов из интернета...
	if(set_ds3231_inet_time_en == 1){
		set_ds3231_inet_time_en = 0;

		Serial.print("\n\n\n///////////////////////////////");
		Serial.print("\ncheck clock val...\n");

		timeUpdate();

		String ds_time_string = "";
		if(ds_minute < 10){ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year);}
		else{ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year);}
		Serial.print(ds_time_string);

		String currentTime = "";
		if( minute() < 10){currentTime = String(hour()) + ":0" + minute() + "       " + String(day()) + "." + month() + "." + year();}
		else{currentTime = String(hour()) + ":" + minute() + "       " + String(day()) + "." + month() + "." + year();}
		Serial.print("\nblynk time: ");
		Serial.print(currentTime);
		Serial.print("\n");
		
		if(
		1
		/*int(ds_minute) != int(minute()) ||
		int(ds_hour) != int(hour()) ||
		int(ds_day) != int(day()) ||
		int(ds_month) != int(month()) ||
		(int(ds_year) + 2000) != (int(year()) - 48)*/
		)
		{
			Serial.print("\nSET CLOCK...\n");
			setDateDs3231(int(second()), int(minute()), int(hour()), int(day()), int(month()), (int(year()) - 48));

			timeUpdate();

			String ds_time_string = "";
			if(ds_minute < 10){ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year);}
			else{ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year);}
			Serial.print(ds_time_string);

			String currentTime = "";
			if( minute() < 10){currentTime = String(hour()) + ":0" + minute() + "       " + String(day()) + "." + month() + "." + year();}
			else{currentTime = String(hour()) + ":" + minute() + "       " + String(day()) + "." + month() + "." + year();}
			Serial.print("\nblynk time: ");
			Serial.print(currentTime);
			Serial.print("\n");
			Serial.print("///////////////////////////////\n\n\n");
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// V pin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// V10
// V11 возраст дней
// V12
// V13
// V14
// V15
// V16

// V21 светодиод (пустой бункер красный) (ок зеленый) (кормление синий)

// V30 кнопка покормить
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blynk func
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fbLedBunkerConditionAndButtonColor(){  // функция зажигает светодиод в блинке в зависимости от состояния программы
	// feeding_state = 0;
	// feed_bunker_condition = 1;

	if(feed_bunker_condition == 1){
		Blynk.setProperty(V21, "color", "#00FF00");  // установить GREEN цвет светодиода, штатный режим
		Blynk.setProperty(V21, "label", "        работает");  // установить заголовок светодиода

		if(feeding_state == 1){
		Blynk.setProperty(V21, "color", "#FFFF00");  // установить YELOW цвет светодиода, кормление
		}
	}
	else{	
		Blynk.setProperty(V21, "color", "#FF0000");  // установить RED цвет светодиода, пустой питающий бункер
		Blynk.setProperty(V21, "label", "  пустой бункер");  // установить заголовок светодиода
	}
	
	B_LED_bunkerCondition.on();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
// sync
///////////////////////////////////////////////////////////
// как только подключились
BLYNK_CONNECTED(){

	// запросить информацию о состоянии виртуальных пинов Vx
	Blynk.syncVirtual(V30);  // виртуальная кнопка покормить
	Blynk.syncVirtual(V31);  // виртуальная кнопка возобновить
	Blynk.syncVirtual(V32);  // виртуальная кнопка check oline
	Blynk.syncVirtual(V10);  // вес из облака
	Blynk.syncVirtual(V0);   // настройки

	// синхронизация времени при подключении
	B_rtc.begin();

	// отправить уведомление в облако
	if(notify_en == 1){
		if(feed_bunker_condition == 0){
			notifyFunc();
		}
		notify_en = 0;
		EEPROM.put(NOTIFY_EN_ADDR, notify_en);
	}

	if(to_table_cloud_weight_en == 1){
		to_table_cloud_weight_en = 0;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		Blynk.virtualWrite(V10, String(cloud_feed_weight));  // вес 1 кормления +- cloud weight
		feedingParamUpdate();
	}

	// Blynk.virtualWrite(V10, String(cloud_feed_weight));  // вес 1 кормления +- cloud weight
}
///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
// display
///////////////////////////////////////////////////////////
// // вызвать из приложения. if sync mode
// BLYNK_READ(V11){  // блинк запрашивает состояние V11
// 	// отправить в приложение
// 	Blynk.virtualVrite(V11, val)
// }

///////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
// button
///////////////////////////////////////////////////////////
// этот метод будет вызван после ответа сервера BLYNK_CONNECTED

BLYNK_WRITE(V30){  // блинк передает значения с телефона на ардуино (feeding)
	int16_t _v_feeding_button_state;  // состояние виртуальной кнопки покормить
	_v_feeding_button_state = param.asInt();
	if(_v_feeding_button_state == 1){
		_v_feeding_button_state = 0;
		Blynk.virtualWrite(V30, LOW);
		Blynk.setProperty(V21, "color", "#FFFF00");  // установить YELOW цвет светодиода, кормление
		if(feed_bunker_condition == 1){Blynk.setProperty(V21, "label", "       кормление");}
		else{Blynk.setProperty(V21, "label", "        ошибка");}
		B_LED_bunkerCondition.on();
		feeder_responce = feedingProcessing();
	}
}
BLYNK_WRITE(V31){  // блинк передает значения с телефона на ардуино (resume)
	int16_t _v_reset_bunker_button_state;  // состояние виртуальной кнопки сброс
	_v_reset_bunker_button_state = param.asInt();
	if(_v_reset_bunker_button_state == 1){
		_v_reset_bunker_button_state = 0;
		Blynk.virtualWrite(V31, LOW);

		if(feed_bunker_condition == 0){
			feed_bunker_condition = 1;
			EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
			// stepperRun(STEPS_WITHOUT_WEIGHT, !forward_dir);
		}
		// else if(feed_bunker_condition == 1){
		// 	stepperRun(STEPS_WITHOUT_WEIGHT, !forward_dir);
		// }

		fbLedBunkerConditionAndButtonColor();
		ledState();
		lcd.clear(); // очистить дисплей
		lcdDisplay();
	}
}
BLYNK_WRITE(V32){  // блинк передает значения с телефона на ардуино (ping)
	int16_t _v_ping_button_state;  // состояние виртуальной кнопки покормить
	_v_ping_button_state = param.asInt();
		if(_v_ping_button_state == 1){
		_v_ping_button_state = 0;
		Blynk.setProperty(V32, "offLabel", "online");
		Blynk.virtualWrite(V32, LOW);
	}
}
BLYNK_WRITE(V10){  // блинк передает значения с телефона на ардуино, вес из облака
	cloud_feed_weight = param.asInt();
	feedingParamUpdate();
	// Blynk.virtualWrite(V10, String(cloud_feed_weight));  // вес 1 кормления +- cloud weight
}
BLYNK_WRITE(V0){  // блинк передает значения с телефона на ардуино, настройки

	if(en_setting_in_the_app == 0){
		Blynk.virtualWrite(V0, ">set_dis");  // setting field
		return;
	}

	setting_responce = param.asString();
	uint8_t _setting_responce_string_lenght = setting_responce.length();
	setting_responce_int = 0;
	if(setting_responce[3] == 'd' && setting_responce[4] == '='){
		for(int i=5; i<_setting_responce_string_lenght; i++){
	    	setting_responce_int = (setting_responce_int * 10) + (int(setting_responce[i]) - 48);
		}
	}
	else if(setting_responce[3] == 't' && setting_responce[5] == '='){
		for(int i=6; i<_setting_responce_string_lenght; i++){
	    	setting_responce_int = (setting_responce_int * 10) + (int(setting_responce[i]) - 48);
		}
	}

	feedingParamUpdate();
	uint8_t _old_number_of_feedings = number_of_feedings;
	
	Blynk.virtualWrite(V0, "ok>");  // setting field
	if(setting_responce[3] == 'd' && setting_responce[4] == '=' && setting_responce_int <= MAX_GENERAL_CONTROL_DAY){

		general_control_day = setting_responce_int;
		EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
		to_table_cloud_weight_en = 1;
		EEPROM.put(TO_TABLE_CLOUD_WEIGHT_EN_ADDR, to_table_cloud_weight_en);
		feedingParamUpdate();
	}
	else if(setting_responce[4] == '1' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_1 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_1_ADDR, feeding_time_1);}
	}
	else if(setting_responce[4] == '2' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_2 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_2_ADDR, feeding_time_2);}
	}
	else if(setting_responce[4] == '3' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_3 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_3_ADDR, feeding_time_3);}
	}
	else if(setting_responce[4] == '4' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_4 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_4_ADDR, feeding_time_4);}
	}
	else if(setting_responce[4] == '5' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_5 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_5_ADDR, feeding_time_5);}
	}
	else if(setting_responce[4] == '6' && setting_responce[5] == '=' && setting_responce[3] == 't' && setting_responce_int <= 24){
		feeding_time_6 = setting_responce_int;
		if(set_time_in_the_app == 1){EEPROM.put(FEEDING_TIME_6_ADDR, feeding_time_6);}
	}
	else{
	Blynk.virtualWrite(V0, "<error!>");
	delay(3000);
	Blynk.virtualWrite(V0, "ok>");
	}
	feedingParamUpdate();
	if(number_of_feedings != _old_number_of_feedings){
		cloud_feed_weight = feeding_portion;
		feedingParamUpdate();
	}
}
///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timergfedlopEvent()
{	
	generalFeedingLoop();  //...
}

// Digital clock display of the time
void paramDisplay(){

	Serial.print("\nloop  ");
	Serial.print(test_loop_counter);
	Serial.print("\n");
	test_loop_counter ++;

	String currentTime = "";
	if( minute() < 10){currentTime = String(hour()) + ":0" + minute() + "       " + String(day()) + "." + month() + "." + year();}
	else{currentTime = String(hour()) + ":" + minute() + "       " + String(day()) + "." + month() + "." + year();}
	Serial.print("\nblynk time: ");
	Serial.print(currentTime);
	Serial.print("\n");

	ledState();

	if(flag_blynk_connected == 0){return;}
	setClockFromInternetTime();

	
	Blynk.virtualWrite(V17, currentTime);  // время и дата
	Blynk.virtualWrite(V11, String(general_control_day));  // день контроллера
	Blynk.virtualWrite(V14, String(just_a_day));  // всего корма за день из таблицы
	Blynk.virtualWrite(V15, String(estimated_weight_per_day));  // расчетная норма корма за день
	Blynk.virtualWrite(V13, String(number_of_feedings) + " / " + String(feeding_left));  // количество кормлений
	Blynk.virtualWrite(V16, String(fed_for_today));  // скормлено за сегодня
	Blynk.virtualWrite(V12, String(feeding_portion));  // вес 1 кормления

	Blynk.virtualWrite(V10, String(cloud_feed_weight));  // вес 1 кормления +- cloud weight

	delay(1000);

	// V0
	// Blynk.virtualWrite(V0, "");  // setting field

	String _feed_times = " [ "+String(feeding_time_1)+" ]    [ "+String(feeding_time_2)+" ]    [ "+String(feeding_time_3)+" ]    [ "+String(feeding_time_4)+" ]    [ "+String(feeding_time_5)+" ]    [ "+String(feeding_time_6)+" ]";
	Blynk.virtualWrite(V3, _feed_times);   // + test

	if(blynk_weight_display_en == 1){
		blynk_weight_display_en = 0;
		weightUpdate();
		Blynk.virtualWrite(V1, String(val_weight));  // вес
	}

	Blynk.virtualWrite(V2, String(remaining_bunker_weight));

	fbLedBunkerConditionAndButtonColor();
}


void setup(){

	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	// Debug console
	Serial.begin(115200);

	generalFeedingSetup();  //...

	delay(10);

	// Set ESP8266 baud rate
	EspSerial.begin(ESP8266_BAUD);
	delay(10);

	Blynk.begin(auth, wifi, ssid, pass);
	// Blynk.config(auth);
	// Blynk.connectWiFi(ssid, pass);
	// Blynk.connect()

	setSyncInterval(30 * 60); // Sync interval in seconds () 30*60(30 minutes)

	timersynctime.setInterval(1100L, paramDisplay);
	timergfedlop.setInterval(1000L, timergfedlopEvent);
}

void loop(){

	// Blynk.run();  // Blynk run

	if(Blynk.connected()){
		digitalWrite(13, HIGH);
		Serial.print("\nBlynk connected\n");

		if(flag_blynk_connected == 0){connected_count ++;}
		reconnect_en = 1;
		reconnect_count = 1;
		flag_blynk_connected = 1;
	}
	else{
		flag_blynk_connected = 0;
		digitalWrite(13, LOW);
		if(reconnect_en == 1){
			digitalWrite(13, HIGH);
			Serial.print("\n\nBlynk reconnect l=" + String(reconnect_count) 
				+ "   g=" + String(global_reconnect_count) + "   connected " + String(connected_count) + "\n\n");
			digitalWrite(13, LOW);

			Blynk.connectWiFi(ssid, pass);
			Blynk.connect();  // Blynk reconnect

			reconnect_en = 0;
			reconnect_count ++;
			global_reconnect_count ++;
			reconnect_start_timer = millis();
		}
		else{
			if(((reconnect_count <= MAX_RECONNECT_ATTEMP) || (MAX_RECONNECT_ATTEMP >= 100))){
				if(millis() - reconnect_start_timer >= RECONNECT_TIME){
					reconnect_start_timer = millis();
					reconnect_en = 1;
				}
			}
		}
	}

	// generalFeedingLoop();  //...
	timergfedlop.run();
	timersynctime.run();

	delay(1000);
}
