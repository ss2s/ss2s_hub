/* контроллер температуры теплицы
открывает закрывает окно
есть два режима авто и ручной переключается джампером
в ручном режиме откр. закр. кнопкой ресет */
//#include "Arduino.h"
#include "Dht11.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

// НАСТРОЙКИ ТЕРМОМЕТРА
byte OPEN_TEMP = 26; // температура открытия
byte CLOSE_TEMP = 20; // температура закрытия

// ПРОЧИЕ НАСТРОЙКИ
int MOTOR_PWM = 255; // скорость мотора
unsigned long MOTOR_TIMER = 4000; // таймер мотора в мс
byte MIN_LCD_PWM = 2; // минимальная яркость дисплея

// РАСПИНОВКА
byte WTC_MODE_PIN = 4; // режим работы +А -М
byte DHT_DATA_PIN = 5; // датчик температуры
byte LIGHT_SENSOR_PIN = A0; // датчик освещения
byte LCD_PWM_PIN = 10;  // ШИМ яркость подсветки
byte MOTOR_PWM_PIN = 9; // ШИМ мотора
byte MOTOR_RED_1_PIN = 8; // выход мотора 1 желтый
byte MOTOR_GRN_2_PIN = 7; // выход мотора 2 черный
byte LIMIT_SWITCH_OPN_PIN = 2; // концевик открытия оранжевый
byte LIMIT_SWITCH_CLS_PIN = 3; // концевик закрытия желтый

// ПЕРЕМЕННЫЕ
unsigned long startTime; // старт таймер концевиков
unsigned long stopTime; // стоп таймер концевиков
int timerWatchDog = 0; // таймер собаки
int timer = 0; // общий таймер
short lightSensor = 0; // освещение 0-1023
byte lcdPWM = 255; // яркость подсветки (при включении дальше авто)
byte TEMP = 0; // температура
byte HMDT = 0; // влажность
boolean WTC_MODE; // режим работы +А -М
boolean OPN; // значение концевика открытия
boolean CLS; // значение концевика закрытия
boolean OPN_CLS_CHK = false; // проверка концевик или таймер

LiquidCrystal_I2C lcd(0x27, 16, 2); // установка адреса 0x27 и разрешения 16/2

void winAUTO(){  // автоматический режим AUTO
  wdt_reset(); // сбросить собаку
  if(TEMP >= OPEN_TEMP){ // если темп.откр >= текущей температуры
    winOPN(); // открыть окно
  } // или
  else if(TEMP <= CLOSE_TEMP){ // если темп.закр <= текущей температуры
    winCLS(); // закрыть окно
  }
  
}

void winMANUAL(){  // ручной режим MANUAL
  wdt_reset(); // сброс собаки
  byte result; // переменная для проверки откр. закр.
  result = winCHK(); // проверка откр. закр.
  if(result == 0){ // если открыто
    winCLS(); // закрыть окно
  }
  else if(result == 1){ // или если закрыто
    winOPN(); // открыть окно
  }else{ // или
    lcd.clear(); // очистить дисплей
    lcd.print("winCHK???"); // состояние окна неизвестно
    delay(1000); // ждать 1с
    lcd.clear(); // очистить дисплей
    winCLS(); // закрыть окно
  }
  
}

byte winCHK(){  // проверка окр. закр.
  wdt_reset(); // сброс собаки
  boolean winCHKOPN; // концевик откр.
  boolean winCHKCLS; // концевик закр.
  pinMode(LIMIT_SWITCH_OPN_PIN,INPUT_PULLUP); // включить подтяжку
  pinMode(LIMIT_SWITCH_CLS_PIN,INPUT_PULLUP); // включить подтяжку
  delay(100); // ждать 0,1с
  winCHKOPN = digitalRead(LIMIT_SWITCH_OPN_PIN); // проверка концевиков откр.
  winCHKCLS = digitalRead(LIMIT_SWITCH_CLS_PIN); // проверка концевиков закр.
  pinMode(LIMIT_SWITCH_OPN_PIN,INPUT); // отключить подтяжку
  pinMode(LIMIT_SWITCH_CLS_PIN,INPUT); // отключить подтяжку
  if(winCHKOPN == LOW){ // если открыто вернуть 0
    return 0;
  }
  else if(winCHKCLS == LOW){ // или если закрыто вернуть 1
    return 1;
  }else{ // или вернуть 2
    return 2;
  }
  
}

