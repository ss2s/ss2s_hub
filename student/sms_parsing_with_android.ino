#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 7    // pin на котором висит датчик температуры
#define DHTTYPE DHT22  // DHT 22 тип датчика
//Отсылается смс в формате: влажность;температура;устройство1;устройство2;устройство3;устройство4;Контроль периметра;Состояние периметра;Контроль автотемпературы;Установленная температура;STEWARD;
int greenLed = 13; //Зеленый светодиод на PIN13
int powerGSM = 6; //Пин включения GSM
int unit1 = 8; //Управляемое устройство 1
int unit2 = 9; //Управляемое устройство 2
int unit3 = 10;//Управляемое устройство 3
int unit4 = 11;//Управляемое устройство 4
int perimetr = 12;// Датчик периметра
String statusHome;
int t=0;
 
unsigned long currentTime;
unsigned long loopTime;
 
boolean unit1status = false; //Управляемое устройство 1
boolean unit2status = false; //Управляемое устройство 2
boolean unit3status = false;//Управляемое устройство 3
boolean unit4status = false;//Управляемое устройство 4
 
boolean isPerimetr = false; //Контроль периметра
boolean sendSMS = false; //Для проверки что смс отсылаем только один раз
boolean isAuto = false; //Автоматическое управление температурой
int autoTemp = 0;
int delta=2;
 
DHT dht(DHTPIN, DHTTYPE);
 
SoftwareSerial gprsSerial(2, 3);
 
boolean prevBtn = LOW;
String currStr = "";
// Переменная принимает значение True, если текущая строка является сообщением
boolean isStringMessage = false;
 
void setup()
{
    pinMode(unit1, OUTPUT);
    pinMode(unit2, OUTPUT);
    pinMode(unit3, OUTPUT);
    pinMode(unit4, OUTPUT);
    pinMode(powerGSM, OUTPUT);
    pinMode(perimetr, INPUT);
    pinMode(greenLed, OUTPUT);
 
    digitalWrite(unit1, LOW);
    digitalWrite(unit2, LOW);
    digitalWrite(unit3, LOW);
    digitalWrite(unit4, LOW);
    digitalWrite(powerGSM, LOW);
    digitalWrite(greenLed, LOW);
 
    //Включаю GSM Модуль
    digitalWrite(powerGSM, HIGH);
    delay(1000);
    digitalWrite(powerGSM, LOW);
    delay(5000);
    gprsSerial.begin(19200);
    delay(300);
    // Настраиваем приём сообщений с других устройств
    // Между командами даём время на их обработку
    gprsSerial.print("AT+CMGF=1\r");
    delay(300);
    gprsSerial.print("AT+IFC=1, 1\r");
    delay(300);
    gprsSerial.print("AT+CPBS=\"SM\"\r");
    delay(300);
    gprsSerial.print("AT+CNMI=1,2,2,1,0\r");
    delay(500);
 
    //Инициализация температурного датчика
    Serial.begin(9600);
    Serial.println("DHTxx test!");
 
    dht.begin();
    delay(3000);
// ParseSMS("1;20;0;1;1;1;1");
  currentTime = millis();
  loopTime = currentTime;
}
 
 
 
