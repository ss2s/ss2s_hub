//  РАСПИНОВКА:
#define RESET_MODEM_PIN 9  // RESET sim800
#define RELE_PIN 8  // РЕЛЕ
#define RAIN_SENSOR_VCC_PIN 7  // ДАТЧИК ДОЖДЯ VCC
#define RAIN_SENSOR_DIGITAL_PIN 6  // ДАТЧИК ДОЖДЯ ЦИФРОВОЙ ВХОД
#define MOTION_SENSOR_PIN 5  // ДАТЧИК ДВИЖЕНИЯ
#define DHT_DATA_PIN 4 // ДАТЧИК ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ DHT 21
#define HX711_DOUT_PIN A2  // ВЕСЫ DT
#define HX711_PD_SCK_PIN A3  // ВЕСЫ SCK
#define SOFT_RX 10  // SoftSerial RX
#define SOFT_TX 11  // SoftSerial TX

// НАСТРОЙКИ:

// кнопки телефона
#define BUTTON_RELE_ON "1"
#define BUTTON_RELE_OFF "2"
#define BUTTON_RELE_STATE "3"
#define BUTTON_ALARM_ON "4"
#define BUTTON_ALARM_OFF "5"
#define BUTTON_ALARM_STATE "6"
#define BUTTON_ALL_STATE "9"
#define BUTTON_GET_SMS "0"

// номера телефонов с которых будет принимать звонок
String whiteListPhones = "+380xxxxxxxxx, +380xxxxxxxxx, +380xxxxxxxxx"; // Белый список телефонов
String alarmMasterPhones = "+380xxxxxxxxx"; // мастер телефон для сигнализации

// задержки
uint32_t timeOutMotionDetector = 1000;  // 1s
uint32_t timeOutMotionAlarm = 60000;  // 60s

uint32_t timeOutRainDetector = 1200000;  // 20m
uint32_t timeOutRainAlarm = 86400000;  // 24h


float calibration_factor = -0.804;          // калибровка весов!
float minus_tare_factor = 6065.00;          // тара весов грамм!

#define RELE_HIGH 0
#define RELE_LOW 1

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "HX711.h"
#include "TroykaDHT.h"
#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(SOFT_RX, SOFT_TX); // RX, TX

uint32_t timeOutTempCheck = 1000*60*10;  // 10m  to future

// переменные весов
float units;
float ounces;

// flags
bool motionDetectAlarmEn = 1;

bool rainAlarmEn = 1;
bool rainDetectEn = 1;
// variables
String _response = "R";  // Переменная для хранения ответа модуля
String messageStr = "";  // message string
String lastPhoneNumber = alarmMasterPhones;  // последний номер телефона для ответного смс с показаниями
uint32_t strtTimeoutRainPwr = 0;
uint32_t strtTimeoutRainDetect = 0;
uint32_t strtTimeoutRainAlarm = 0;

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

// #define HX711_GAIN 64  // усилитель 64 chanel A
// HX711 hx711Obj;

HX711 scale(HX711_DOUT_PIN, HX711_PD_SCK_PIN);

bool VAL_MOTION_DETECT = 0;
bool VAL_INNER_MOTION_DETECT = 0;

bool VAL_RAIN_DETECT = 0;
bool VAL_RAIN_GENERAL_FLAG = 0;

uint32_t VAL_WEIGHT = 0;

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
	scale.power_up();
	delay(500);
	ComPort.print("Weight: ");
	units = scale.get_units(10);  //..................?
	ounces = units * 0.035274;    // переводим унции в граммы
	ounces -= minus_tare_factor;            // отнимаем тару
	ComPort.print(ounces);         // отправляем в монитор порта
	ComPort.print(" grams");  
	ComPort.println();
	VAL_WEIGHT = int(ounces);
	scale.power_down();
	return VAL_WEIGHT;
}

bool getMotoinState()
{
	VAL_INNER_MOTION_DETECT = digitalRead(MOTION_SENSOR_PIN);
	return VAL_INNER_MOTION_DETECT;
}