void winOPN(){  // открытие окна
  wdt_reset(); // сбросить собаку
  pinMode(LIMIT_SWITCH_OPN_PIN,INPUT_PULLUP); // включить подтяжку
  startTime = millis(); // записать старт таймер
  OPN = digitalRead(LIMIT_SWITCH_OPN_PIN); //считать значение концевика откр.
  if(OPN == HIGH){ // если не открыто
    lcd.clear(); // очистить дисплей
    lcd.print("OPEN"); // написать "OPEN"
  }
  
  digitalWrite(MOTOR_RED_1_PIN,LOW);
  digitalWrite(MOTOR_GRN_2_PIN,HIGH);
  //analogWrite(MOTOR_PWM_PIN,MOTOR_PWM);
  digitalWrite(MOTOR_PWM_PIN,HIGH);
  
  while(OPN == HIGH){ // если не открыто
    wdt_reset(); // сбросить собаку
    
    delay(50);
    OPN = digitalRead(LIMIT_SWITCH_OPN_PIN);
    stopTime = millis();
    if((stopTime - startTime) >= MOTOR_TIMER){
      OPN = LOW;
    }
    
  }
  
  digitalWrite(MOTOR_PWM_PIN,LOW);
  wdt_reset();
  // функция проверки концевик или таймер
  OPN_CLS_CHK = digitalRead(LIMIT_SWITCH_OPN_PIN);
  if(OPN_CLS_CHK == OPN){
    OPN_CLS_CHK = true;
  }else{
    OPN_CLS_CHK = false;
  }

  pinMode(LIMIT_SWITCH_OPN_PIN,INPUT);
  lcd.clear(); // очистить дисплей
  
}

void winCLS(){  // закрытие окна
  wdt_reset();
  pinMode(LIMIT_SWITCH_CLS_PIN,INPUT_PULLUP);
  startTime = millis();
  CLS = digitalRead(LIMIT_SWITCH_CLS_PIN);
  if(CLS == HIGH){
    lcd.clear(); // очистить дисплей
    lcd.print("CLOSE");
  }
  
  digitalWrite(MOTOR_RED_1_PIN,HIGH);
  digitalWrite(MOTOR_GRN_2_PIN,LOW);
  //analogWrite(MOTOR_PWM_PIN,MOTOR_PWM);
  digitalWrite(MOTOR_PWM_PIN,HIGH);
  
  while(CLS == HIGH){
    wdt_reset();
    
    delay(50);
    CLS = digitalRead(LIMIT_SWITCH_CLS_PIN);
    stopTime = millis();
    if((stopTime - startTime) >= MOTOR_TIMER){
      CLS = LOW;
    }
    
  }
  
  digitalWrite(MOTOR_PWM_PIN,LOW);
  wdt_reset();
  
  OPN_CLS_CHK = digitalRead(LIMIT_SWITCH_CLS_PIN);
  if(OPN_CLS_CHK == CLS){
    OPN_CLS_CHK = true;
  }else{
    OPN_CLS_CHK = false;
  }
 
  pinMode(LIMIT_SWITCH_CLS_PIN,INPUT);
  lcd.clear(); // очистить дисплей
  
}

