#include <SoftwareSerial.h>                     // Библиотека програмной реализации обмена по UART-протоколу
SoftwareSerial SIM800(3, 2);                    // RX, TX

String _response = "";                          // Переменная для хранения ответа модуля
void setup() {
  Serial.begin(9600);                           // Скорость обмена данными с компьютером
  SIM800.begin(9600);                           // Скорость обмена данными с модемом
  Serial.println("Start! PS");

  sendATCommand("AT", true);                    // Отправили AT для настройки скорости обмена данными

  // Команды настройки модема при каждом запуске
  //_response = sendATCommand("AT+CLIP=1", true);  // Включаем АОН
  //_response = sendATCommand("AT+DDET=1", true);  // Включаем DTMF
  _response = sendATCommand("AT+CMGF=1;&W", true); // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
  sendATCommand("AT+CMGDA=\"DEL ALL\"", true);      // Удалить все сообщения, чтобы не забивали память модуля (txt mode)
  sendSMS("+38xxxxxxxxxx", "modem ready");

}

String sendATCommand(String cmd, bool waiting) {
  String _resp = "";                            // Переменная для хранения результата
  Serial.println(cmd);                          // Дублируем команду в монитор порта
  SIM800.println(cmd);                          // Отправляем команду модулю
  if (waiting) {                                // Если необходимо дождаться ответа...
    _resp = waitResponse();                     // ... ждем, когда будет передан ответ
    // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
    if (_resp.startsWith(cmd)) {                // Убираем из ответа дублирующуюся команду
      _resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
    }
    Serial.println(_resp);                      // Дублируем ответ в монитор порта
  }
  return _resp;                                 // Возвращаем результат. Пусто, если проблема
}

String waitResponse() {                         // Функция ожидания ответа и возврата полученного результата
  String _resp = "";                            // Переменная для хранения результата
  long _timeout = millis() + 10000;             // Переменная для отслеживания таймаута (10 секунд)
  while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
  if (SIM800.available()) {                     // Если есть, что считывать...
    _resp = SIM800.readString();                // ... считываем и запоминаем
  }
  else {                                        // Если пришел таймаут, то...
    Serial.println("Timeout...");               // ... оповещаем об этом и...
  }
  return _resp;                                 // ... возвращаем результат. Пусто, если проблема
}

void loop() {
  if (SIM800.available())   {                   // Если модем, что-то отправил...
    _response = waitResponse();                 // Получаем ответ от модема для анализа
    _response.trim();                           // Убираем лишние пробелы в начале и конце
    Serial.println(_response);                  // Если нужно выводим в монитор порта
    //....
    if (_response.startsWith("+CMTI:")) {       // Пришло сообщение об отправке SMS
      int index = _response.lastIndexOf(",");   // Находим последнюю запятую, перед индексом
      String result = _response.substring(index + 1, _response.length()); // Получаем индекс
      result.trim();                            // Убираем пробельные символы в начале/конце
      _response=sendATCommand("AT+CMGR="+result, true); // Получить содержимое SMS
      parseSMS(_response);                      // Распарсить SMS на элементы
      sendATCommand("AT+CMGDA=\"DEL ALL\"", true); // Удалить все сообщения, чтобы не забивали память модуля
    }
  }
  if (Serial.available())  {                    // Ожидаем команды по Serial...
    SIM800.write(Serial.read());                // ...и отправляем полученную команду модему
  };
}


void parseSMS(String msg) {
  String msgheader  = "";
  String msgbody    = "";
  String msgphone    = "";

  msg = msg.substring(msg.indexOf("+CMGR: "));
  msgheader = msg.substring(0, msg.indexOf("\r"));

  msgbody = msg.substring(msgheader.length() + 2);
  msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK"));
  msgbody.trim();

  int firstIndex = msgheader.indexOf("\",\"") + 3;
  int secondIndex = msgheader.indexOf("\",\"", firstIndex);
  msgphone = msgheader.substring(firstIndex, secondIndex);

  Serial.println("Phone: "+msgphone);
  Serial.println("Message: "+msgbody);

  // Далее пишем логику обработки SMS-команд.
  // Здесь также можно реализовывать проверку по номеру телефона
  // И если номер некорректный, то просто удалить сообщение.
}


void sendSMS(String phone, String message)
{
  sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
  sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
}