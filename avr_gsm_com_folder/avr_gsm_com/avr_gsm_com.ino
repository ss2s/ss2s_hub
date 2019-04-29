
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AVR GSM CONTROLLER FOR GASOLINE GENERATOR
// license com
// created by SinAPPS
// e-mail: chuga.aleksandr.kso2@gmail.com
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  РАСПИНОВКА:

	#define RESET_MODEM_PIN 9          // RESET sim800 optional

	#define AIR_DAMPER_RELE_PIN 8      // РЕЛЕ ВОЗДУШНОЙ ЗАСЛОНКИ
	#define STOP_GENERATOR_RELE_PIN 7  // РЕЛЕ ОСТАНОВКИ ГЕНЕРАТОРА
	#define STARTER_RELE_PIN 6         // РЕЛЕ СТАРТЕРА 30А ИЛИ МОСФЕТ 30А

	#define VOLTAGE_220_SENSOR_PIN 5     // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ

	#define SOFT_RX_PIN 10  // SoftSerial RX
	#define SOFT_TX_PIN 11  // SoftSerial TX

// НАСТРОЙКИ:

	// КНОПКИ ТЕЛЕФОНА
	#define BUTTON_GENERATOR_START "1"
	#define BUTTON_STOP_GENERATOR "0"
	#define BUTTON_GENERATOR_STATUS "*"
	#define BUTTON_SETTING "#"  // to future...
	#define BUTTON_GET_SMS "0"

	// root pasword
	uint16_t rootPasword = 1234;  // pasword 4 digit number

	// номера телефонов с которых будет принимать звонок
	String whiteListPhones = "+380955478117, +380xxxxxxxxx, +380xxxxxxxxx"; // Белый список телефонов
	String masterPhones = "+380955478117"; // мастер телефон

	// задержки
	uint32_t timeOut_starter_ON = 2000;  // столько работает стартер 2s
	uint32_t timeOut_airDamper_OFF = 5000;  // задержка перед закрытием воздушной заслонки 5s

	// типы активации реле
	#define Y_220 1  // out sensor 220 yes
	#define N_220 0  // out sensor 220 no
	#define RELE_HIGH 0  // 10A rele high
	#define RELE_LOW 1   // 10A rele low
	#define STARTER_RELE_HIGH 0  // 30A starter rele high
	#define STARTER_RELE_LOW 1   // 30A starter rele low

	// настройка портов GSM serial 0: com=hard, gsm=soft.   1: com=soft, gsm=hard.
	#define SIM800 Sim800Port
	#define GSM_SERIAL_MODE 0
	#if GSM_SERIAL_MODE == 1
	#define Sim800Port Serial
	#define ComPort SoftSerial
	#else
	#define Sim800Port SoftSerial
	#define ComPort Serial
	#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// MASTER SETTING AND GLOBAL VARIABLES

	#include <SoftwareSerial.h>

	SoftwareSerial SoftSerial(SOFT_RX_PIN, SOFT_TX_PIN); // RX, TX

	// variables
	String _response = "R";  // Переменная для хранения ответа модуля
	String messageStr = "";  // message string
	String lastPhoneNumber = masterPhones;  // последний номер телефона для ответного смс с показаниями
	bool alarmFlag1 = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// system F

	void trx_Gsm_Port()
	{  // сервисная функция test UART F
		if (Sim800Port.available()){ComPort.write(Sim800Port.read());}
	  if (ComPort.available()){Sim800Port.write(ComPort.read());}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	String sendATCommand(String cmd, bool waiting)
	{  // сервисная функция отправки АТ команды на модем
		String _resp = "";                            // Переменная для хранения результата
		ComPort.print("cmd: ");                       // Дублируем команду в монитор порта
		ComPort.println(cmd);                         // Дублируем команду в монитор порта
		SIM800.println(cmd);                          // Отправляем команду модулю
		if (waiting) {                                // Если необходимо дождаться ответа...
			_resp = waitResponse();                     // ... ждем, когда будет передан ответ

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
	
	String waitResponse()
	{  // сервисная функция ожидания ответа и возврата полученного результата
		String _resp = "";                            // Переменная для хранения результата
		long _timeout = millis() + 15000;             // Переменная для отслеживания таймаута (10 секунд)
		while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 15 секунд, если пришел ответ или наступил таймаут, то...
		if (SIM800.available()) {                     // Если есть, что считывать...
			_resp = SIM800.readString();                // ... считываем и запоминаем
		}
		else {                                        // Если пришел таймаут, то...
			ComPort.println("Timeout...");               // ... оповещаем об этом и...
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
	
	void outgoingCall (String phNum)
	{  // сервисная функция исходящий вызов
		// phNum = masterPhones;
		String vizov = "ATD" + phNum + ";";
		_response = sendATCommand(vizov, true);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void sendSMS(String phone, String message)
	{  // сервисная функция отправки СМС
		sendATCommand("AT+VTS=\"9,0\"", true);              // Переходим в режим ввода текстового сообщения
		delay(700);
		sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
		sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// user F
	
	void gsmModemStart()
	{  // стартовая функция для старта модема
		ComPort.println("");
		ComPort.print("sys: ");
		ComPort.println("Start system");

		// sendATCommand("AT", true);                // Автонастройка скорости
		// sendATCommand("ATV1", true);                // Автонастройка скорости
		uint32_t blinkSetupTimeStart = millis();
		do {
	    	_response = sendATCommand("AT", true);  // проверка модема
	    	_response.trim();                       // Убираем пробельные символы в начале и конце

	    	if(millis()-blinkSetupTimeStart >= 500){
	    		digitalWrite(13, !digitalRead(13));
	    		blinkSetupTimeStart = millis();
	    	}
		} while (_response != "OK");              // Не пускать дальше, пока модем не вернет ОК
		digitalWrite(13, LOW);
		ComPort.println("sys: GSM modem is ready!");

		// Команды настройки модема при каждом запуске
		_response = sendATCommand("AT+CLIP=1", true);  // Включаем АОН
		_response = sendATCommand("AT+DDET=1", true);  // Включаем DTMF
		_response = sendATCommand("AT+CMGF=1;&W", true); // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
		// _response = sendATCommand("AT+CMGF=1", true);         // Включить TextMode для SMS
		// sendATCommand("AT+CLVL?", true);          // Запрашиваем громкость динамика

		do {
	    	_response = sendATCommand("AT+CLIP=1", true);  // Включаем АОН
	    	_response.trim();                       // Убираем пробельные символы в начале и конце
		} while (_response != "OK");              // Не пускать дальше, пока модем не вернет ОК

		ComPort.println("Opredelenie nomera VKL");            // Информируем, что АОН включен
		// Проверка пройдена, модем сообщил о готовности, можно запускать основной цикл...

		sendATCommand("AT+CMGDA=\"DEL ALL\"", true); // Удалить все сообщения, чтобы не забивали память модуля

		sendATCommand("AT+VTD=3", true); // установить длительность DTMF генератора
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void smsReportF(bool nTipe = 1)
	{  // функция отправки смс с отчетом о состоянии генератора
		String smsText = "";
		int16_t VAL_ = 14;

		if(nTipe == 0){
			smsText += "          ALARM !!!";
			smsText += "\n\nStr1: ";
		}
		else{
			smsText += "Str1: ";
		}
		smsText += String(VAL_);
		smsText += " postStr1";


		smsText += "\nStr2: ";
		smsText += String(VAL_);
		smsText += " postStr2";

		smsText += "\nStr3: ";
		smsText += String(VAL_);
		smsText += " postStr3";

		smsText += "\nStr4: ";
		if(VAL_ == 0){smsText += " R :(";}
		else{smsText += " S :)";}

		smsText += "\nStr5Sensor220: ";
		if(digitalRead(VOLTAGE_220_SENSOR_PIN) == Y_220){smsText += " ON";}
		else{smsText += " OFF";}

		smsText += "\nStr6Alarm: ";
		if(alarmFlag1 == 1){smsText += " ON";}
		else{smsText += " OFF";}



		if(nTipe == 0){sendSMS(masterPhones, smsText);}
		else{sendSMS(lastPhoneNumber, smsText);}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void processingDTMF(String symbol)
	{  // функция обработки нажатия кнопок
		ComPort.println("Key: " + symbol);             // Выводим в ComPort для контроля, что ничего не потерялось

		if(symbol == BUTTON_GENERATOR_START){
			// СТАРТ ГЕНЕРАТОРА
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
		}

		else if(symbol == BUTTON_STOP_GENERATOR){
			// СТОП ГЕНРАТОРА
			digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_HIGH);
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
		}

		else if(symbol == BUTTON_GENERATOR_STATUS){
			// СТАТУС ГЕНРАТОРА

			// if(digitalRead(RELE_PIN) == RELE_HIGH){
			// 	SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
			// }
			// else{
			// 	SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
			// }
		}

		else if(symbol == BUTTON_SETTING){
			// SETTING BEGIN
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
		}

		else if(symbol == BUTTON_GET_SMS){
			// ОТПРАВИТЬ СМС
			smsReportF(1);
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
		}

		// else if(symbol == BUTTON_ALARM_STATE){
		// 	if(motionDetectAlarmEn == 1){
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
		// 	}
		// 	else{
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
		// 	}
		// }
		// else if(symbol == BUTTON_ALL_STATE){
		// 	SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(14), SIM800.println(".amr\",0,100");
		// 	delay(4000);
		// 	bool _rs = digitalRead(RELE_PIN);
		// 	if(_rs == RELE_HIGH && motionDetectAlarmEn == 1){
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(9), SIM800.println(".amr\",0,100");
		// 	}
		// 	else if(_rs == RELE_LOW && motionDetectAlarmEn == 0){
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(10), SIM800.println(".amr\",0,100");
		// 	}
		// 	else if(_rs == RELE_HIGH && motionDetectAlarmEn == 0){
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
		// 		delay(4000);
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
		// 	}
		// 	else if(_rs == RELE_LOW && motionDetectAlarmEn == 1){
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
		// 		delay(4000);
		// 		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
		// 	}
		// }
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void checkRINGgeneralLoop()
	{  // главная функция обработки входящих звонков
		if (SIM800.available())   {                   // Если модем, что-то отправил...
			_response = waitResponse();                 // Получаем ответ от модема для анализа

			String respDTMF = _response;
			String respRING = _response;
			String respSMS = _response;

			ComPort.println(_response);                  // Если нужно выводим в монитор порта




			respRING.trim();                           // Убираем лишние пробелы в начале и конце
			if (respRING.startsWith("RING")) {         // Есть входящий вызов
				int phoneindex = respRING.indexOf("+CLIP: \"");// Есть ли информация об определении номера, если да, то phoneindex>-1
				String innerPhone = "";                   // Переменная для хранения определенного номера
				if (phoneindex >= 0) {                    // Если информация была найдена
					phoneindex += 8;                        // Парсим строку и ...
					innerPhone = respRING.substring(phoneindex, respRING.indexOf("\"", phoneindex)); // ...получаем номер
					ComPort.println("Number: " + innerPhone); // Выводим номер в монитор порта
				}
				// Проверяем, чтобы длина номера была больше 6 цифр, и номер должен быть в списке
				if (innerPhone.length() >= 7 && whiteListPhones.indexOf(innerPhone) >= 0) {
					sendATCommand("ATA", true);        // Если да, то отвечаем на вызов
					lastPhoneNumber = innerPhone;
				}
				else {
					sendATCommand("ATH", true);        // Если нет, то отклоняем вызов
				}
			}



			respSMS.trim();                           // Убираем лишние пробелы в начале и конце
			if (respSMS.startsWith("+CMGS:")) {       // Пришло сообщение об отправке SMS
				int index = respSMS.lastIndexOf("\r\n");// Находим последний перенос строки, перед статусом
				String result = respSMS.substring(index + 2, respSMS.length()); // Получаем статус
				result.trim();                            // Убираем пробельные символы в начале/конце

				if (result == "OK") {                     // Если результат ОК - все нормально
					ComPort.println ("Message was sent. OK");
					sendATCommand("AT+VTS=\"1,2,3,4,5\"", true);              // Переходим в режим ввода текстового сообщения
					delay(2000);
				}
				else {                                    // Если нет, нужно повторить отправку
					ComPort.println ("Message was not sent. Error");
				}
		    }



			int index = -1;
			do  {                                       // Перебираем построчно каждый пришедший ответ
				index = respDTMF.indexOf("\r\n");        // Получаем идекс переноса строки
				String submsg = "";
				if (index > -1) {                         // Если перенос строки есть, значит
					submsg = respDTMF.substring(0, index); // Получаем первую строку
					respDTMF = respDTMF.substring(index + 2); // И убираем её из пачки
				}
				else {                                    // Если больше переносов нет
					submsg = respDTMF;                     // Последняя строка - это все, что осталось от пачки
					respDTMF = "";                         // Пачку обнуляем
				}
				submsg.trim();                            // Убираем пробельные символы справа и слева
				if (submsg != "") {                       // Если строка значимая (не пустая), то распознаем уже её
					// ComPort.println("submessage: " + submsg);
					if (submsg.startsWith("+DTMF:")) {      // Если ответ начинается с "+DTMF:" тогда:
						String symbol = submsg.substring(7, 8);  // Выдергиваем символ с 7 позиции длиной 1 (по 8)
						processingDTMF(symbol);               // Логику выносим для удобства в отдельную функцию
					}
				}
			} while (index > -1);                       // Пока индекс переноса строки действителен
		}





		if (ComPort.available())  {                    // Ожидаем команды по ComPort...
			SIM800.write(ComPort.read());                // ...и отправляем полученную команду модему
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void checkAlarmSensorGeneralLoop()
	{  // функция обработки сенсоров безопасности
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// general algoritme F
	void setup()
	{	
		// пресекаем запуск стартера если тип реле 1 при старте системы
		pinMode(STARTER_RELE_PIN, OUTPUT);
		digitalWrite(STARTER_RELE_PIN, RELE_LOW);

		// ставим в ВЫКЛ остальные реле
		pinMode(AIR_DAMPER_RELE_PIN, OUTPUT);
		pinMode(STOP_GENERATOR_RELE_PIN, OUTPUT);
		digitalWrite(AIR_DAMPER_RELE_PIN, RELE_LOW);
		digitalWrite(STOP_GENERATOR_RELE_PIN, RELE_LOW);

		// Sim800Port.println("ATE0V0+CMEE=1;&W");  // digital
		// Sim800Port.println("ATE1V1+CMEE=2;&W");  // text

		pinMode(VOLTAGE_220_SENSOR_PIN, INPUT);

		pinMode(13, OUTPUT);
		digitalWrite(13, HIGH);

		ComPort.begin(9600);
		Sim800Port.begin(9600);

		delay(100);

		gsmModemStart();

		// outgoingCall(masterPhones);

		String strtMsgStr = "MicroController restart"; sendSMS(masterPhones, strtMsgStr);
	}


	void loop()
	{
		// trx_Gsm_Port();  // trx
		// checkRINGgeneralLoop();  // check GSM uart and control
		checkAlarmSensorGeneralLoop();  // check alarm sensor
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
