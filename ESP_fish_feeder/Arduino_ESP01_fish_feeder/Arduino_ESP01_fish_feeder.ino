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

#define BLYNK_NOWHILE_PATCH  // эта строка отключает зависание блинка при отсутствии сети

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

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


// us
bool reconnect_en = 1;
uint16_t reconnect_count = 1;
uint16_t connected_count = 0;
uint16_t global_reconnect_count = 1;
uint16_t test_loop_counter = 1;
uint32_t reconnect_start_timer = 0;
// us
bool set_ds3231_inet_time_en = 1;
bool flag_blynk_connected = 0;
// us


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blynk objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetLED B_LED_bunkerCondition(V21);  // blynk led widget состояние питающего бункера
WidgetRTC B_rtc;
ESP8266 wifi(&EspSerial);
BlynkTimer timersynctime;
BlynkTimer timergfedlop;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "feeding_control.h" // файл с алгоритмом управления кормлением



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

	// синхронизация времени при подключении
	B_rtc.begin();

	// отправить уведомление в облако
	if(notify_en == 1){
		if(feed_bunker_condition == 0){

			// отпрпавить уведомление
			Blynk.notify("ПУСТОЙ БУНКЕР\nКормушка номер " + String(FEEDER_INDEX_NUMBER));

			// отправить e-mail
			// Blynk.email(e_mail_addr, "Кормушка номер " + String(FEEDER_INDEX_NUMBER), "ПУСТОЙ БУНКЕР\nКормушка номер " + String(FEEDER_INDEX_NUMBER));
			Blynk.email("Кормушка номер " + String(FEEDER_INDEX_NUMBER), "ПУСТОЙ БУНКЕР\nКормушка номер " + String(FEEDER_INDEX_NUMBER));
			// e_mail_addr

			// установить индикацию пустой бункер в блинке
			Blynk.setProperty(V21, "color", "#FF0000");  // установить RED цвет светодиода, пустой питающий бункер
			Blynk.setProperty(V21, "label", "  пустой бункер");  // установить заголовок светодиода
			B_LED_bunkerCondition.on();
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
// 	Blynk.virtualVrite(V1, val)
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
	Blynk.virtualWrite(V13, String(number_of_feedings));  // количество кормлений
	Blynk.virtualWrite(V16, String(fed_for_today));  // скормлено за сегодня
	Blynk.virtualWrite(V12, String(feeding_portion));  // вес 1 кормления

	Blynk.virtualWrite(V10, String(cloud_feed_weight));  // вес 1 кормления +- cloud weight

	delay(1000);

	String _resp_string = "ответ " + String(feeder_responce);
	Blynk.virtualWrite(V0, _resp_string);

	// V0
	// Blynk.virtualWrite(V18, "10");  // test 2

	// if(old_cloud_feed_weight != cloud_feed_weight){
	// 	old_cloud_feed_weight = cloud_feed_weight;
	// 	EEPROM.put(CLOUD_FEED_WEIGHT_ADDR, cloud_feed_weight);
	// }

	Blynk.virtualWrite(V2, String(remaining_bunker_weight));  // test

	weightUpdate();
	Blynk.virtualWrite(V1, String(val_weight));  // вес
	// Blynk.virtualWrite(V0, String(val_weight));  // вес


	fbLedBunkerConditionAndButtonColor();

	// Blynk.setProperty(V32, "offLabel", "online ?");
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
