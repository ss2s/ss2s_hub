#include <auth.h>

#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART_BRIDGE_EN 0  // 0-для работы системы в штатном режиме. 1-для организации моста с модемом.  // default 0
// если прошить с 0 то программа будет работать в штатном режиме, обработка смс, таймеров и запуск генератора по команде
// если прошить с 1 то программа будет работать как мост для настройки модема
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА:  здесь настраиваются выводы микроконтроллера
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define RESET_MODEM_PIN 8                  // RESET sim800 optional

#define STARTER_RELE_PIN A1                // РЕЛЕ СТАРТЕРА 30А ИЛИ МОСФЕТ 30А

#define AIR_DAMPER_RELE_PIN A2             // РЕЛЕ ВОЗДУШНОЙ ЗАСЛОНКИ
#define STOP_GENERATOR_RELE_PIN 7          // РЕЛЕ ОСТАНОВКИ ГЕНЕРАТОРА
#define FUEL_GATE_RELE_PIN 9               // РЕЛЕ КРАНИКА ТОПЛИВА

#define VOLTAGE_VCC_SENSOR_PIN 4           // ИНДИКАТОР НАПРЯЖЕНИЯ 220V НА ГЕНЕРАТОРЕ VCC
#define VOLTAGE_OUT_SENSOR_PIN 5           // ИНДИКАТОР НАПРЯЖЕНИЯ 220V НА ГЕНЕРАТОРЕ OUT
#define VOLTAGE_GND_SENSOR_PIN 6           // ИНДИКАТОР НАПРЯЖЕНИЯ 220V НА ГЕНЕРАТОРЕ GND

#define SOFT_RX_PIN 3                      // SoftSerial RX
#define SOFT_TX_PIN 2                      // SoftSerial TX
#define GND_PIN 12                         // GND

#define BATTERY_12V_VOLTAGE_SENSOR_PIN A0  // НАПРЯЖЕНИЯ НА АККУМУЛЯТОРЕ ГЕНЕРАТОРА
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                              ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ ПРОГРАММЫ:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
// VERSION SELECTION. ВЫБОР ВЕРСИИ
#define PRODUCT_VERSION 2  // ВЕРСИЯ ПРОДУКТА: 1-базовая; 2-расширеная;
// COMPOSITION OF VERSIONS. СОСТАВ ВЕРСИЙ
#define VERSION_USER_TIMERS_EN 2
#define VERSION_TOTAL_RUN_TIME_REPORT_EN 2
#define VERSION_OIL_REMINDER_EN 2
#define VERSION_LOAD_REPORT_EN 2

// номера телефонов с которых будет принимать звонок
String whiteListPhones = "+380xxxxxxxxx, +380xxxxxxxxx, +380xxxxxxxxx"; // Белый список телефонов
String wlphones = my_phones;   // Белый список телефонов
String masterPhones = my_phone_number_str;     // мастер телефон

String sms_password = "ttks2";         // пароль для смс настроек 5 символов !

// настройки запуска, изменяемые через СМС
// количество повторных пусков:                                nsl=(x)
// время работы стартера                                       tsl=(x)
// прирост времени работы стартера при повторном пуске         gsl=(x)
// время глушения двигателя:                                   srt=(x)
uint8_t  number_starter_launch = 4;      // количество попыток запуска, при неудачном старте. MAX4  nsl=(x)
uint32_t timeOut_starter_launch = 3;     // столько работает стартер (2-4s)                      tsl=(x)
uint32_t gain_starter_launch = 1;        // прирост времени для последующих попыток запуска         gsl=(x)
uint32_t stop_rele_time = 10;            // столько работает реле глущения генератора (1-10s)    srt=(x)

uint8_t  number_stop_attempt = 5;        // количество попыток остановки, при неудачном старте. MAX255

// задержки
#define timeOut_operation 50           // other operation timeout 0.05s
#define timeOut_operation_2 1000       // other operation timeout 2 = 0.5s // задержка после отключения стартера
#define prev_starter_DEL 500   // задержка перед запуском стартера, после открытия заслонки 0.5s
#define timeOut_airDamper_OFF 2000     // задержка перед закрытием воздушной заслонки 2s
#define timeOut_next_start 10000       // задержка между попытками запуска, при неудачном старте


