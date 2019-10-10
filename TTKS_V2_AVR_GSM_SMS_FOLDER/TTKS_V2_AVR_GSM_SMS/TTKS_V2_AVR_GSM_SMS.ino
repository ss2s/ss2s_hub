#include <SoftwareSerial.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define UART_BRIDGE_EN 0  // 0-для работы системы в штатном режиме. 1-для организации моста с модемом.  // default 0
// если прошить с 0 то программа будет работать в штатном режиме, обработка смс, таймеров и запуск генератора по команде
// если прошить с 1 то программа будет работать как мост для настройки модема
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// РАСПИНОВКА:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// здесь настраиваются выводы микроконтроллера

	#define RESET_MODEM_PIN 8                  // RESET sim800 optional

	#define STARTER_RELE_PIN A1                // РЕЛЕ СТАРТЕРА 30А ИЛИ МОСФЕТ 30А

	#define AIR_DAMPER_RELE_PIN A2             // РЕЛЕ ВОЗДУШНОЙ ЗАСЛОНКИ
	#define STOP_GENERATOR_RELE_PIN A3         // РЕЛЕ ОСТАНОВКИ ГЕНЕРАТОРА
	#define FUEL_GATE_RELE_PIN 7               // РЕЛЕ КРАНИКА ТОПЛИВА

	#define VOLTAGE_VCC_SENSOR_PIN 4           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ VCC
	#define VOLTAGE_OUT_SENSOR_PIN 5           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ OUT
	#define VOLTAGE_GND_SENSOR_PIN 6           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ GND

	#define SOFT_RX_PIN 10                     // SoftSerial RX
	#define SOFT_TX_PIN 11                     // SoftSerial TX
	#define GND_PIN 12                         // GND

	#define BATTERY_12V_VOLTAGE_SENSOR_PIN A0  // НАПРЯЖЕНИЯ НА АККУМУЛЯТОРЕ ГЕНЕРАТОРА
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                              ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// НАСТРОЙКИ ПРОГРАММЫ:
	
	// VERSION SELECTION. ВЫБОР ВЕРСИИ
	#define PRODUCT_VERSION 2  // ВЕРСИЯ ПРОДУКТА: 1-базовая; 2-расширеная;
	// COMPOSITION OF VERSIONS. СОСТАВ ВЕРСИЙ
	#define VERSION_USER_TIMERS_EN 2
	#define VERSION_TOTAL_RUN_TIME_REPORT_EN 2
	#define VERSION_OIL_REMINDER_EN 2
	#define VERSION_LOAD_REPORT_EN 2

	// КНОПКИ ТЕЛЕФОНА
	#define BUTTON_GENERATOR_START "1"
	#define BUTTON_STOP_GENERATOR "2"
	#define BUTTON_GENERATOR_STATUS "3"
	#define BUTTON_SETTING "#"   // to future...
	#define BUTTON_REMINDER "0"  // reminder

	// номера телефонов с которых будет принимать звонок
	String whiteListPhones = "+380xxxxxxxxx, +380xxxxxxxxx, +380xxxxxxxxx"; // Белый список телефонов
	String masterPhones = "+380xxxxxxxxx";     // мастер телефон

	// настройки запуска
	uint32_t timeOut_starter_ON = 3000;    // столько работает стартер 2s (1-5s)
	uint8_t  number_launch_attempt = 5;    // количество попыток запуска, при неудачном старте. MAX255

	uint32_t timeOut_stop_rele_ON = 5000;  // столько работает реле глущения генератора 2s (1-5s)
	uint8_t  number_stop_attempt = 3;      // количество попыток остановки, при неудачном старте. MAX255

	uint32_t voice_timeout = 5000;         // задержка для голосового ответа >5s

	// задержки
	#define timeOut_operation 50           // other operation timeout 0.05s
	#define timeOut_operation_2 1000       // other operation timeout 2 = 0.5s // задержка после отключения стартера
	#define timeOut_prev_starter_DEL 500   // задержка перед запуском стартера, после открытия заслонки 0.5s
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// КОНЕЦ ВСЕХ НАСТРОЕК
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                              ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "ttks_v2_config.h"



