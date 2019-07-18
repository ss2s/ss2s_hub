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
#define STEPPER_STEP_PIN 34  // шаговый двигатель step pin
#define STEPPER_DIR_PIN 36   // шаговый двигатель dir pin
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
#define GREEN_LED_PIN 43
#define RED_LED_PIN 45

// физические кнопки
#define FEED_BUTTON_PIN 46                // кнопка покормить
#define RESUME_BUTTON_PIN 47              // кнопка возобновления работы после аварийной остановки
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void feedingParamUpdate();
void lcdDisplay();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// перменные блинк
int16_t v_feeding_button_state;  // состояние виртуальной кнопки покормить
int16_t v_reset_bunker_button_state;  // состояние виртуальной кнопки сброс

// переменные часов
uint8_t ds_second, ds_minute = 61, ds_hour, ds_dayOfWeek, ds_day, ds_month, ds_year;  // переменные часов
uint8_t old_ds_day;  // переменная для отслеживания переключения дня
// переменные весов
int32_t val_weight = 0;  // текущий вес на веах
int32_t remaining_bunker_weight = 0;  // остаточный вес при ошибке пустой бункер
int32_t cloud_feed_weight = 22;  // вес из облака
int32_t old_cloud_feed_weight = 22;  // предыдущий вес из облака


uint32_t feeding_stepper_timer = 0;

uint32_t fed_for_today = 0;  // скормлено за сегодня

uint8_t feeding_state = 0;  // состояние кормления: 1-кормление подготовка. 2-подготовка пройдена. 0-ожидание
bool feeder_responce = 1;  // 1-weight limit, 0-time limit
bool feed_bunker_condition = 1;  // 0-пустой бункер. 1-полный бункер
// flags
bool flag_feeding_time_en = 1;

