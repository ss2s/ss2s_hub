#include "HX711.h"
#include "TroykaDHT.h"
#include <SoftwareSerial.h>

//  РАСПИНОВКА:
#define RESET_MODEM_PIN 9  // RESET sim800
#define RELE_PIN 8  // РЕЛЕ
#define RAIN_SENSOR_VCC_PIN 7  // ДАТЧИК ДОЖДЯ VCC
#define RAIN_SENSOR_DIGITAL_PIN 6  // ДАТЧИК ДОЖДЯ ЦИФРОВОЙ ВХОД
#define RAIN_SENSOR_ANALOG_PIN A1  // ДАТЧИК ДОЖДЯ АНАЛОГОВЫЙ ВХОД
#define MOTION_SENSOR_PIN 5  // ДАТЧИК ДВИЖЕНИЯ
#define DHT_DATA_PIN 4 // ДАТЧИК ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ DHT 22
#define HX711_DOUT_PIN A2  // ВЕСЫ DT
#define HX711_PD_SCK_PIN A3  // ВЕСЫ SCK
SoftwareSerial SoftSerial(10, 11); // RX, TX

// НАСТРОЙКИ:
String whiteListPhones = "+380978254069, +3809xxxxxxxx, +3809xxxxxxxx"; // Белый список телефонов
String reportMasterPhones = "+380978254069"; // телефон sms
uint32_t timeOutMotionDetector = 1000;  // 1s
uint32_t timeOutRainDetector = 1000*60*30;  // 30m
uint32_t timeOutTempCheck = 1000*60*30;  // 30m


// variables
String _response = "R";  // Переменная для хранения ответа модуля
uint32_t strtTimeoutRainPwr = 0;

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
// Serial.
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(DHT_DATA_PIN, DHT21);

#define HX711_GAIN 64  // усилитель 64 chanel A
HX711 hx711Obj;

bool VAL_MOTION_DETECT = 0;
bool VAL_RAIN_DETECT = 0;

int VAL_WEIGHT = 0;

int VAL_TEMP = 0;
int VAL_HMDT = 0;

void getDhtDataInt(void)
{
  	// считывание данных с датчика
	dht.read();
	// проверяем состояние данных
	switch(dht.getState())
	{
		// всё OK
		case DHT_OK:
		// выводим показания влажности и температуры
		VAL_TEMP = dht.getTemperatureC();
		VAL_HMDT = dht.getHumidity();
		
		// выводим показания влажности и температуры
		ComPort.print("Temperature = ");
		ComPort.print(VAL_TEMP);
		// ComPort.print(dht.getTemperatureC());
		ComPort.println(" C \t");
		ComPort.print("Humidity = ");
		ComPort.print(VAL_HMDT);
		// ComPort.print(dht.getHumidity());
		ComPort.println(" %\n");
		break;
		// ошибка контрольной суммы
		case DHT_ERROR_CHECKSUM:
		ComPort.println("Checksum error");
		break;
		// превышение времени ожидания
		case DHT_ERROR_TIMEOUT:
		ComPort.println("Time out error");
		break;
		// данных нет, датчик не реагирует или отсутствует
		case DHT_ERROR_NO_REPLY:
		ComPort.println("Sensor not connected");
		break;
	}
  
  // ждём две секунды
  delay(2000);
}

int getWeight()
{
	VAL_WEIGHT = 500;
	return VAL_WEIGHT;
}

bool getMotoinState()
{
	VAL_MOTION_DETECT = digitalRead(MOTION_SENSOR_PIN);
	return VAL_MOTION_DETECT;
}


bool getRainStateForLoop()
{
	digitalWrite(RAIN_SENSOR_VCC_PIN, HIGH);
	if(strtTimeoutRainPwr - millis() >= 50){

		VAL_RAIN_DETECT = digitalRead(RAIN_SENSOR_DIGITAL_PIN);
		digitalWrite(RAIN_SENSOR_VCC_PIN, LOW);
		return VAL_RAIN_DETECT;
	}
	return 0;
}