// НАСТРОЙКИ ФУНКЦИЙ  // эта функция еще не активна
#define SET_STARTER_TIMER_FUNC 0       // ЕСЛИ 0 ТО СТАРТЕР БУДЕТ УЧИТЫВАТЬ ТОЛЬКО ТАЙМАУТ СТАРТЕРА
							           // ЕСЛИ 1 ТО СТАРТЕР БУДЕТ УЧИТЫВАТЬ НАПРЯЖЕНИЕ НА ГЕНЕРАТОРЕ И ТАЙМАУТ СТАРТЕРА
// типы активации реле
// реле бывает 2 типов активации 1 или 0. здесь можно настроить тип активации реле
#define Y_220 0                        // out sensor 220 yes
#define N_220 1                        // out sensor 220 no
#define RELE_HIGH 0                    // 10A rele high
#define RELE_LOW 1                     // 10A rele low
#define STARTER_RELE_HIGH 1            // 30A starter rele high
#define STARTER_RELE_LOW 0             // 30A starter rele low

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// КОНЕЦ ВСЕХ НАСТРОЕК
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                              ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// #include "ttks_v2_config.h"



// GLOBAL VARIABLES
// настройки для разработки. сюда лучше не лезть

SoftwareSerial SoftSerial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX

// определение направления портов UART
// настройка портов GSM serial 0: com=hard, gsm=soft.   1: com=soft, gsm=hard.
#define GSM_SERIAL_MODE 0

#if GSM_SERIAL_MODE == 1
#define Sim800Port Serial
#define ComPort SoftSerial
#else
#define Sim800Port SoftSerial
#define ComPort Serial
#endif

#define SIM800 Sim800Port  // sys

// variables
uint32_t vremya_zapuska_dvigatelya = 0;

uint32_t zapushen_s_popitki = 0;

uint32_t obshaya_narabotka_v_motominutah = 0;
uint32_t obshaya_narabotka_v_motochasah = 0;

String _response = "R";  // Переменная для хранения ответа модуля
String lastPhoneNumber = masterPhones;  // последний номер телефона для ответного смс с показаниями
bool alarmFlag1 = 0;

bool flag_Start = 0;
bool flag_Stop = 0;

bool hasmsg = false;                                              // Флаг наличия сообщений к удалению

long lastUpdate = millis();                                   // Время последнего обновления
long updatePeriod   = 60000;                                  // Проверять каждую минуту

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM F

// eeKey
#define EEPROM_SETUP_KEY 124  // любое число от 1 до 254. измените это число если нужно перепрошить EEPROM
// addr
#define EEPROM_SETUP_KEY_ADDR 0           // ключ перезаписи
#define NUMBER_STARTER_LAUNCH_ADDR 4
#define TIMEOUT_STARTER_LAUNCH_ADDR 8
#define GAIN_STARTER_LAUNCH_ADDR 12
#define STOP_RELE_TIME_ADDR 16
#define OBSHAYA_NARABOTKA_V_MOTOCHASAH_ADDR 20
#define OBSHAYA_NARABOTKA_V_MOTOMINUTAH_ADDR 24