bool flag_feed_1_OK = 0;
bool flag_feed_2_OK = 0;
bool flag_feed_3_OK = 0;
bool flag_feed_4_OK = 0;
bool flag_feed_5_OK = 0;
bool flag_feed_6_OK = 0;
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
	}

	// reed
	EEPROM.get(GENERAL_CONTROL_DAY_ADDR, general_control_day);
	EEPROM.get(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
	EEPROM.get(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	EEPROM.get(CALIBRATION_FACTOR_ADDR, calibration_factor);
	EEPROM.get(CALIBRATION_WEIGHT_ADDR, calibration_Weight);
	EEPROM.get(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
	EEPROM.get(FED_FOR_TODAY_ADDR, fed_for_today);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// КОНТРОЛЬ ДНЕЙ

// сохранить в EEPROM
void saveToMemoryDay(){
	EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
}
void saveToMemoryOldDay(){
	EEPROM.put(OLD_DAY_ADDR, ds_day);
}

void changeDayControlSetup(){
	// timeUpdate();
	EEPROM.get(OLD_DAY_ADDR, old_ds_day);
	if(ds_day != old_ds_day){
		general_control_day += 1;
		saveToMemoryDay();
		old_ds_day = ds_day;
		saveToMemoryOldDay();

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);

	}else if(general_control_day == 0){
		general_control_day += 1;
		saveToMemoryDay();
		old_ds_day = ds_day;
		saveToMemoryOldDay();

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	}
}
void changeDayControl(){
	// timeUpdate();  //...
	if(general_control_day == 0){return;}

	if(ds_day != old_ds_day){
		old_ds_day = ds_day;

		general_control_day += 1;
		saveToMemoryDay();
		saveToMemoryOldDay();

		if(general_control_day > MAX_GENERAL_CONTROL_DAY){
			general_control_day = 0;
			saveToMemoryDay();
			saveToMemoryOldDay();
		}

		fed_for_today = 0;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);

		feedingParamUpdate();
		cloud_feed_weight = feeding_portion;
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setAndSaveDayVal(uint8_t _day_val = 1){
	general_control_day = _day_val;
	EEPROM.put(GENERAL_CONTROL_DAY_ADDR, general_control_day);
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
	
	feeding_portion = just_a_day / number_of_feedings;  // 1 порция кормления из таблицы

	EEPROM.get(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);  // предыдущий вес бункера, при ошибке питающий бункер

	// uint8_t fed_for_today_counter = 0;
	// if((feeding_time_1) && feeding_time_1 <= ds_hour){fed_for_today_counter ++;}
	// if((feeding_time_2) && feeding_time_2 <= ds_hour){fed_for_today_counter ++;}
	// if((feeding_time_3) && feeding_time_3 <= ds_hour){fed_for_today_counter ++;}
	// if((feeding_time_4) && feeding_time_4 <= ds_hour){fed_for_today_counter ++;}
	// if((feeding_time_5) && feeding_time_5 <= ds_hour){fed_for_today_counter ++;}
	// if((feeding_time_6) && feeding_time_6 <= ds_hour){fed_for_today_counter ++;}
	// fed_for_today = feeding_portion * fed_for_today_counter;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float getWeight(){
	scale.power_up();  // включить весы	
	delay(10);
	int32_t _val_weight;
	_val_weight = scale.get_units(MEASURE_QUANTITY);
	scale.power_down();  // выключить весы
	return _val_weight;
}
////////////////////////////////////////////////////////////
float weightUpdate(){
	val_weight = getWeight();
	Serial.print("\nweight " + String(val_weight) + "\n");
	return val_weight;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void greenLed1s2blink(uint8_t _blink_sec = 3){
	for(int i=0; i<_blink_sec; i++){
		digitalWrite(GREEN_LED_PIN, HIGH);
		delay(250);
		digitalWrite(GREEN_LED_PIN, LOW);
		delay(250);
		digitalWrite(GREEN_LED_PIN, HIGH);
		delay(250);
		digitalWrite(GREEN_LED_PIN, LOW);
		delay(250);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void redLed1s2blink(uint8_t _blink_sec = 3){
	for(int i=0; i<_blink_sec; i++){
		digitalWrite(RED_LED_PIN, HIGH);
		delay(250);
		digitalWrite(RED_LED_PIN, LOW);
		delay(250);
		digitalWrite(RED_LED_PIN, HIGH);
		delay(250);
		digitalWrite(RED_LED_PIN, LOW);
		delay(250);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ledState(){
	if(feed_bunker_condition == 1){
		digitalWrite(GREEN_LED_PIN, HIGH);
		digitalWrite(RED_LED_PIN, LOW);
	}
	else{
		digitalWrite(RED_LED_PIN, HIGH);
		digitalWrite(GREEN_LED_PIN, LOW);
	}
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
bool feedingProcessing(){
	if(feed_bunker_condition == 0){
		Serial.print("\npustoy bunker. feeding disable\n");
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

	lcd.clear(); // очистить дисплей
	if(cloud_feed_weight != feeding_portion){lcd.print(F("cloud w "));}
	else{lcd.print(F("table w "));}
  	lcd.print(_this_feeding_portion);
  	lcd.setCursor(0, 1);
  	lcd.print(F("curent w "));
  	lcd.setCursor(9, 1);
  	lcd.print(val_weight);

	feeding_stepper_timer = millis();
	while(1){
		stepperRun(STEPS_WITHOUT_WEIGHT, forward_dir);
		weightUpdate();
		lcd.setCursor(9, 1);
		lcd.print(F("      "));
		lcd.setCursor(9, 1);
  		lcd.print(val_weight);
		
		if(val_weight >= _this_feeding_portion){
		    delay(100);
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
			    break;
		    }
		}
		else if(millis() - feeding_stepper_timer >= stepper_rotation_time){
			// пустой питающий бункер
			_in_feeder_responce = 0;  // время вышло
			Serial.print("\nTIME IS OWER\n");
			lcd.clear();
			lcd.print(F("TIME IS OWER"));
			break;
		}
	}

	if(_in_feeder_responce > 0){  // сли вес набран RUN SPREADER

	    delay(AC_DELAY_TIME);
	    digitalWrite(RELE_SPREADER_PIN, RELE_HIGH);
	    digitalWrite(RELE_VIBRATOR_PIN, RELE_HIGH);

	    delay(SERVO_DELAY_TIME);
	    servo.write(servo_open_angle);
	    
	    delay(SPREADER_RUNING_TIME);
	    digitalWrite(RELE_SPREADER_PIN, RELE_LOW);
	    digitalWrite(RELE_VIBRATOR_PIN, RELE_LOW);
	    servo.write(servo_close_angle);
	    // send val weight to cloud...

		// scale.power_up();      // включить весы
		// scale.tare();          // тара
		// scale.power_down();    // выключить весы
	}

	if(_in_feeder_responce > 0){
		// вес набран, все нормально, отправить вес в облако
		fed_for_today += val_weight;
		EEPROM.put(FED_FOR_TODAY_ADDR, fed_for_today);
	}
	else{
		// пустой питающий бункер
		feed_bunker_condition = 0;
		EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
		remaining_bunker_weight = val_weight;
		EEPROM.put(REMAINING_WEIGHT_ADDR, remaining_bunker_weight);
		// отправить уведомление в облако
		Blynk.notify("ПУСТОЙ БУНКЕР\nкормушка номер " + String(FEEDER_INDEX_NUMBER));
		Blynk.setProperty(V21, "color", "#FF0000");  // установить RED цвет светодиода, пустой питающий бункер
		Blynk.setProperty(V21, "label", "  пустой бункер");  // установить заголовок светодиода
		B_LED_bunkerCondition.on();
		delay(3000);
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
  		lcd.print(F("last weight  "));
  		lcd.print(remaining_bunker_weight);
		return;
	}

	lcd.setCursor(0, 0);
	lcd.print(F("FeedN "));
  	lcd.print(FEEDER_INDEX_NUMBER);
  	lcd.setCursor(9, 0);
  	lcd.print(F("dAY "));
  	lcd.print(general_control_day);
  	lcd.setCursor(0, 1);
  	lcd.print(F("Total feed  "));
  	lcd.print(fed_for_today);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool feedTimeDetector(){
	if(old_cloud_feed_weight != cloud_feed_weight){
		old_cloud_feed_weight = cloud_feed_weight;
		EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	}
	
	timeUpdate();
	changeDayControl();
	lcdDisplay();
	String _ds_time_string = "";
	if(ds_minute < 10){_ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	else{_ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	Serial.print(_ds_time_string);

	if(ds_minute > 30 && flag_feeding_time_en == 0){
	    flag_feeding_time_en = 1;
	}
	if(flag_feeding_time_en == 0){
		return 0;
	}
	if(
	((ds_hour == feeding_time_1) && (ds_minute < 30)) 
	|| ((ds_hour == feeding_time_2) && (ds_minute < 30)) 
	|| ((ds_hour == feeding_time_3) && (ds_minute < 30)) 
	|| ((ds_hour == feeding_time_4) && (ds_minute < 30)) 
	|| ((ds_hour == feeding_time_5) && (ds_minute < 30)) 
	|| ((ds_hour == feeding_time_6) && (ds_minute < 30))
	|| ((ds_hour == 0) && (
							feeding_time_1 == 24 
							|| feeding_time_2 == 24 
							|| feeding_time_3 == 24 
							|| feeding_time_4 == 24 
							|| feeding_time_5 == 24 
							|| feeding_time_6 == 24 
							) && (ds_minute < 30))
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
	digitalWrite(RED_LED_PIN, HIGH);
	digitalWrite(GREEN_LED_PIN, HIGH);
	scale.set_scale();
	scale.tare();
	delay(5000);
	while(1){
		if(!digitalRead(FEED_BUTTON_PIN)){
			float _temporaryScale;
			float _temporaryRatio;
			_temporaryScale = scale.get_units(10);
			_temporaryRatio = _temporaryScale/calibration_Weight;
			scale.set_scale(_temporaryRatio);
			EEPROM.put(CALIBRATION_WEIGHT_ADDR, _temporaryRatio);
			digitalWrite(RED_LED_PIN, LOW);
			digitalWrite(GREEN_LED_PIN, LOW);
			greenLed1s2blink();
		}
	}
}
void checkButtonResetDayForSetup(){  // обработка кнопок при старте, сбросить день
	if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
		delay(HOLD_BUTTON_RESET_DAY_DELAY);
		if(!digitalRead(FEED_BUTTON_PIN)){
	    	setAndSaveDayVal(1);
	    	greenLed1s2blink();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkButtonCalibrationScaleForSetup(){  // обработка кнопок при старте, калибровка весов
	if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
		redLed1s2blink((HOLD_BUTTON_CALIBRATION_SCALE_DELAY - (HOLD_BUTTON_RESET_DAY_DELAY + 3000)) / 1000);
		if(!digitalRead(FEED_BUTTON_PIN)){
	    	digitalWrite(RED_LED_PIN, LOW);
	    	delay(5000);
	    	// запустить авто калибровку весов...
	    	autoCalibrationScale();
		}
		else{
			digitalWrite(GREEN_LED_PIN, HIGH);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkButtonForSetup(){  // обработка кнопок при старте, настроить день контроллера, калибровка весов
	if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
		delay(500);  // задержка пол секунды
		// автокалибровка весов
	}
	if(!digitalRead(RESUME_BUTTON_PIN)){  // если нажата физическая кнопка возобновить
		delay(500);  // задержка пол секунды
	}
	if((!digitalRead(FEED_BUTTON_PIN)) && (!digitalRead(RESUME_BUTTON_PIN))){  // если нажаты кнопки покормить и возобновить
		delay(500);  // задержка пол секунды
		// настроить день контроллера
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkButtonForLoop(){  // обработка кнопок в цикле, покормить и сбросить пустой бункер
	if(!digitalRead(FEED_BUTTON_PIN)){  // если нажата физическая кнопка покормить
		delay(500);  // задержка пол секунды
		if(!digitalRead(FEED_BUTTON_PIN)){  // если до сих пор нажата физическая кнопка покормить
	    	feedingProcessing();
		}
	}
	if(!digitalRead(RESUME_BUTTON_PIN)){  // если нажата физическая кнопка возобновить
		delay(500);  // задержка пол секунды
		if(!digitalRead(RESUME_BUTTON_PIN)){  // если до сих пор нажата физическая кнопка возобновить
			feed_bunker_condition = 1;
			EEPROM.put(FEED_BUNKER_CONDITION_ADDR, feed_bunker_condition);
		}
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

	checkButtonResetDayForSetup();

	Wire.begin();


	// УСТАНОВКА ЧАСОВ:↓
	#if SET_CLOK_FOR_PROG == 1
 	setDateDs3231(__DATE__, __TIME__);       // Устанавливаем время на часах, основываясь на времени компиляции скетча
 	
	// byte set_second = 30;
	// byte set_minute = 0;
	// byte set_hour = 22;
	// byte set_day = 9; // день
	// byte set_month = 7;
	// byte set_year = 19;
	// setDateDs3231(set_second, set_minute, set_hour, set_day, set_month, set_year);
	#endif


	lcd.begin(); // иниализация дисплея LCD 16/2
  	lcd.clear(); // очистить дисплей
  	lcd.print(F("  FISH FEEDER"));
  	lcd.setCursor(0, 1);
  	lcd.print(F("    V_1.0.1"));
  	delay(2000);


	scale.set_scale(calibration_factor);  //Применяем калибровку
	scale.tare(); 
	delay(50);
	scale.power_down();

	servo.attach(SERVO_PIN);
	
	servo.write(servo_close_angle);  // закрыть серво

	checkButtonCalibrationScaleForSetup();

	ledState();

	// feedingParamUpdate();

	timeUpdate();

	eeSetup();
	changeDayControlSetup();

	feedingParamUpdate();

  	lcd.clear(); // очистить дисплей
  	lcdDisplay();

	String _ds_time_string = "";
	if(ds_minute < 10){_ds_time_string = "\nds time: " + String(ds_hour) + ":0" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	else{_ds_time_string = "\nds time: " + String(ds_hour) + ":" + String(ds_minute) + "       " + String(ds_day) + "." + String(ds_month) + ".20" + String(ds_year) + "\n\n\n";}
	Serial.print(_ds_time_string);
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