#include "ttks_v2_config.h"

// MASTER SETTING AND GLOBAL VARIABLES
// мастер настройки. сюда лучше не лезть

	#include <SoftwareSerial.h>

	SoftwareSerial SoftSerial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX

	// настройка портов GSM serial 0: com=hard, gsm=soft.   1: com=soft, gsm=hard.
	#define GSM_SERIAL_MODE 1

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// user F

	// ЗАПУСК
	// - Повернуть ключ в положение ON (размыкаем контакт на глушение генератора), руками.
	// - Посылаем команду на запуск (с телефона)
	int16_t startEengineSingleF(){  // функция запуска двигателя
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
	int16_t startEengineLoopF(){
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

	int16_t stopGeneratorF(){
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
		#if AMR_DOWNLOAD == 1
		gsmModemStart();
		#endif

		// outgoingCall(masterPhones);  // исходящий вызов при перезагрузке, если нужно

		digitalWrite(13, LOW);
	}


	void loop(){
		#if AMR_DOWNLOAD == 1
		checkRINGgeneralLoop();  // check GSM uart and control
		#else
		trx_Gsm_Port();  // trx для работы с модемом напрямую
		#endif
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
