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
#include "feeding_control.h" // файл с алгоритмом управления кормлением

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

ESP8266 wifi(&EspSerial);

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
// Blynk objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetLED B_LED_bunkerCondition(V21);  // blynk led widget состояние питающего бункера
WidgetRTC B_rtc;
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

		// Blynk.setProperty(V30, "offBackColor", "#23C48E");  // установить GREEN цвет кнопки, кормление
		// Blynk.setProperty(V30, "onBackColor", "#FFFF00");   // установить YELOW цвет кнопки, кормление

		// Blynk.setProperty(V30, "offLabel", "покормить");  // установить надпись кнопки
		// Blynk.setProperty(V30, "onLabel", "ок...");   // установить надпись кнопки
	}
	else{	
		Blynk.setProperty(V21, "color", "#FF0000");  // установить RED цвет светодиода, пустой питающий бункер
		Blynk.setProperty(V21, "label", "  пустой бункер");  // установить заголовок светодиода

		if(feeding_state == 1){
		Blynk.setProperty(V21, "color", "#FFFF00");  // установить YELOW цвет светодиода, кормление
		}

		// Blynk.setProperty(V30, "offBackColor", "#D3435C");  // установить RED цвет кнопки, кормление
		// Blynk.setProperty(V30, "onBackColor", "#23C48E");   // установить GREN цвет кнопки, кормление

		// Blynk.setProperty(V30, "offLabel", "возобновить");  // установить надпись кнопки
		// Blynk.setProperty(V30, "onLabel", "ок...");   // установить надпись кнопки
	}
	
	B_LED_bunkerCondition.on();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blynk methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////
// sync
///////////////////////////////////////////////////////////
// как только подключились
BLYNK_CONNECTED(){

	// запросить информацию о состоянии виртуальных пинов Vx
	Blynk.syncVirtual(V30);  // виртуальная кнопка покормить
	Blynk.syncVirtual(V31);  // виртуальная кнопка покормить

	// синхронизация времени при подключении
	B_rtc.begin();
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
BLYNK_WRITE(V30){  // блинк передает значения с телефона на ардуино
	v_feeding_button_state = param.asInt();
	if(v_feeding_button_state == 1){
		v_feeding_button_state = 0;
		Blynk.virtualWrite(V30, LOW);
		Blynk.setProperty(V21, "color", "#FFFF00");  // установить YELOW цвет светодиода, кормление
		if(feed_bunker_condition == 1){Blynk.setProperty(V21, "label", "       кормление");}
		B_LED_bunkerCondition.on();
		feeder_responce = feedingProcessing();
		String _resp_string = "ответ " + String(feeder_responce);
		Blynk.virtualWrite(V0, _resp_string);
	}
}
BLYNK_WRITE(V31){  // блинк передает значения с телефона на ардуино
	v_reset_bunker_button_state = param.asInt();
	if(v_reset_bunker_button_state == 1){
		v_reset_bunker_button_state = 0;
		Blynk.virtualWrite(V31, LOW);

		if(feed_bunker_condition == 0){
			feed_bunker_condition = 1;
			// stepperRun(200, !forward_dir);
		}
		else if(feed_bunker_condition == 1){
			feed_bunker_condition = 0;
			// stepperRun(200, forward_dir);
		}
		fbLedBunkerConditionAndButtonColor();
		ledState();
	}
}
///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end methods
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BlynkTimer timersynctime;

BlynkTimer timergfedlop;

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
	// delay(10);
	Blynk.virtualWrite(V11, String(general_control_day));  // день контроллера
	// delay(10);
	Blynk.virtualWrite(V14, String(just_a_day));  // всего корма за день из таблицы
	// delay(10);
	Blynk.virtualWrite(V15, String(just_a_day));  // расчетная норма корма за день
	// delay(10);
	Blynk.virtualWrite(V13, String(number_of_feedings));  // количество кормлений
	// delay(10);
	Blynk.virtualWrite(V16, String(fed_for_today));  // скормлено за сегодня
	// delay(10);
	Blynk.virtualWrite(V12, String(feeding_portion));  // вес 1 кормления
	// delay(10);
	Blynk.virtualWrite(V10, String(feeding_portion));  // вес 1 кормления +-

	weightUpdate();
	Blynk.virtualWrite(V1, String(val_weight));  // вес
	// Blynk.virtualWrite(V0, String(val_weight));  // вес

	// Blynk.virtualWrite(V14, "10");  // всего за день из таблицы

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

	setSyncInterval(TIME_SYNC_MINUTE_VAL * 60); // Sync interval in seconds () 30*60(30 minutes)

	timersynctime.setInterval(1000L, paramDisplay);
	timergfedlop.setInterval(31000L, timergfedlopEvent);
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

	// digitalWrite(13, !digitalRead(13));

	delay(1000);
}