void GetDatchik() {
  // Пол
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    statusHome = int(h)+String(";")+int(t)+String(";")+int(unit1status)+String(";")+int(unit2status)+String(";")+int(unit3status)+String(";")+int(unit4status)+String(";")+int(isPerimetr)+String(";")+digitalRead(perimetr)+String(";")+int(isAuto)+String(";")+int(autoTemp)+String(";STEWARD");
    sendTextMessage(statusHome);
//  Serial.print("H"); //Влажность
//  Serial.print(h);
//  Serial.print(";");
//  Serial.print("T"); //Температура
//  Serial.print(t);
//  Serial.println(";");
//  Serial.println(statusHome);
 
  }
}
 
 
 
 
 
 
void loop()
{
// delay(5000);
// GetDatchik();
int sostPerimetr; //Состояние периметра
  if (gprsSerial.available()){
  char currSymb = gprsSerial.read();
// Serial.print(currSymb);
 
      //  return;
  //  char currSymb = gprsSerial.read();
  //  Serial.println(currSymb);
    if ('\r' == currSymb) {
        if (isStringMessage) {
            //если текущая строка - SMS-сообщение,
            //отреагируем на него соответствующим образом
            if (!currStr.compareTo("STATUS"))
            {
              GetDatchik();
            }
            else if (currStr.startsWith("0")||currStr.startsWith("1")) {
              ParseSMS(currStr);
            }
         
            isStringMessage = false;
        } else {
            if (currStr.startsWith("+CMT")) {
                //если текущая строка начинается с "+CMT",
                //то следующая строка является сообщением
                isStringMessage = true;
            }
        }
        currStr = "";
    } else if ('\n' != currSymb) {
        currStr += String(currSymb);
    }
  }
//  Контроль периметра!!!
  if (isPerimetr) { //Проверка Включен ли контроль периметра
      digitalWrite(greenLed, HIGH);
      sostPerimetr = digitalRead(perimetr);
        if (sostPerimetr==LOW) { //Проверка на разомкнутый периметр
          digitalWrite(greenLed, LOW);
          delay(200);
          digitalWrite(greenLed, HIGH);
          delay(200);
          //и отсылаем СМС
          if (sendSMS==false){
            sendTextMessage("WARNING! The perimeter of the broken!");
            sendSMS=true;
          }
      }
 
  } else {
    digitalWrite(greenLed, LOW);
  }
//  Контроль температуры!!! 
  if (isAuto==true) { //Проверка Включен автоуправление батареями
  //Замер температуры можно проводить не чаще раз в 5 минут
      currentTime = millis();
  if(currentTime >= (loopTime + 60000)){
  t = int(dht.readTemperature());
  loopTime = currentTime;
  }
//
//  Serial.print("t=");
//  Serial.println(t);
//  Serial.print("autoTemp+delta=");
//  Serial.println(autoTemp+delta);
    if (t > autoTemp+delta) {
        if ( unit1status==true){
        digitalWrite(unit1, LOW);
        unit1status=false;
        }
        if ( unit2status==true){
        digitalWrite(unit2, LOW);
        unit2status=false;
        }
        if ( unit3status==true){
        digitalWrite(unit3, LOW);
        unit3status=false;
        }
        if ( unit4status==true){
        digitalWrite(unit4, LOW);
        unit4status=false;
        }
        } else if (t < autoTemp-delta) {
        if ( unit1status==false){
        digitalWrite(unit1, HIGH);
        unit1status=true;
        }
        if ( unit2status==false){
        digitalWrite(unit2, HIGH);
        unit2status=true;
        }
        if ( unit3status==false){
        digitalWrite(unit3, HIGH);
        unit3status=true;
        }
        if ( unit4status==false){
        digitalWrite(unit4, HIGH);
        unit4status=true;
        }
        }
 
  }
}
 
 
 
 
void ParseSMS(String currStr) {
  int firstSemicolon;
  //Распарсиваем SMS
  firstSemicolon = 1;//находим первую ;
  //Получаем значение Автоматического режима контроля температуры
  // if (currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0"){
    if (currStr.substring(0,1) == "0"){
    isAuto=false;
  }
    else{
      isAuto=true;
    }
  //  Serial.println(currStr.substring(0,1));
//  Serial.print("isAuto=");
//  Serial.println(isAuto);
  firstSemicolon = currStr.indexOf(';');
  //Получаем значение Автоматического уровня температуры
  autoTemp = stringToNumber(currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )));
  firstSemicolon = currStr.indexOf(';',firstSemicolon + 1 );
// Serial.println(autoTemp);
  //Получаем значение Контроля периметра
  if (currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0"){
    isPerimetr=false;
  } else {
      isPerimetr=true;
    }
//  Serial.println(isPerimetr);
  firstSemicolon = currStr.indexOf(';',firstSemicolon + 1 );
 
  //Получаем значение Устройства 1
if ((currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0")&&(isAuto==false)){
    unit1status=false;
    digitalWrite(unit1, LOW);
//  Serial.println("Устройство 1 ВЫКЛ");
  } else{
      unit1status=true;
      digitalWrite(unit1, HIGH);
  //    Serial.println("Устройство 1 ВКЛ");
    }
firstSemicolon = currStr.indexOf(';',firstSemicolon + 1 );
 
  //Получаем значение Устройства 2
if ((currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0")&&(isAuto==false)){
    unit2status=false;
    digitalWrite(unit2, LOW);
  } else {
      unit2status=true;
      digitalWrite(unit2, HIGH);
    }
  firstSemicolon = currStr.indexOf(';',firstSemicolon + 1 );
 
  //Получаем значение Устройства 3
if ((currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0")&&(isAuto==false)){
    unit3status=false;
    digitalWrite(unit3, LOW);
  } else {
      unit3status=true;
      digitalWrite(unit3, HIGH);
    }
  firstSemicolon = currStr.indexOf(';',firstSemicolon + 1 );
 
  //Получаем значение Устройства 4
if ((currStr.substring(firstSemicolon+1,currStr.indexOf(';', firstSemicolon + 1 )) == "0")&&(isAuto==false)){
    unit4status=false;
    digitalWrite(unit4, LOW);
  } else {
      unit4status=true;
      digitalWrite(unit4, HIGH);
    }
 
 
 
  }
 
/*
* Функция отправки SMS-сообщения
*/
void sendTextMessage(String text) {
    // Устанавливает текстовый режим для SMS-сообщений
    gprsSerial.print("AT+CMGF=1\r");
    delay(100); // даём время на усваивание команды
    // Устанавливаем адресата: телефонный номер в международном формате
    gprsSerial.println("AT + CMGS = \"+7909939XXXX\""); //Телефон нужно откорректировать
    delay(100);
    // Пишем текст сообщения
    gprsSerial.println(text);
    delay(100);
    // Отправляем Ctrl+Z, обозначая, что сообщение готово
    gprsSerial.println((char)26);
}
 
 
//Функция преобразовангия STRING в INT
int stringToNumber(String thisString) {
  int i, value = 0, length;
  length = thisString.length();
  for(i=0; i<length; i++) {
    value = (10*value) + thisString.charAt(i)-(int) '0';;
  }
  return value;
}