void eeSetup(){  // write and reed EEPROM setup settings...
	// write
	uint8_t _temp_ee_key;
	EEPROM.get(EEPROM_SETUP_KEY_ADDR, _temp_ee_key);
	if(_temp_ee_key != EEPROM_SETUP_KEY){
		// записать начальные значения в EEPROM
		EEPROM.put(EEPROM_SETUP_KEY_ADDR, EEPROM_SETUP_KEY);

		EEPROM.put(NUMBER_STARTER_LAUNCH_ADDR, number_starter_launch);
		EEPROM.put(TIMEOUT_STARTER_LAUNCH_ADDR, timeOut_starter_launch);
		EEPROM.put(GAIN_STARTER_LAUNCH_ADDR, gain_starter_launch);
		EEPROM.put(STOP_RELE_TIME_ADDR, stop_rele_time);
		EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOCHASAH_ADDR, obshaya_narabotka_v_motochasah);
		EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOMINUTAH_ADDR, obshaya_narabotka_v_motominutah);
	}

	// reed
	EEPROM.get(NUMBER_STARTER_LAUNCH_ADDR, number_starter_launch);
	EEPROM.get(TIMEOUT_STARTER_LAUNCH_ADDR, timeOut_starter_launch);
	EEPROM.get(GAIN_STARTER_LAUNCH_ADDR, gain_starter_launch);
	EEPROM.get(STOP_RELE_TIME_ADDR, stop_rele_time);
	EEPROM.get(OBSHAYA_NARABOTKA_V_MOTOCHASAH_ADDR, obshaya_narabotka_v_motochasah);
	EEPROM.get(OBSHAYA_NARABOTKA_V_MOTOMINUTAH_ADDR, obshaya_narabotka_v_motominutah);

	ComPort.print(F("\n nsl = "));
	ComPort.println(number_starter_launch);
	ComPort.print(F(" tsl = "));
	ComPort.println(timeOut_starter_launch);
	ComPort.print(F(" gsl = "));
	ComPort.println(gain_starter_launch);
	ComPort.print(F(" srt = "));
	ComPort.println(stop_rele_time);

	ComPort.print(F(" obshaya_narabotka_v_motochasah = "));
	ComPort.println(obshaya_narabotka_v_motochasah);
	ComPort.print(F(" obshaya_narabotka_v_motominutah = "));
	ComPort.println(obshaya_narabotka_v_motominutah);

	ComPort.println(F("\n"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// GSM F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gsmModemStart(){  // стартовая функция для старта модема

	delay(100);
	// выбор режима ответов модема, текстовый или цифровой
	Sim800Port.println("AT");  // автонастройка скорости
	delay(100);
	Sim800Port.println("ATE1V1+CMEE=2;&W");  // text responce
	// Sim800Port.println("ATE0V0+CMEE=1;&W");  // digital responce
	delay(50);

	ComPort.println("");
	ComPort.print("sys: ");
	ComPort.println("Start system");

	uint32_t blinkSetupTimeStart = millis();
	uint8_t _resp_counter = 0;
	do {
    	_response = sendATCommand("AT", true);        // проверка модема
    	_response.trim();                             // Убираем пробельные символы в начале и конце

    	if(millis()-blinkSetupTimeStart >= 500){
    		digitalWrite(13, !digitalRead(13));
    		blinkSetupTimeStart = millis();
    	}
    	_resp_counter ++;
    	if(_resp_counter == 5){
    		pinMode(RESET_MODEM_PIN, OUTPUT);
    		digitalWrite(RESET_MODEM_PIN, LOW);
    		delay(2000);
    		pinMode(RESET_MODEM_PIN, INPUT);
    		ComPort.println("FIRST HARD RESET MODEM");
    		delay(10000);
    	}
    	else if(_resp_counter >= 30){
    		pinMode(RESET_MODEM_PIN, OUTPUT);
    		digitalWrite(RESET_MODEM_PIN, LOW);
    		delay(2000);
    		pinMode(RESET_MODEM_PIN, INPUT);
    		ComPort.println("HARD RESET MODEM");
    		_resp_counter = 6;
    		delay(10000);
    	}
	} while (_response != "OK");                      // Не пускать дальше, пока модем не вернет ОК
	digitalWrite(13, LOW);
	ComPort.println("sys: GSM modem is ready!");

	// Команды настройки модема при каждом запуске
	_response = sendATCommand("AT+CLIP=1", true);     // Включаем АОН
	_response = sendATCommand("AT+DDET=1;&W", true);  // Включаем DTMF и сразу сохраняем значение (AT&W)!
	// sendATCommand("AT+CLVL?", true);               // Запрашиваем громкость динамика

	do {
    	_response = sendATCommand("AT+CLIP=1", true); // Включаем АОН
    	_response.trim();                             // Убираем пробельные символы в начале и конце
	} while (_response != "OK");                      // Не пускать дальше, пока модем не вернет ОК

	ComPort.println("Opredelenie nomera VKL");        // Информируем, что АОН включен
	// Проверка пройдена, модем сообщил о готовности, можно запускать основной цикл...

	
	sendATCommand("AT+CMGF=1;&W", true); // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
	// sendATCommand("AT+CMGDA=\"DEL ALL\"", true);      // Удалить все сообщения, чтобы не забивали память модуля (txt mode)
	// sendATCommand("AT+CMGDA=6", true);             // Удалить все сообщения, чтобы не забивали память модуля (pdu mode)

	ComPort.println("\nMODEM START OK\nRUN GENERAL LOOP");        //
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void trx_Gsm_Port(){  // сервисная функция test UART F
	if (Sim800Port.available()){ComPort.write(Sim800Port.read());}
	if (ComPort.available()){Sim800Port.write(ComPort.read());}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String _resp = "";                            // Переменная для хранения результата
String sendATCommand(String cmd, bool waiting){   // сервисная функция отправки АТ команды на модем
	_resp = "";                            // Переменная для хранения результата
	ComPort.print("cmd: ");                       // Дублируем команду в монитор порта
	ComPort.println(cmd);                         // Дублируем команду в монитор порта
	SIM800.println(cmd);                          // Отправляем команду модулю
	if (waiting) {                                // Если необходимо дождаться ответа...
		_resp = waitResponse();                   // ... ждем, когда будет передан ответ

			// Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
		if (_resp.startsWith(cmd)) {  // Убираем из ответа дублирующуюся команду
			_resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
		}

		// ComPort.print("sys: ");
		ComPort.println(_resp);                   // Дублируем ответ в монитор порта
	}
	return _resp;                                 // Возвращаем результат. Пусто, если проблема
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long _timeout = 0;
String waitResponse(){  // сервисная функция ожидания ответа и возврата полученного результата
	_resp = "";                            // Переменная для хранения результата
	_timeout = millis() + 15000;             // Переменная для отслеживания таймаута (10 секунд)
	while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 15 секунд, если пришел ответ или наступил таймаут, то...
	if (SIM800.available()) {                     // Если есть, что считывать...
		_resp = SIM800.readString();              // ... считываем и запоминаем
	}
	else {                                        // Если пришел таймаут, то...
		ComPort.println("Timeout...");            // ... оповещаем об этом и...
		// pinMode(RESET_MODEM_PIN, OUTPUT);
		// digitalWrite(RESET_MODEM_PIN, LOW);
		// delay(2000);
		// pinMode(RESET_MODEM_PIN, INPUT);
		// delay(5000);
	}
	return _resp;                                 // ... возвращаем результат. Пусто, если проблема
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sendSMS(String phone, String message){
	sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
	sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String msgheader  = "";
String msgbody    = "";
String msgphone   = "";
void parseSMS(String msg) {                                   // Парсим SMS
	msgheader  = "";
	msgbody    = "";
	msgphone   = "";

	msg = msg.substring(msg.indexOf("+CMGR: "));
	msgheader = msg.substring(0, msg.indexOf("\r"));            // Выдергиваем телефон

	msgbody = msg.substring(msgheader.length() + 2);
	msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK"));  // Выдергиваем текст SMS
	msgbody.trim();

	int firstIndex = msgheader.indexOf("\",\"") + 3;
	int secondIndex = msgheader.indexOf("\",\"", firstIndex);
	msgphone = msgheader.substring(firstIndex, secondIndex);

	ComPort.println("Phone: " + msgphone);                       // Выводим номер телефона
	ComPort.println("Message: " + msgbody);                      // Выводим текст SMS

	if (msgphone.length() > 6 && wlphones.indexOf(msgphone) > -1) { // Если телефон в белом списке, то...
		processSMScommands(msgbody, msgphone);                           // ...выполняем команду
	}
	else {
		ComPort.println("Unknown phonenumber");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// engine F

// ЗАПУСК
// - Повернуть ключ в положение ON (размыкаем контакт на глушение генератора), руками.
// - Посылаем команду на запуск (с телефона)
int16_t startEngineSingleF(uint32_t _gain){  // функция запуска двигателя 1 раз
	// - Проверяем наличие напряжения на генераторе (контроль того, что генератор не запущен)
	ComPort.println(F("SE: 220 sensor read"));
	if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
		ComPort.println(F("__dvigatel_uzhe_zapushen"));
		return -1;  // ДВИГАТЕЛЬ УЖЕ ЗАПУЩЕН
	}
	// - Подаем напряжение +12В на привод воздушной заслонки, реле 10 А
	digitalWrite(AIR_DAMPER_RELE_PIN, RELE_HIGH);
	ComPort.println(F("SE: airdamper rele ON"));
	delay(prev_starter_DEL);
	// - Подаем напряжение +12В на стартер, используем реле 30А, длительность 2 сек (1-5 сек)
	digitalWrite(STARTER_RELE_PIN, STARTER_RELE_HIGH);
	ComPort.println(F("SE: STARTER rele ON"));
	delay((timeOut_starter_launch * 1000) + (_gain * gain_starter_launch * 1000));
	digitalWrite(STARTER_RELE_PIN, STARTER_RELE_LOW);
	delay(timeOut_operation);
	delay(timeOut_operation_2);

	zapushen_s_popitki ++;

	// - Контролируем запуск генератора появлением напряжения на выходе 200В
	ComPort.println(F("SE: 220 sensor re-read"));
	if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
		// - Если запуск был успешным, снимаем напряжение с привода воздушной заслонки
		ComPort.println(F("__dvigatel zapushen"));
		digitalWrite(AIR_DAMPER_RELE_PIN, RELE_LOW);
		vremya_zapuska_dvigatelya = millis();
		return 1;  // ДВИГАТЕЛЬ УСПЕШНО ЗАПУЩЕН
	}
	else{
		ComPort.println(F("__dvigatel ne zapustilsa"));
		return 0;  // ДВИГАТЕЛЬ НЕ ЗАПУСТИЛСЯ
	}
}

// - Если генератор не запустился, проводим повторный запуск через 5-15 сек (возможность настройки интервала), 
//   количество запусков должно настраиваться. Если генератор не запустился после указанного количества запусков, 
//   оператор  должен получить сигнал об ошибке и генератор переходит в режим откл.
int16_t startEngineLoopF(){  // функция запуска двигателя пока не заведется
	int16_t _startEngStatus = 0;
	for(int i=0; i<number_starter_launch; i++){
		// - Посылаем команду на запуск двигателя и смотрим ответ от функции
		_startEngStatus = startEngineSingleF(i);
		if(_startEngStatus != 0){return _startEngStatus;}  // если генератор запустился выходим из цикла запуска
		// - Если генератор не запустился, проводим повторный запуск через 5-15 сек (возможность настройки интервала), 
		if(i < (number_starter_launch - 1)){
			ComPort.println(F("__dvigatel_ne_zapustilsa_poprobuem_eshe_raz"));
			delay(timeOut_next_start);
		}
		// - Если генератор не запустился после указанного количества запусков, оператор  должен получить сигнал об ошибке и генератор переходит в режим откл.
		else if(i == (number_starter_launch - 1)){
			ComPort.println(F("__dvigatel_ne_zapustilsa_popitok_bolshe_net_avariynaya_ostanovka"));
			// ВЫПОЛНИТЬ АВАРИЙНУЮ ОСТАНОВКУ:
		}
	}
	return _startEngStatus;
}

int16_t stopGeneratorF(){  // функция остановки генератора
	if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == N_220){return -1;}
	for(int i=0; i<number_stop_attempt; i++){ 
		digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_HIGH);
		delay(stop_rele_time * 1000);
		if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == N_220){
			// генератор остановился
			digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);
			ComPort.println(F("__dvigatel_ostanovlen"));
			obshaya_narabotka_v_motochasah += (millis() - vremya_zapuska_dvigatelya) / 1000 / 60 / 60;
			obshaya_narabotka_v_motominutah += (millis() - vremya_zapuska_dvigatelya) / 1000 / 60;
			EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOCHASAH_ADDR, obshaya_narabotka_v_motochasah);
			EEPROM.put(OBSHAYA_NARABOTKA_V_MOTOMINUTAH_ADDR, obshaya_narabotka_v_motominutah);
			return 1;
		}
	}
	// генератор не остановился
	digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);
	ComPort.println(F("__dvigatel_NE_OSTANOVLEN"));
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// general algoritme F
int16_t _gf_ansver = 2;
bool _correct = false;                                       // Для оптимизации кода, переменная корректности команды
String _msgToSend = "init_msg";
String _first_simbol = "aaaa";
String _pass = "*****";
String _set_com = "com(";
String _set_val_str = "20";
int _set_val_int = 0;
void processSMScommands(String _sms_command, String _phone){

	ComPort.println(F("-SMScommand prosessing-"));  // Выводим в ComPort для контроля, что ничего не потерялось
	ComPort.print(F("-Key: "));                // Выводим в ComPort для контроля, что ничего не потерялось
	ComPort.println(_sms_command);                // Выводим в ComPort для контроля, что ничего не потерялось
	
	_gf_ansver = 2;
	_correct = false;                                       // Для оптимизации кода, переменная корректности команды
	_msgToSend = "init_msg";
	
	if (_sms_command.length() >= 4 && _sms_command.length() <= 50) {

		_first_simbol = (String)_sms_command[0] + (String)_sms_command[1] + (String)_sms_command[2] + (String)_sms_command[3];

		if(_first_simbol == "star" || _first_simbol == "Star"){  // Исполняем команду Start
			zapushen_s_popitki = 0;
			_gf_ansver = startEngineLoopF();
			if(_gf_ansver == -1){_msgToSend = "generator uje rabotaet";}
			else if(_gf_ansver == 0){_msgToSend = "generator ne zapushen!";}
			else if(_gf_ansver == 1){_msgToSend = "generator zapushen s " + (String)zapushen_s_popitki + "popitki";}
			_correct = true;
		}
		else if(_first_simbol == "stop" || _first_simbol == "Stop"){  // Исполняем команду Stop
			_gf_ansver = stopGeneratorF();
			if(_gf_ansver == -1){_msgToSend = "generator ne zapushen";}
			else if(_gf_ansver == 0){_msgToSend = "generator ne ostanovlen!";}
			else if(_gf_ansver == 1){_msgToSend = "generator ostanovlen";}
			_correct = true;
		}
		else if(_first_simbol == "stat" || _first_simbol == "Stat"){  // Исполняем команду Status
			if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
				ComPort.println(F("__dvigatel_zapushen"));
				ComPort.println(F("__dvigatel_rabotaet X minut"));
				ComPort.println(F("__timers"));
			}
			else{
				ComPort.println(F("__dvigatel_ostanovlen"));
			}
			_correct = true;
		}
		else if(_first_simbol == "set_" || _first_simbol == "Set_"){  // переходим к проверке пароля и настройкам set_pasms()()()
			ComPort.println(F("COMAND SETTING RUN"));  // Выводим в ComPort для контроля, что ничего не потерялось
			 _pass = (String)_sms_command[4] + (String)_sms_command[5] + (String)_sms_command[6] + (String)_sms_command[7] + (String)_sms_command[8];
			if(_pass == sms_password){
				ComPort.println(F("pass_OK"));

				// количество повторных пусков:                                kpp=(x)
				// время работы стартера, прирост для следующей попытки 1 сек: vrs=(x)
				// время глушения двигателя:                                   vgd=(x)

				_set_com = (String)_sms_command[10] + (String)_sms_command[11] + (String)_sms_command[12] + (String)_sms_command[13] + (String)_sms_command[14];
				_set_val_str = "";
				for(int i=15; i<_sms_command.length(); i++){
					if((String)_sms_command[i] == ")"){break;}
				    _set_val_str += (String)_sms_command[i];
				}

				_set_val_int = 0;
				for(int i=0; i<_set_val_str.length(); i++){
					_set_val_int *= 10;
				    _set_val_int = ((String)_set_val_str[0]).toInt();               // Получаем первую цифру
				}

				ComPort.print("...setting value = ");
				ComPort.println(_set_val_int);

				if(_set_com == "kpp=("){}
				if(_set_com == "vrs=("){}
				if(_set_com == "vgd=("){}
			}	
			ComPort.println("setting_OK");
			_correct = true;
		}
		else if(_first_simbol == "rese" || _first_simbol == "Rese"){  // исполняем команду Reset
			_msgToSend = "motohours reseted";
			_correct = true;
		}
		else if(_first_simbol == "time" || _first_simbol == "Time"){  // переходим к настройке таймеров
			_msgToSend = "timers setting";
			_correct = true;
		}
		else if(_first_simbol == "help" || _first_simbol == "Help"){  // справка
			ComPort.println("COMAND HELP RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			ComPort.println("help_OK");
			_correct = true;
			_msgToSend = "spravka v razrabotke";
			_correct = true;
		}
	}
	if (!_correct) {
		_msgToSend = "Incorrect command: " + _sms_command;               // Статус исполнения
	}
	ComPort.print(F("-MSG to send:  "));
	ComPort.println(_msgToSend);                                  // Выводим результат в терминал
	// sendSMS(masterPhones, _msgToSend);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void generalLoop(){  // главная функция обработки входящих СМС
	if(lastUpdate + updatePeriod < millis() ) {                               // Пора проверить наличие новых сообщений
		do{
			_response = sendATCommand("AT+CMGL=\"REC UNREAD\",1", true);      // Отправляем запрос чтения непрочитанных сообщений
			if(_response.indexOf("+CMGL: ") > -1) {                           // Если есть хоть одно, получаем его индекс
				int msgIndex = _response.substring(_response.indexOf("+CMGL: ") + 7, _response.indexOf("\"REC UNREAD\"", _response.indexOf("+CMGL: ")) - 1).toInt();
				char i = 0;                                                   // Объявляем счетчик попыток
				do{
					i++;                                                      // Увеличиваем счетчик
					_response = sendATCommand("AT+CMGR=" + (String)msgIndex + ",1", true);  // Пробуем получить текст SMS по индексу
					_response.trim();                                         // Убираем пробелы в начале/конце
					if(_response.endsWith("OK")) {                            // Если ответ заканчивается на "ОК"
						if(!hasmsg){hasmsg = true;}                           // Ставим флаг наличия сообщений для удаления
						sendATCommand("AT+CMGR=" + (String)msgIndex, true);   // Делаем сообщение прочитанным
						sendATCommand("\n", true);                            // Перестраховка - вывод новой строки
						parseSMS(_response);                                  // Отправляем текст сообщения на обработку
						break;                                                // Выход из do{}
					}
					else{                                                     // Если сообщение не заканчивается на OK
						ComPort.println ("Error answer");                     // Какая-то ошибка
						sendATCommand("\n", true);                            // Отправляем новую строку и повторяем попытку
					}
				}while (i < 10);
				break;
			}
			else{
				lastUpdate = millis();                                        // Обнуляем таймер
				if (hasmsg){
					sendATCommand("AT+CMGDA=\"DEL READ\"", true);             // Удаляем все прочитанные сообщения
					hasmsg = false;
				}
				break;
			}
		}while(1);
	}

	if (SIM800.available())   {                     // Если модем, что-то отправил...
		_response = waitResponse();                 // Получаем ответ от модема для анализа
		_response.trim();                           // Убираем лишние пробелы в начале и конце
		ComPort.println(_response);                 // Если нужно выводим в монитор порта
		if (_response.indexOf("+CMTI:")>-1) {       // Пришло сообщение об отправке SMS
			// Теперь нет необходимости обрабатываеть SMS здесь, достаточно просто
			lastUpdate = millis() -  updatePeriod;  // сбросить счетчик автопроверки и в следующем цикле все будет обработано                                                 
		}
	}

	if (ComPort.available())  {                     // Ожидаем команды по ComPort...
		SIM800.write(ComPort.read());               // ...и отправляем полученную команду модему
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ЛупСэтап F
void setup(){

	// инициализация выводов микроконтроллера:

	// модем
	pinMode(RESET_MODEM_PIN, INPUT);

	// встроенный светодиод
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	// стартер
	pinMode(STARTER_RELE_PIN, OUTPUT);
	digitalWrite(STARTER_RELE_PIN, STARTER_RELE_LOW);

	// воздушная заслонка
	pinMode(AIR_DAMPER_RELE_PIN, OUTPUT);
	digitalWrite(AIR_DAMPER_RELE_PIN, RELE_LOW);

	// стоп генератора
	pinMode(STOP_GENERATOR_RELE_PIN, OUTPUT);
	digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);

	// вход датчика 220В
	pinMode(VOLTAGE_VCC_SENSOR_PIN, OUTPUT);
	digitalWrite(VOLTAGE_VCC_SENSOR_PIN, HIGH);
	pinMode(VOLTAGE_OUT_SENSOR_PIN, INPUT);
	pinMode(VOLTAGE_GND_SENSOR_PIN, OUTPUT);
	digitalWrite(VOLTAGE_GND_SENSOR_PIN, LOW);

	// доп земля
	pinMode(GND_PIN, OUTPUT);
	digitalWrite(GND_PIN, LOW);

	// инициализация UART:
	ComPort.begin(9600);
	Sim800Port.begin(9600);

	delay(100);

	// инициализация модема:
	delay(50);
	#if UART_BRIDGE_EN == 0
	gsmModemStart();
	#endif

	// outgoingCall(masterPhones);  // исходящий вызов при перезагрузке, если нужно

	digitalWrite(13, LOW);

	sendATCommand("AT+CMGDA=\"DEL ALL\"", true);      // Удалить все сообщения, чтобы не забивали память модуля (txt mode)
  	sendSMS(masterPhones, "controller RESTARTED\nall previous SMS deleted");
	
	eeSetup();
}


void loop(){
	#if UART_BRIDGE_EN == 0
	generalLoop();  // check GSM uart and control
	#else
	trx_Gsm_Port();  // trx для работы с модемом напрямую
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