bool getRainState()
{
	digitalWrite(RAIN_SENSOR_VCC_PIN, RELE_HIGH);
	delay(500);
	VAL_RAIN_DETECT = digitalRead(RAIN_SENSOR_DIGITAL_PIN);
	digitalWrite(RAIN_SENSOR_VCC_PIN, RELE_LOW);
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

void outgoingCall (String phNum)
{
	// phNum = alarmMasterPhones;
	String vizov = "ATD" + phNum + ";";
	_response = sendATCommand(vizov, true);
}

void sendSMS(String phone, String message)
{
	sendATCommand("AT+VTS=\"9,0\"", true);              // Переходим в режим ввода текстового сообщения
	delay(700);
	sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
	sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
// void outgoingCall (String phNum)
// {  // сервисная функция исходящий вызов
// 	// phNum = masterPhones;
// 	String _call = "ATD" + phNum + ";";
// 	_response = sendATCommand(_call, true);
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void sendSMS(String phone, String message)
// {  // сервисная функция отправки СМС
// 	sendATCommand("AT+CMGF=1", true);  // Включаем текстовый режима SMS (Text mode)
// 	sendATCommand("AT+VTS=\"9,0\"", true);              // Переходим в режим ввода текстового сообщения
// 	delay(700);
// 	sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
// 	sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gsmModemStart()
{
	ComPort.println("");
	ComPort.print("sys: ");
	ComPort.println("Start system");

	// sendATCommand("AT", true);               // Автонастройка скорости
	// sendATCommand("ATV1", true);                // Автонастройка скорости
	uint32_t blinkSetupTimeStart = millis();
	do {
    	_response = sendATCommand("AT", true);  // проверка модема
    	_response.trim();                       // Убираем пробельные символы в начале и конце

    	if(millis()-blinkSetupTimeStart >= 500){
    		digitalWrite(13, !digitalRead(13));
    		blinkSetupTimeStart = millis();
    	}
	} while (_response != "OK");                // Не пускать дальше, пока модем не вернет ОК
	digitalWrite(13, LOW);
	ComPort.println("sys: GSM modem is ready!");

	// Команды настройки модема при каждом запуске
	_response = sendATCommand("AT+CLIP=1", true);  // Включаем АОН
	_response = sendATCommand("AT+DDET=1", true);  // Включаем DTMF
	_response = sendATCommand("AT+CMGF=1;&W", true); // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
	// _response = sendATCommand("AT+CMGF=1", true);         // Включить TextMode для SMS
	// sendATCommand("AT+CLVL?", true);            // Запрашиваем громкость динамика

	do {
    	_response = sendATCommand("AT+CLIP=1", true);  // Включаем АОН
    	_response.trim();                          // Убираем пробельные символы в начале и конце
	} while (_response != "OK");                   // Не пускать дальше, пока модем не вернет ОК

	ComPort.println("Opredelenie nomera VKL");     // Информируем, что АОН включен
	// Проверка пройдена, модем сообщил о готовности, можно запускать основной цикл...

	sendATCommand("AT+CMGDA=\"DEL ALL\"", true); // Удалить все сообщения, чтобы не забивали память модуля

	sendATCommand("AT+VTD=3", true); // установить длительность DTMF генератора
}

void smsReportF(bool nTipe = 1){
	getDhtDataInt();
	String smsText = "";

	if(nTipe == 0){
		smsText += "          RAIN !!!";
		smsText += "\n\nTemperature: ";
	}
	else{
		smsText += "Temperature: ";
	}

	smsText += String(VAL_TEMP);
	smsText += " gr C";

	smsText += "\nHumidity: ";
	smsText += String(VAL_HMDT);
	smsText += " %";

	smsText += "\nWeight: ";
	smsText += String(getWeight());
	smsText += " gramm";

	smsText += "\nRain: ";
	if(getRainState() == 0){smsText += " RAIN :(";}
	else{smsText += " SUN :)";}

	smsText += "\nRele: ";
	if(digitalRead(RELE_PIN) == RELE_HIGH){smsText += " ON";}
	else{smsText += " OFF";}

	smsText += "\nAlarm: ";
	if(motionDetectAlarmEn == 1){smsText += " ON";}
	else{smsText += " OFF";}



	if(nTipe == 0){sendSMS(alarmMasterPhones, smsText);}
	else{sendSMS(lastPhoneNumber, smsText);}
}

void processingDTMF(String symbol) {
	ComPort.println("Key: " + symbol);             // Выводим в ComPort для контроля, что ничего не потерялось
	if(symbol == BUTTON_RELE_ON){
		digitalWrite(RELE_PIN, RELE_HIGH);
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
	}
	else if(symbol == BUTTON_RELE_OFF){
		digitalWrite(RELE_PIN, RELE_LOW);
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
	}
	else if(symbol == BUTTON_RELE_STATE){
		if(digitalRead(RELE_PIN) == RELE_HIGH){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
		}
		else{
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
		}
	}
	else if(symbol == BUTTON_ALARM_ON){
		motionDetectAlarmEn = 1;
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
	}
	else if(symbol == BUTTON_ALARM_OFF){
		motionDetectAlarmEn = 0;
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
	}
	else if(symbol == BUTTON_ALARM_STATE){
		if(motionDetectAlarmEn == 1){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
		}
		else{
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
		}
	}
	else if(symbol == BUTTON_ALL_STATE){
		SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(14), SIM800.println(".amr\",0,100");
		delay(4000);
		bool _rs = digitalRead(RELE_PIN);
		if(_rs == RELE_HIGH && motionDetectAlarmEn == 1){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(9), SIM800.println(".amr\",0,100");
		}
		else if(_rs == RELE_LOW && motionDetectAlarmEn == 0){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(10), SIM800.println(".amr\",0,100");
		}
		else if(_rs == RELE_HIGH && motionDetectAlarmEn == 0){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(1), SIM800.println(".amr\",0,100");
			delay(4000);
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(6), SIM800.println(".amr\",0,100");
		}
		else if(_rs == RELE_LOW && motionDetectAlarmEn == 1){
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(5), SIM800.println(".amr\",0,100");
			delay(4000);
			SIM800.print("AT+CREC=4,\"C:\\User\\"), SIM800.print(2), SIM800.println(".amr\",0,100");
		}
	}
	else if(symbol == BUTTON_GET_SMS){
		smsReportF(1);
	}
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

uint32_t strtTimeoutMotionDetect = 0;
uint32_t strtTimeoutMotionAlarm = 0;
void checkAlarmSensorGeneralLoop()
{
	if(getMotoinState() > 0){
		VAL_MOTION_DETECT = 1;
	}
	if(motionDetectAlarmEn == 0){
		VAL_MOTION_DETECT = 0;
	}

	if((millis() - strtTimeoutMotionDetect >= timeOutMotionDetector) && (VAL_MOTION_DETECT == 1) && (getMotoinState() > 0)){
		strtTimeoutMotionDetect = millis();
		ComPort.println("motion detected !!!");
	}
	if((millis() - strtTimeoutMotionAlarm >= timeOutMotionAlarm) && (VAL_MOTION_DETECT == 1)){
		strtTimeoutMotionAlarm = millis();
		VAL_MOTION_DETECT = 0;
		outgoingCall(alarmMasterPhones);
	}





	if(millis() - strtTimeoutRainDetect >= timeOutRainDetector){
		if(getRainState() == 0){
			ComPort.println("rain detected !!!");
			if(rainAlarmEn == 1){
				rainAlarmEn = 0;
				smsReportF(0);
			}
			// strtTimeoutRainDetect = millis();
		}
		strtTimeoutRainDetect = millis();
	}
	if(millis() - strtTimeoutRainAlarm >= timeOutRainAlarm){
		strtTimeoutRainAlarm = millis();
		rainAlarmEn = 1;
	}
}

void setup()
{	
	// Sim800Port.println("ATE0V0+CMEE=1;&W");  // digital
	// Sim800Port.println("ATE1V1+CMEE=2;&W");  // text

	pinMode(RELE_PIN, OUTPUT);
	digitalWrite(RELE_PIN, RELE_LOW);

	pinMode(RAIN_SENSOR_DIGITAL_PIN, INPUT_PULLUP);
	pinMode(RAIN_SENSOR_VCC_PIN, OUTPUT);
	digitalWrite(RAIN_SENSOR_VCC_PIN, RELE_LOW);

	pinMode(MOTION_SENSOR_PIN, INPUT);

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	ComPort.begin(9600);
	Sim800Port.begin(9600);

	scale.set_scale();
	// scale.tare();                              //Сбрасываем на 0
	scale.set_scale(calibration_factor);       //Применяем калибровку
	
	dht.begin();

	delay(100);

	gsmModemStart();
	scale.power_down();

	// outgoingCall(alarmMasterPhones);

	String testMsgStr = "MicroController restart"; sendSMS(alarmMasterPhones, testMsgStr);
}

void loop()
{
	// trx_Gsm_Port();  // trx
	checkRINGgeneralLoop();  // check GSM uart and control
	checkAlarmSensorGeneralLoop();  // check alarm sensor
}