// MASTER SETTING AND GLOBAL VARIABLES
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
	String _response = "R";  // Переменная для хранения ответа модуля
	String lastPhoneNumber = masterPhones;  // последний номер телефона для ответного смс с показаниями
	bool alarmFlag1 = 0;

	bool flag_Start = 0;
	bool flag_Stop = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// system F

	void trx_Gsm_Port(){  // сервисная функция test UART F
		if (Sim800Port.available()){ComPort.write(Sim800Port.read());}
		if (ComPort.available()){Sim800Port.write(ComPort.read());}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	String sendATCommand(String cmd, bool waiting){   // сервисная функция отправки АТ команды на модем
		String _resp = "";                            // Переменная для хранения результата
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	String waitResponse(){  // сервисная функция ожидания ответа и возврата полученного результата
		String _resp = "";                            // Переменная для хранения результата
		long _timeout = millis() + 15000;             // Переменная для отслеживания таймаута (10 секунд)
		while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 15 секунд, если пришел ответ или наступил таймаут, то...
		if (SIM800.available()) {                     // Если есть, что считывать...
			_resp = SIM800.readString();              // ... считываем и запоминаем
		}
		else {                                        // Если пришел таймаут, то...
			ComPort.println("Timeout...");            // ... оповещаем об этом и...
			pinMode(RESET_MODEM_PIN, OUTPUT);
			digitalWrite(RESET_MODEM_PIN, LOW);
			delay(1000);
			digitalWrite(RESET_MODEM_PIN, HIGH);
			pinMode(RESET_MODEM_PIN, INPUT);
			delay(5000);
		}
		return _resp;                                 // ... возвращаем результат. Пусто, если проблема
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void outgoingCall (String phNum){  // сервисная функция исходящий вызов
		// phNum = masterPhones;
		String _call = "ATD" + phNum + ";";
		_response = sendATCommand(_call, true);
	}

	void smsTransmitter(){  // сервисная функция исходящее сообщение

	}
	void smsReceiver(){  // сервисная функция входящее сообщение

	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// user F

	// ЗАПУСК
	// - Повернуть ключ в положение ON (размыкаем контакт на глушение генератора), руками.
	// - Посылаем команду на запуск (с телефона)
	int16_t startEengineSingleF(){  // функция запуска двигателя 1 раз
		// - Проверяем наличие напряжения на генераторе (контроль того, что генератор не запущен)
		if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_uzhe_zapushen"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
			return -1;  // ДВИГАТЕЛЬ УЖЕ ЗАПУЩЕН
		}
		// - Подаем напряжение +12В на привод воздушной заслонки, реле 10 А
		digitalWrite(AIR_DAMPER_RELE_PIN, RELE_HIGH);
		delay(timeOut_prev_starter_DEL);
		// - Подаем напряжение +12В на стартер, используем реле 30А, длительность 2 сек (1-5 сек)
		digitalWrite(STARTER_RELE_PIN, STARTER_RELE_HIGH);
		delay(timeOut_starter_ON);
		digitalWrite(STARTER_RELE_PIN, STARTER_RELE_LOW);
		delay(timeOut_operation);
		delay(timeOut_operation_2);
		// - Контролируем запуск генератора появлением напряжения на выходе 200В
		if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
			// - Если запуск был успешным, снимаем напряжение с привода воздушной заслонки
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_zapushen"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
			digitalWrite(AIR_DAMPER_RELE_PIN, RELE_LOW);
			return 1;  // ДВИГАТЕЛЬ УСПЕШНО ЗАПУЩЕН
		}
		else{
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_ne_zapustilsa"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
			return 0;  // ДВИГАТЕЛЬ НЕ ЗАПУСТИЛСЯ
		}
	}

	// - Если генератор не запустился, проводим повторный запуск через 5-15 сек (возможность настройки интервала), 
	//   количество запусков должно настраиваться. Если генератор не запустился после указанного количества запусков, 
	//   оператор  должен получить сигнал об ошибке и генератор переходит в режим откл.
	int16_t startEengineLoopF(){  // функция запуска двигателя пока не заведется
		int16_t _startEngStatus = 0;
		for(int i=0; i<number_launch_attempt; i++){
			// - Посылаем команду на запуск двигателя и смотрим ответ от функции
			_startEngStatus = startEengineSingleF();
			if(_startEngStatus != 0){return _startEngStatus;}  // если генератор запустился выходим из цикла запуска
			// - Если генератор не запустился, проводим повторный запуск через 5-15 сек (возможность настройки интервала), 
			if(i < (number_launch_attempt - 1)){
				SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("poprobuem_eshe_raz"), SIM800.println(".amr\",1,100");
				// delay(voice_timeout);
				delay(timeOut_next_start);
			}
			// - Если генератор не запустился после указанного количества запусков, оператор  должен получить сигнал об ошибке и генератор переходит в режим откл.
			else if(i == (number_launch_attempt - 1)){
				SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("popitok_net_avariynaya_ostanovka"), SIM800.println(".amr\",1,100");
				delay(voice_timeout);
				// ВЫПОЛНИТЬ АВАРИЙНУЮ ОСТАНОВКУ:
			}
		}
		return _startEngStatus;
	}

	int16_t stopGeneratorF(){  // функция остановки генератора
		for(int i=0; i<number_stop_attempt; i++){ 
			digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_HIGH);
			delay(timeOut_stop_rele_ON);
			if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == N_220){
				// генератор остановился
				digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);
				SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_ostanovlen"), SIM800.println(".amr\",1,100");
				delay(voice_timeout);
				return 1;
			}
		}
		// генератор не остановился
		digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("funkcia_eshe_nedostupna"), SIM800.println(".amr\",1,100");
		delay(voice_timeout);
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("popitok_net_avariynaya_ostanovka"), SIM800.println(".amr\",1,100");
		delay(voice_timeout);
		return 0;
	}
	
	void gsmModemStart(){  // стартовая функция для старта модема

		delay(100);
		// выбор режима ответов модема, текстовый или цифровой
		Sim800Port.println("AT");  // автонастройка скорости
		delay(100);
		Sim800Port.println("ATE1V1+CMEE=2;&W");  // text
		// Sim800Port.println("ATE0V0+CMEE=1;&W");  // digital
		delay(50);

		ComPort.println("");
		ComPort.print("sys: ");
		ComPort.println("Start system");

		// sendATCommand("AT", true);                     // Автонастройка скорости
		// sendATCommand("ATV1", true);                   // настройка режиа ответа, текстовый
		uint32_t blinkSetupTimeStart = millis();
		do {
	    	_response = sendATCommand("AT", true);        // проверка модема
	    	_response.trim();                             // Убираем пробельные символы в начале и конце

	    	if(millis()-blinkSetupTimeStart >= 500){
	    		digitalWrite(13, !digitalRead(13));
	    		blinkSetupTimeStart = millis();
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

		sendATCommand("AT+CMGF=1", true);                 // Включаем текстовый режим SMS (Text mode) txt comand mode
		sendATCommand("AT+CMGDA=\"DEL ALL\"", true);      // Удалить все сообщения, чтобы не забивали память модуля (txt mode)
		// sendATCommand("AT+CMGDA=6", true);             // Удалить все сообщения, чтобы не забивали память модуля (pdu mode)

		sendATCommand("AT+VTD=3", true);                  // установить длительность DTMF генератора
		ComPort.println("\nMODEM START OK\nRUN GENERAL LOOP");        //
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void processingDTMF(String symbol){  // функция обработки нажатия кнопок

		// ComPort.println("Key: " + symbol);                // Выводим в ComPort для контроля, что ничего не потерялось

		// СТАРТ ГЕНЕРАТОРА
		if(symbol == BUTTON_GENERATOR_START){
			ComPort.println("COMAND START GENERATORA RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			// SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("komanda_start_dvigatela"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
			startEengineLoopF();
		}

		// СТОП ГЕНРАТОРА
		else if(symbol == BUTTON_STOP_GENERATOR){
			ComPort.println("COMAND STOP GENERATORA RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("komanda_stop_dvigatela"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
			stopGeneratorF();
		}

		// СТАТУС ГЕНРАТОРА
		else if(symbol == BUTTON_GENERATOR_STATUS){
			ComPort.println("COMAND STATUS GENERATORA RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			if(digitalRead(VOLTAGE_OUT_SENSOR_PIN) == Y_220){
				SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_zapushen"), SIM800.println(".amr\",1,100");
				delay(voice_timeout);
			}
			else{
				SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("dvigatel_ostanovlen"), SIM800.println(".amr\",1,100");
				delay(voice_timeout);
			}
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("funkcia_eshe_nedostupna"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
		}

		// НАСТРОЙКИ
		else if(symbol == BUTTON_SETTING){
			ComPort.println("COMAND SETTING RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("funkcia_eshe_nedostupna"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
		}

		// НАПОМНИТЬ КНОПКИ УПРАВЛЕНИЯ
		else if(symbol == BUTTON_REMINDER){
			ComPort.println("COMAND REMINDER RUN");  // Выводим в ComPort для контроля, что ничего не потерялось
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print("funkcia_eshe_nedostupna"), SIM800.println(".amr\",1,100");
			delay(voice_timeout);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void checkRINGgeneralLoop(){  // главная функция обработки входящих звонков
		if (SIM800.available())   {                     // Если модем, что-то отправил...
			_response = waitResponse();                 // Получаем ответ от модема для анализа

			String respDTMF = _response;
			String respRING = _response;

			// ComPort.println(_response);                 // Если нужно выводим в монитор порта



			// ЕСЛИ ВХОДЯЩИЙ ЗВОНОК:
			respRING.trim();                            // Убираем лишние пробелы в начале и конце
			if (respRING.startsWith("RING")) {          // Есть входящий вызов
				int phoneindex = respRING.indexOf("+CLIP: \"");// Есть ли информация об определении номера, если да, то phoneindex>-1
				String innerPhone = "";                 // Переменная для хранения определенного номера
				if (phoneindex >= 0) {                  // Если информация была найдена
					phoneindex += 8;                    // Парсим строку и ...
					innerPhone = respRING.substring(phoneindex, respRING.indexOf("\"", phoneindex)); // ...получаем номер
					ComPort.println("Number: " + innerPhone); // Выводим номер в монитор порта
				}
				// Проверяем, чтобы длина номера была больше 6 цифр, и номер должен быть в списке
				if (innerPhone.length() >= 7 && whiteListPhones.indexOf(innerPhone) >= 0) {
					sendATCommand("ATA", true);         // Если да, то отвечаем на вызов
					lastPhoneNumber = innerPhone;
				}
				else {
					sendATCommand("ATH", true);         // Если нет, то отклоняем вызов
				}
			}



			// ЕСЛИ НАЖАТА КНОПКА НА ТЕЛЕФОНЕ:
			int index = -1;
			do  {                                       // Перебираем построчно каждый пришедший ответ
				index = respDTMF.indexOf("\r\n");       // Получаем идекс переноса строки
				String submsg = "";
				if (index > -1) {                       // Если перенос строки есть, значит
					submsg = respDTMF.substring(0, index); // Получаем первую строку
					respDTMF = respDTMF.substring(index + 2); // И убираем её из пачки
				}
				else {                                  // Если больше переносов нет
					submsg = respDTMF;                  // Последняя строка - это все, что осталось от пачки
					respDTMF = "";                      // Пачку обнуляем
				}
				submsg.trim();                          // Убираем пробельные символы справа и слева
				if (submsg != "") {                     // Если строка значимая (не пустая), то распознаем уже её
					// ComPort.println("submessage: " + submsg);
					if (submsg.startsWith("+DTMF:")) {  // Если ответ начинается с "+DTMF:" тогда:
						String symbol = submsg.substring(7, 8);  // Выдергиваем символ с 7 позиции длиной 1 (по 8)
						processingDTMF(symbol);         // Логику выносим для удобства в отдельную функцию
					}
				}
			} while (index > -1);                       // Пока индекс переноса строки действителен
		}





		if (ComPort.available())  {                     // Ожидаем команды по ComPort...
			SIM800.write(ComPort.read());               // ...и отправляем полученную команду модему
		}
	}

	void checkSMSgeneralLoop(){  // главная функция обработки входящих СМС
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// general algoritme F

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
	}


	void loop(){
		#if UART_BRIDGE_EN == 0
		checkSMSgeneralLoop();  // check GSM uart and control
		#else
		trx_Gsm_Port();  // trx для работы с модемом напрямую
		#endif
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