bool getRainState()
{
	digitalWrite(RAIN_SENSOR_VCC_PIN, HIGH);
	delay(50);
	VAL_RAIN_DETECT = digitalRead(RAIN_SENSOR_DIGITAL_PIN);
	digitalWrite(RAIN_SENSOR_VCC_PIN, LOW);
	return VAL_RAIN_DETECT;
}

void trx_Gsm_Port()
{
	if (Sim800Port.available()){ComPort.write(Sim800Port.read());}
  if (ComPort.available()){Sim800Port.write(ComPort.read());}
}

String sendATCommand(String cmd, bool waiting)
{
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

String waitResponse()
{                         // Функция ожидания ответа и возврата полученного результата
	String _resp = "";                            // Переменная для хранения результата
	long _timeout = millis() + 10000;             // Переменная для отслеживания таймаута (10 секунд)
	while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
	if (SIM800.available()) {                     // Если есть, что считывать...
		_resp = SIM800.readString();                // ... считываем и запоминаем
	}
	else {                                        // Если пришел таймаут, то...
		ComPort.println("Timeout...");               // ... оповещаем об этом и...
	}
	return _resp;                                 // ... возвращаем результат. Пусто, если проблема
}

void outgoingCall (String phNum)
{
	// phNum = reportMasterPhones;
	String vizov = "ATD" + phNum + ";";
	_response = sendATCommand(vizov, true);
}

void sendSMS(String phone, String message)
{
	sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
	sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z

}

void gsmModemStart()
{
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
}

void processingDTMF(String symbol) {
	ComPort.println("Key: " + symbol);             // Выводим в ComPort для контроля, что ничего не потерялось
}

void sendSMSloop()
{

}

void checkDTMFloop()
{

}

void checkRINGgeneralLoop()
{
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
				else if (respRING.startsWith("RING")) {   // При входящем звонке...
					// sendATCommand("ATA", true);           // ...отвечаем (поднимаем трубку)
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
					}
					else {
						sendATCommand("ATH", true);        // Если нет, то отклоняем вызов
					}
				}
				else if (respSMS.startsWith("+CMGS:")) {       // Пришло сообщение об отправке SMS
					int index = respSMS.lastIndexOf("\r\n");// Находим последний перенос строки, перед статусом
					String result = respSMS.substring(index + 2, respSMS.length()); // Получаем статус
					result.trim();                            // Убираем пробельные символы в начале/конце

					if (result == "OK") {                     // Если результат ОК - все нормально
						ComPort.println ("Message was sent. OK");
					}
					else {                                    // Если нет, нужно повторить отправку
						ComPort.println ("Message was not sent. Error");
					}
			    }
			}
		} while (index > -1);                       // Пока индекс переноса строки действителен
	}





	if (ComPort.available())  {                    // Ожидаем команды по ComPort...
		SIM800.write(ComPort.read());                // ...и отправляем полученную команду модему
	}
}

void checkAlarmSensorGeneralLoop()
{
	if(getMotoinState() > 0){}
	if(getRainStateForLoop() > 0){}
}

void setup()
{	
	// Sim800Port.println("ATE0V0+CMEE=1;&W");  // digital
	// Sim800Port.println("ATE1V1+CMEE=2;&W");  // text

	pinMode(RELE_PIN, OUTPUT);
	digitalWrite(RELE_PIN, LOW);

	pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT);
	pinMode(RAIN_SENSOR_VCC_PIN, OUTPUT);

	pinMode(MOTION_SENSOR_PIN, INPUT);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	ComPort.begin(9600);
	Sim800Port.begin(9600);
	
	dht.begin();

	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN, HX711_GAIN);
	delay(100);
	hx711Obj.set_gain(HX711_GAIN);

	gsmModemStart();

	// outgoingCall(reportMasterPhones);

	String testMsgStr = "test msg";
	sendSMS(reportMasterPhones, testMsgStr);
}

void loop()
{
	// trx_Gsm_Port();  // trx
	checkRINGgeneralLoop();  // check GSM uart and control
	checkAlarmSensorGeneralLoop();  // check alarm sensor
}