void setup() {    // сэтап
   wdt_disable(); // выключить собаку
   
  WTC_MODE = digitalRead(WTC_MODE_PIN); // считать значение режима работы А М
  
  wdt_enable (WDTO_8S); // включить собаку 8с
  
  lcd.begin(); // иниализация дисплея LCD 16/2
  lcd.clear(); // очистить дисплей
  analogWrite(LCD_PWM_PIN, lcdPWM); // яркость дисплея
  lcd.backlight(); // включение подсветки
  lcd.setCursor(0,0);
  lcd.print("  GHWTC v1.0.1  ");
  lcd.setCursor(0,1);
  lcd.print("   by    SS2S");
  wdt_reset();
  delay(1000);
  wdt_reset();
  lcd.clear(); // очистить дисплей

  if(WTC_MODE == HIGH){
    lcd.print("AUTO");
    
  }else{
    lcd.print("MANUAL");
    
  }
  
  delay(1000);
  wdt_reset();
  lcd.clear(); // очистить дисплей

  

  pinMode(LIMIT_SWITCH_OPN_PIN,INPUT);
  pinMode(LIMIT_SWITCH_CLS_PIN,INPUT);
  pinMode(LCD_PWM_PIN,OUTPUT);
  pinMode(MOTOR_PWM_PIN,OUTPUT);
  pinMode(MOTOR_RED_1_PIN,OUTPUT);
  pinMode(MOTOR_GRN_2_PIN,OUTPUT);
  //pinMode(13,OUTPUT);
  wdt_reset();
  
  digitalWrite(MOTOR_PWM_PIN,LOW);
  digitalWrite(MOTOR_RED_1_PIN,HIGH);
  digitalWrite(MOTOR_GRN_2_PIN,LOW);
  
  if(WTC_MODE == LOW){
    winMANUAL();
  }

}

void loop() {     // луп
  wdt_reset();

  static Dht11 sensor(DHT_DATA_PIN);

  switch (sensor.read()) {
    case Dht11::OK:
        wdt_reset();
        //lcd.clear(); // очистить дисплей
        TEMP = sensor.getTemperature();
        HMDT = sensor.getHumidity();
        lcd.setCursor(0,0);
        lcd.print("Temp: ");
        lcd.print(TEMP);
        lcd.setCursor(9,0);
        lcd.print("C");
        lcd.setCursor(0,1);
        lcd.print("Hmdt: ");
        lcd.print(HMDT);
        lcd.setCursor(9,1);
        lcd.print("%");       
        break;

    case Dht11::ERROR_CHECKSUM:
        wdt_reset();       
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Checksum error");
        timerWatchDog ++;
        lcd.setCursor(12,0);
        lcd.print(timerWatchDog);
        digitalWrite(13, digitalRead(13)==1?0:1);  
        delay(1000);
        break;

    case Dht11::ERROR_TIMEOUT:
        wdt_reset();
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Timeout error");
        timerWatchDog ++;
        lcd.setCursor(12,0);
        lcd.print(timerWatchDog);
        digitalWrite(13, digitalRead(13)==1?0:1);  
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;

    default:
        wdt_reset();
        lcd.clear(); // очистить дисплей
        lcd.setCursor(0,0);
        lcd.println("Unknown error");
        timerWatchDog ++;
        lcd.setCursor(12,0);
        lcd.print(timerWatchDog);
        digitalWrite(13, digitalRead(13)==1?0:1);  
        delay(1000);
        lcd.clear(); // очистить дисплей
        break;
    }
    wdt_reset();
    
    lightSensor = analogRead(LIGHT_SENSOR_PIN); // прочитать значение освещенности
    lcdPWM = lightSensor / 4; // мап значения сенсора в ШИМ
    if(lcdPWM < MIN_LCD_PWM){ // минимальный уровень подсветки
      lcdPWM = MIN_LCD_PWM;
    }
    analogWrite(LCD_PWM_PIN, lcdPWM);
    lcd.setCursor(15,1);
    lcd.print(lightSensor / 103); // уровень подсветки дисплея 0-9
    
    wdt_reset();
    
    if(WTC_MODE == HIGH){
      lcd.setCursor(12,1);
      lcd.print('A');
      winAUTO();
    }else{
      lcd.setCursor(12,1);
      lcd.print('M');
    }

    if(OPN_CLS_CHK == true){
      lcd.setCursor(13,1);
      lcd.print('K');
    }else{
      lcd.setCursor(13,1);
      lcd.print('T');
    }

    wdt_reset();
    
    timerWatchDog ++;
    lcd.setCursor(12,0);
    lcd.print(timerWatchDog);
    digitalWrite(13, digitalRead(13)==1?0:1);  
    delay(1000);

    wdt_reset();

    /*timerWatchDog ++;
    lcd.setCursor(12,0);
    lcd.print(timerWatchDog);
    digitalWrite(13, digitalRead(13)==1?0:1);  
    delay(1000);
    
    wdt_reset();*/
     
    if(timerWatchDog >= 9999){
      timerWatchDog = 0;
    }
    

}










