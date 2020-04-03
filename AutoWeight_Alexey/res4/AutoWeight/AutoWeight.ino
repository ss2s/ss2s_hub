
//////  #include  ///////////////
#include <EEPROM.h>                         //  PROGMEM Библиотека для записи данных во Flash память
#include <Wire.h>
#include <LiquidCrystal_I2C.h>              //  LCD
#include "Streaming.h"                      //  http://arduiniana.org/libraries/streaming/  Позволяет записывать
// #include "Adafruit_VL53L0X.h"               //  Лазерные дальномеры
#include "HX711.h"                          //  Тензодатчик https://arduinomaster.ru/datchiki-arduino/tenzodatchiki-i-vesy-na-arduino-i-nh711/


#include <SPI.h>

/////////////////////////////////////////////////////////////////////////
#define debug   1

int BT_VER = 2;
/////////////////////////////////////////////////////////////////////////
//  Инициализация LCD экрана
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Задаем адрес дисплея.
HX711 scale(A6, A7);              // DT, CLK Тензодатчик

/////////////////////////////////////////
// Настройки Bluetooth
String BTname = "AutoWeight";
String BTpin = "1111";

//////////////////////////////////////////

//////////////////////////////////////////


//  Создание спец символов. Экран.
//  Онлайн-конструктор https://maxpromer.github.io/LCD-Character-Creator/
#include "symbols.h"

//  Add-in
//  Ультразвуковые датчики
#include <NewPing.h>
#define MAX_DISTANCE 2500 // Константа для определения максимального расстояния, которое мы будем считать корректным.

//  Первый сонар
#define PIN_TRIG 25
#define PIN_ECHO 27

//  Второй сонар
#define PIN_TRIG_2 45//41
#define PIN_ECHO_2 47//43
//
//delay(50);
//  Третий сонар
#define PIN_TRIG_3 51//45
#define PIN_ECHO_3 49//47
//
/// ДЖОЙСТИК  ///////////////////////////////////
#define pinX    A1  // ось X джойстика         //
#define pinY    A2  // ось Y джойстика         //
#define swPin    2  // кнопка джойстика        //
#define JOYSTICK_MOVE 200 //Движение указателя //
/////////////////////////////////////////////////

// Создаем объект, методами которого будем затем пользоваться для получения расстояния.
// В качестве параметров передаем номера пинов, к которым подключены выходы ECHO и TRIG датчика

//  Округление до 0.5
double MyRound(const double x)
{
  const int mainPart = static_cast<int>(x);
  if (x < 0)
  {
    const bool toLowerBound = (0.75 <= mainPart - x);
    const bool toUpperBound = (mainPart - x < 0.25);
    return toUpperBound ? mainPart : toLowerBound ? mainPart - 1 : mainPart - 0.5;
  }
  else
  {
    const bool toLowerBound = (x - mainPart < 0.25);
    const bool toUpperBound = (0.75 <= (x - mainPart));
    return toUpperBound ? mainPart + 1 : toLowerBound ? mainPart : mainPart + 0.5;
  }
}


struct Sonar {
  unsigned int pingInt;
  float pingFloat;

};
struct Sonar sonya1;
struct Sonar sonya2;
struct Sonar sonya3;


/////////////////////////////////


//  Переменные для показаний дальномеров.
float FirstVx   = 0;
float SecondVx  = 0;
float ThirdVx   = 0;

//  Единица измерения длины: 1 мм, 10 см, 1000 м
int measure = 1;

volatile int menuPage = 1;            //  Страница меню   Главная "Меню Данных" - 1
volatile int subMenuPage = 0;         //  Субменю
volatile int prevpositionX = 0;
volatile int prevpositionY = 0;
volatile int prevMenuPage = menuPage;

boolean unpressed = true;             //  Нажатая кнопка джойстика

unsigned long currentMillis   = 200;
unsigned long previousMillis  = 0;    //  Переменная для хранения миллисекунд.
unsigned long PreviousMillisPing = 0;

volatile int LED = 13;
String command = "";
String tempString = "";
volatile int arrayOfSonarsValues[4] = {0};
volatile int menuSubPage = 0;


//  Класс джойстика
//  Чтение позиции X и Y
class Joystick  {
  public:

    int axisX;
    int axisY;
    bool keyPressed;

    void readAll() {
      axisX = 1023 - analogRead(pinX);
      axisY = 1023 - analogRead(pinY);
    };

    void readX() {
      axisX = analogRead(pinX);
      return axisX;
    };

    void readY() {
      axisY = 1023 - analogRead(pinY);
      return axisY;
    };

    void readKey() {
      keyPressed  = digitalRead(swPin);
      return keyPressed;
    };

    //X 514 Y 511
    void upOrDown (int& y, int minpos, int maxpos)  {   //  1 - Up, 0 - Center, -1 - Down

      //  311 калибровка
      if (axisY > 1000 && y > minpos) {
        y -= 1;     //  Up
        return y;
      } else if (axisY < 20 && y < maxpos)  {
        y += 1;    //  Down
        return y;
      } else { // if (axisY == 511)  {
        //  Center
        previousMillis = millis() - 50;
        return y;
      }
    }

    void leftOrRight (int& x)  {   //  1 - Right, 0 - Center, -1 - left
      if (axisX > 822 && x < 19) {
        x += 1;
        return x;     //  Right
      } else if (axisX < 200 && x > 0)  {
        x -= 1;
        return x;    //  Left
      } else if (axisX == 514)  {
        return x;    //  Center
      }
    }

};
//////////////////////////////////////////////////
//  Структура для управления кнопки выбора меню //
struct  TrackBallPosition  {
  int X;
  int Y;
};

//byte trackball = byte(0);
char trackball = '>';

struct TrackBallPosition trackBallPosition;         //  Новая переменная

Joystick joystick;                                  //  Обьект класса Joystick

//volatile int YY = 0;                                    //
volatile bool isKeyPressed = false;     //
//////////////////////////////////////////

//////////////////////////////////////////
//  Класс датчика дальномера

class Box {
  public:

    unsigned long Volume (float& Vl1, float& Vl2, float& Vl3) {
      return abs((length + Vl1) * (height + Vl2) * (width + Vl3) * 1000);
    }
    float   length = 0.0;
    float   height = 0.0;
    float   width = 0.0;

};

Box Box1;

//////////////////////////////////////////
//   Класс тензодатчика
//////////////////////////////////////////
class Tenzo {
  public:
    float scaleUnits = 0.00;
    float rationGrams = 0.03527;
    float weight = 0.00;
    float caliboration = 1.00;
    float tempCaliboration  = 1.00;
    const float defaultCaliboration = 12181;

    float inputWeight = 1;


    void getUnits ()  {
      scale.power_up();
      scaleUnits = scale.get_units(3);
      if (scaleUnits < 0 || isnan(scaleUnits))  {
        weight = 0.00;
      } else {
        weight = scaleUnits * rationGrams;
      }

      if (debug) Serial << "Tenzo: ScaleUnits - " << scaleUnits << " weight - " << weight << endl;
      scale.power_down();
    }

    void setCaliboration (float& kg)  {
      scale.power_up();
      scaleUnits = scale.get_units(3);
      if (scaleUnits < 0 || isnan(scaleUnits))  {
        weight = 0.00;
      } else {
        tempCaliboration = scaleUnits / kg;
        weight = scaleUnits / tempCaliboration;

        if (debug) Serial << "kg " << kg << "\t tempCaliboration: " << tempCaliboration << "\t scaleUnits: " << scaleUnits << "\t Weight: " << weight << endl;
      }
      scale.power_down();
    }

    void getCaliboratedUnits  ()  {
      scale.power_up();
      scaleUnits = scale.get_units(3);
      if (scaleUnits < 0 || isnan(scaleUnits))  {
        weight = 0.00;
      } else {
        weight = scaleUnits / caliboration;
      }
      scale.power_down();

    }
};

Tenzo tenzo;
//////////////////////////////////////////
// Переменная для отправки на ПК
String to_serial = "";


void send_data() {
  Serial3.print(String(to_serial + '\n'));
  to_serial = "";
};
//////////////////////////////////////////

//////////////////////////////////////////

void BTinit() {

  if (BT_VER == 1) {
    Serial3.print("AT+NAME=" + BTname + "\r\n");
    delay(100);
    Serial3.print("AT+PSWD=\"" + BTpin + "\"\r\n");
  } else if (BT_VER == 2) {
    Serial3.print("AT+NAME" + BTname);
    delay(200);
    Serial3.print("AT+PIN" + BTpin);
  }
}

//////////////////////////////////////////


//////////////////////////////////////////
//  Структура данных настроек
class  Settings  {
  public:
    float tempclearWay;           //  1) Еталлоное расстояние от начала конструкции до датчиков
    float tempVl1;                //  2) Калибровочное значение Первого лазерного дальномера
    float tempVl2;                //  3) Калибровочное значение Второго лазерного дальномера
    float tempVl3;                //  4) Калибровочное значение Третьего лазерного дальномера
    float tempTenzo;              //  5) Калибровочное значение весов (тензодатчик))
    int   tempTenzoAccuracy;      //  6) Округление значение весов. 1, 10, 50, 100 грамм
    float tempTenzoCaliboration;  //  7) Полученное калибровочное значение для тензодатчика
    float tempDeviceMode = 0;     //  8) Тип работы устройства. 0 - ручной с джойстиком, 1 - по блютусу

    float clearWay = 1000;    //  1) Еталлоное расстояние от начала конструкции до датчиков
    float Vl1 = 0;            //  2) Калибровочное значение Первого лазерного дальномера
    float Vl2 = 0;            //  3) Калибровочное значение Второго лазерного дальномера
    float Vl3 = 0;            //  4) Калибровочное значение Третьего лазерного дальномера
    float Tenzo = -3.7;       //  5) Калибровочное значение весов (тензодатчик))
    float tenzoAccuracy = 1;  //  6) Округление значение весов. 1, 10, 50, 100 грамм
    float TenzoCaliboration;  //  7) Полученное калибровочное значение для тензодатчика
    float deviceMode = 0;     //  8) Тип работы устройства. 0 - ручной с джойстиком, 1 - по блютусу


    String  settingsParametersName[12] = {"clearWay", "Vl1", "Vl2", "Vl3", "Tenzo", "tenzoAccuracy"};
    float   settingsParametersValue[12] = {1000, 0, 0, 0, -3.7, 1};

    void findIndexArrayOfName (char&  localstring)  {

      int m = sizeof(settingsParametersName) / sizeof(float);

      for (int i = 0; i < m; i++)  {
        if (settingsParametersName[i] == localstring ) {
          return i;
        }
      }
    }
    //  Чтение
    float EEPROM_float_read(int addr) {
      byte raw[4];
      for (byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr + i);
      float &num = (float&)raw;
      return num;
    }
    //  Запись
    float EEPROM_float_write(int addr, float num) {
      byte raw[4];
      (float&)raw = num;
      for (byte i = 0; i < 4; i++) EEPROM.write(addr + i, raw[i]);
    }

    void saveMainSettings () {
      noInterrupts();
      EEPROM_float_write(4,   tempVl1);
      EEPROM_float_write(8,   tempVl2);
      EEPROM_float_write(16,  tempVl3);
      EEPROM_float_write(24,  tempTenzo);
      EEPROM_float_write(28,  tempTenzoAccuracy);
      EEPROM_float_write(40,  tempDeviceMode);

      Vl1 = tempVl1;
      Vl2 = tempVl2;
      Vl3 = tempVl3;
      tenzoAccuracy = tempTenzoAccuracy;
      deviceMode = tempDeviceMode;

      if (debug) Serial << "Save Settings:" << endl;
      if (debug) Serial << "Vl1: " << tempVl1 << endl;
      if (debug) Serial << "Vl2: " << tempVl2 << endl;
      if (debug) Serial << "Vl3: " << tempVl3 << endl;
      if (debug) Serial << "Tenzo: " << tempTenzo << endl;
      if (debug) Serial << "Tenzo Accuracy: " << tempTenzoAccuracy << endl;
      if (debug) Serial << "Device Mode: " << tempDeviceMode << endl;
      interrupts();
    }


    void saveCaliborationSettings (float& tempCaliboration, float& inputWeight) {
      noInterrupts();
      if (isnan(inputWeight) || isnan(tempCaliboration)) { //Проверяем если новая плата - обнуляем значения
        inputWeight = 0;
        tempCaliboration = 0;
      }
      EEPROM_float_write(32,  tempCaliboration);
      tenzo.caliboration = tenzo.tempCaliboration;

      EEPROM_float_write(36,  inputWeight);
      if (debug) Serial << "Caliboration Saved" << endl;

      interrupts();
    }


    void checkSettingValues ()  { //  Проверяет на корректность значений натсроек в памяти . Если некорректны - переписываем на стандартные.
      //  Стандартные настройки   {1000, 0, 0, 0, -3.4, 1};
      int Numbers[10] = {0};
      int k = 0;
      float mode = 0;
      mode = EEPROM_float_read(40);
      if (mode != 10 || mode != 20) EEPROM_float_write(40, 20);

      for (int i = 0; i < 6; i++) {
        Numbers[i] = EEPROM_float_read(i + 4);
        k += 4; //  4 байта
      }

      for (int i = 0; i < 6; i++) {
        if (Numbers[i] > 2000 || Numbers[i] < -1000) {    //  Допустимые границы
          Numbers[i] = 0;
          EEPROM_float_write(i + 4,  0);
        }
      }

    }


};

Settings settings;
//////////////////////////////////////////


NewPing sonar1(PIN_TRIG, PIN_ECHO, MAX_DISTANCE);
NewPing sonar2(PIN_TRIG_2, PIN_ECHO_2, MAX_DISTANCE);
NewPing sonar3(PIN_TRIG_3, PIN_ECHO_3, MAX_DISTANCE);

///////////// Setup
void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  delay(500);
  BTinit();

  pinMode(swPin, INPUT_PULLUP);

  lcd.begin(20, 4);
  lcd.setBacklight(HIGH);                                 //  Set Back light turn On
  delay(100);
  lcd.createChar(1, rus_D);                               //  Добавление спец символа.
  delay(50);
  lcd.createChar(2, rus_Y);                               //  Добавление спец символа.
  delay(50);
  lcd.createChar(3, rus_II);                              //  Добавление спец символа.
  delay(50);
  lcd.createChar(4, rus_I);                               //  Добавление спец символа.
  delay(100);
  lcd.createChar(5, rus_YA);                              //  Добавление спец символа.
  delay(50);
  lcd.createChar(6, rus_L);                               //  Добавление спец символа.
  delay(50);
  lcd.createChar(7, rus_B);                               //  Добавление спец символа.
  //lcd.createChar(0, customChar);                          //  Добавление спец символа.
  delay(100);

  lcd.setCursor(5, 2);                                    //  Move cursor to 2
  lcd.print("Loading...");
  lcd.setCursor(0, 1);

  //  Тензодатчик
  scale.set_scale();
  scale.tare();                                         //  Сброс масштаба до 0
  long zero_factor = scale.read_average();              //  Получаем базовое чтение
  //

  trackBallPosition.X = -1;
  trackBallPosition.Y = -1;


  lcd.clear();
  settings.checkSettingValues();
  //  Настройки
  EEPROM_float_write(28,10);
  if (int(settings.EEPROM_float_read(220)) != 100) {
    settings.EEPROM_float_write(0,1000);
    settings.EEPROM_float_write(4, 0);
    settings.EEPROM_float_write(8, 0);
    settings.EEPROM_float_write(12, 0);
    settings.EEPROM_float_write(16, 0);
    settings.EEPROM_float_write(24, 10);
    settings.EEPROM_float_write(28, 10);
    settings.EEPROM_float_write(32, 12181.00);
    settings.EEPROM_float_write(40, 20);
    settings.EEPROM_float_write(220, 100);
  }

  settings.clearWay           = settings.EEPROM_float_read(0);
  settings.Vl1                = int(settings.EEPROM_float_read(4));
  settings.Vl2                = int(settings.EEPROM_float_read(8));
  settings.Vl3                = int(settings.EEPROM_float_read(16));
  settings.Tenzo              = settings.EEPROM_float_read(24);
  settings.tenzoAccuracy      = settings.EEPROM_float_read(28);
  settings.TenzoCaliboration  = settings.EEPROM_float_read(32);
  tenzo.caliboration          = settings.EEPROM_float_read(32);
  settings.deviceMode         = settings.EEPROM_float_read(40);
  if (settings.deviceMode == 20) menuPage = 200;

  if (debug) Serial.println(settings.TenzoCaliboration);
  //  Проверка на отрицательное значение кэффициента тензодатчика
//  if (tenzo.caliboration <= 0 || isnan(settings.TenzoCaliboration) || isnan(float(settings.EEPROM_float_read(32)))) {
//    Serial.println("New pcb set default calibration!!");
//    tenzo.caliboration = tenzo.defaultCaliboration;
//    settings.EEPROM_float_write(32,  tenzo.caliboration);
//  }

  if (debug) Serial << "EEPROM clearWay: "             <<   settings.clearWay << endl;
  if (debug) Serial << "EEPROM Vl1: "                  <<   settings.Vl1 << endl;
  if (debug) Serial << "EEPROM Vl2: "                  <<   settings.Vl2 << endl;
  if (debug) Serial << "EEPROM Vl3: "                  <<   settings.Vl3 << endl;
  if (debug) Serial << "EEPROM Tenzo: "                <<   settings.Tenzo << endl;
  if (debug) Serial << "EEPROM TenzoAccurancy: "       <<   settings.tenzoAccuracy << endl;
  if (debug) Serial << "EEPROM Tenzo Сoefficient: "    <<   tenzo.caliboration << endl;

  attachInterrupt(0, changeKeyJoystickPosition, FALLING);

}


///////////////// Loop
void loop() {


  if (debug) Serial << menuPage << " " << isKeyPressed << endl;
  if (settings.deviceMode == 10) { //  Режим работы с джойстиком
    //  Страницы меню:
    switch (menuPage) {
      case 0: menuStartPage();
        break;
      case 1: menuShowOnLCDData();
        break;
      case 2: menuPageVersion();
        break;
      case 3: menuStartPage();
        break;
      case 4: menuPageSettings ();
        break;
      case 5: menuPageSettingSave ();
        break;
      case 6: menuPageSettingSaveYes  ();
        break;
      case 7: menuPageSettingCalibration  ();
        break;
    };
  } //  Режим работы по bluetooth
  else if  (settings.deviceMode == 20) {
    Serial.println("Changing Menu...");
    Serial << "Go to menuPage " << menuPage << " Sub " << menuSubPage << endl;
    command = "";

    //  Страницы меню:
    switch (menuPage) {
      case 200: menuBTShowOnLCDData();
        break;
      case 201: menuPageBTSettings ();
        break;
      case 202: menuPageBTSettingWeight ();
        break;
      case 203: menuPageBTSettingSonars ();
        break;
    };
  }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Логика переходов из одного меню в другое:
void changeKeyJoystickPosition () {

  if (debug) Serial << "Interrput " << endl << "Menu Page: " << menuPage <<  " " << "TrackBallPosition Y: " << trackBallPosition.Y << endl;
  if (debug) Serial << currentMillis << " Prev: " << previousMillis << endl;

  boolean state = digitalRead(swPin);

  if (currentMillis != previousMillis && unpressed && !state) {
    unpressed = false;

    //  Входы
    if (menuPage == 0 && trackBallPosition.Y == 0) {
      menuPage = 1; //  StartPage ->  Data Page
      if (debug) Serial << "Going to Data Page" << endl;
    } else if (menuPage == 0 && trackBallPosition.Y == 1) {
      menuPage = 4; //  StartPage ->  Setting Page
      if (debug) Serial << "Going to Version Pagee" << endl;
    } else if (menuPage == 0 && trackBallPosition.Y == 2) {
      menuPage = 2; //  StartPage ->  Version Page
      if (debug) Serial << "Going to Version Pagee" << endl;
    }
    //  Выходы
    else if (menuPage == 1) {
      menuPage = 0; //  Data Page ->  StartPage
      if (debug) Serial << "Going to StartPage" << endl;
    }
    else if (menuPage == 2) {
      menuPage = 0; //  Version Page  ->  StartPage
      if (debug) Serial << "Going to StartPage" << endl;
    }
    else if (menuPage == 4 && trackBallPosition.Y != 3) {
      menuPage = 5; //  Setting Page  ->  Сохранить?
      trackBallPosition.Y = 1;
      prevpositionY = 1;
      if (debug) Serial << "Save Settings?" << endl;
    }
    else if (menuPage == 4 && trackBallPosition.Y == 3) {
      menuPage = 7; //  Setting Page  ->  Сохранить?
      trackBallPosition.Y = 1;
      prevpositionY = 1;
      if (debug) Serial << "Setting SubPage Caliboration" << endl;
    }
    else if (menuPage == 7) {
      menuPage = 5; //  Setting Caliboration Page  ->  Main Page
      trackBallPosition.Y = 1;
      prevpositionY = 1;
      if (debug) Serial << "Setting SubPage Caliboration" << endl;
    }
    else if (menuPage == 5 && trackBallPosition.Y == 1) {
      menuPageSettingSaveYes();
      menuPage = 6; //  Setting Page  ->  Save Main Settings
      if (debug) Serial << "Saving Settings" << endl;
      if (debug) Serial << "Going to Setting Page" << endl;
    }
    else if (menuPage == 5 && trackBallPosition.Y == 2) {
      menuPage = 0; //  Setting Page  ->  MainMenu
      if (debug) Serial << "Discard Saving" << endl;
      if (debug) Serial << "Going to Setting Page" << endl;
    }
    else if (menuPage == 3) {
      menuPage = 0; //  Version Page  ->  StartPage
      if (debug) Serial << "Going to StartPage" << endl;
    }
    else if (menuPage >= 200) {
      menuPage = 200; //  Version Page  ->  StartPage
      if (debug) Serial << "Going to StartPage" << endl;
    }
  } else if (state) {
    unpressed = true;
  }

  //previousMillis += 50;      //  Для подавление повторного прерывания.

}
/////////////////////////


/////////////////////////
void menuStartPage () {
  if (debug) Serial << "Show Start Menu" << endl;

  lcd.clear();
  delay(100);
  lcd.createChar(1, rus_D);                               //  Добавление спец символа.
  lcd.createChar(2, rus_Y);                               //  Добавление спец символа.
  delay(50);
  lcd.createChar(3, rus_II);                              //  Добавление спец символа.
  delay(100);
  lcd.createChar(4, rus_I);                               //  Добавление спец символа.
  lcd.createChar(5, rus_YA);                              //  Добавление спец символа.
  delay(100);
  lcd.createChar(6, rus_L);                               //  Добавление спец символа.
  lcd.createChar(7, rus_B);                               //  Добавление спец символа.
  delay(50);
  trackBallPosition.Y = 0;
  prevpositionY = 0;

  while (menuPage == 0) {
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }

    currentMillis = millis();

    if (currentMillis - previousMillis > JOYSTICK_MOVE) {

      previousMillis = currentMillis;

      joystick.readY();

      prevpositionY = trackBallPosition.Y;
      joystick.upOrDown(trackBallPosition.Y, 0, 2);

      lcd.setCursor(2, trackBallPosition.Y);
      lcd.write(trackball);

      if (prevpositionY != trackBallPosition.Y) {
        lcd.setCursor(2, prevpositionY);
        lcd << " ";
      }
      ///
      //  (1, rus_D);                               //  Буква Д
      //  (2, rus_Y);                               //  Буква Ы
      //  (3, rus_II);                              //  Буква Й
      //  (4, rus_I);                               //  Буква И
      //  (5, rus_YA);                              //  Буква Я
      //  (6, rus_L);                               //  Буква Л
      //  (7, rus_B);                               //  Буква Б

      lcd.setCursor(3, 0); // Move cursor to 0
      //lcd <<" Show data       ";
      lcd << ' ';
      lcd.write(byte(1));
      lcd << "AHH";
      lcd.write(byte(2));
      lcd << 'E' << "          ";

      lcd.setCursor(3, 1); // Move cursor to 0
      //lcd << " Settings        ";
      lcd << " HACTPO";
      lcd.write(byte(3));
      lcd << 'K';
      lcd.write(byte(4));
      lcd << "       ";

      lcd.setCursor(3, 2); // Move cursor to 0

      //lcd << " Version         ";
      lcd << " BEPC";
      lcd.write(byte(4));
      lcd.write(byte(5));
      lcd << "          ";

      if (debug) Serial << "TrackBallPositionY: " << trackBallPosition.Y << "  joystick.axisY: " << joystick.axisY << "  " << "Menu Page: " << menuPage << endl;
    }
  }
  //return;
}
/////////////////////////

/////////////////////////
void menuShowOnLCDData()  {

  if (debug) Serial << "Show Data On LCD" << endl;
  lcd.clear();
  //  Обнуляем шарик
  trackBallPosition.X = -1;
  trackBallPosition.Y = -1;

  tenzo.caliboration = settings.EEPROM_float_read(32);

  if (debug) Serial << "tenzo.caliboration:" << tenzo.caliboration << endl;

  // scale.set_scale(settings.Tenzo);                 //  Устанавливаем коэффициент для тензодатчика

  previousMillis = currentMillis - 1505;

  while (menuPage == 1) {
    if (!digitalRead(swPin) && unpressed) {
      unpressed = false;
      menuPage = 0;
      break;
    }
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }

    if (currentMillis - previousMillis > 1000) {

      previousMillis = currentMillis;

      //Box1.read_three_sensors(Box1.length, Box1.width, Box1.height);
      //  Читаем сенсоры
      sonya1.pingFloat  = sonar1.ping_median(3);
      delay(100);
      sonya2.pingFloat  = sonar2.ping_median(3);
      delay(100);
      sonya3.pingFloat  = sonar3.ping_median(3);
      delay(100);

      if (debug) Serial << "sonya1: " << sonya1.pingFloat << "\t sonya2: " <<  sonya2.pingFloat << "\t sony3: " << sonya3.pingFloat << endl;

      Box1.length = float(1000.0 - sonya1.pingFloat / 5.7 )  / 10.0;

      Box1.height = float(1000.0 - sonya2.pingFloat / 5.7 )  / 10.0;

      Box1.width  = float(1000.0 - sonya3.pingFloat / 5.7 )   / 10.0;


      if (debug) Serial << settings.Vl1 + Box1.length << " " << settings.Vl2 + Box1.height << " " << settings.Vl3 + Box1.width << endl;

      if (debug) Serial << settings.Vl1 << " " << settings.Vl2 << " " << settings.Vl3 << endl;


      if (debug) Serial << Box1.length << " " << Box1.height << " " << " " <<  Box1.width << endl;


      unsigned long total = Box1.Volume(settings.Vl1, settings.Vl2, settings.Vl3);

      //static char outstr[15];
      String outstr;
      float floatvar  = float(total) / 1000000000;

      outstr = String(floatvar, 3);

      //  Данные из тензодатчика
      tenzo.getCaliboratedUnits();

      if (debug) Serial << "scaleUnits: " << tenzo.scaleUnits << "\t caliboration: " <<  tenzo.caliboration << endl;


      //  Округляем до : {1, 10, 50, 100} Согласно настройкам.

      tenzo.weight = tenzo.weight / settings.tenzoAccuracy * settings.tenzoAccuracy;

      String tenzoStr;

      float floatTenzoVar  =  float(long(tenzo.weight * 1000) / int(settings.tenzoAccuracy) * int(settings.tenzoAccuracy)) / 1000;

      tenzoStr = String(floatTenzoVar, 3);


      if (Box1.length >= 0)  {
        lcd.setCursor(2, 0);
        lcd << String(Box1.length + (settings.Vl1),1) << "  "     ;//<< endl;
        lcd.setCursor(7, 0);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 0);
        lcd << "0 " ;//<< endl;   //  Out of range
        lcd.setCursor(7, 0);  lcd << "cm";
      }

      lcd.setCursor(12, 0);
      lcd << "BEC:";

      if (Box1.width >= 0)  {
        lcd.setCursor(2, 1);
        lcd << String(Box1.height + (settings.Vl2),1) << "  "   ;
        lcd.setCursor(7, 1);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 1);
        lcd << "0 ";   //  Out of range
        lcd.setCursor(7, 1);  lcd << "cm";
      }

      lcd.setCursor(13, 1);
      if (debug) Serial << "Tenzo weight: " << tenzo.weight << endl;
      if (tenzo.weight < 10 ) {
        lcd << tenzoStr << "   ";
      } else if (tenzo.weight < 100 )  {
        lcd << tenzoStr << "  ";
      } else if (tenzo.weight < 1000 )  {
        lcd << tenzoStr << " ";
      }

      if (Box1.height >= 0)  {
        lcd.setCursor(2, 2);
        lcd << String(Box1.width + (settings.Vl3),1) << "  "  ;
        lcd.setCursor(7, 2);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 2);
        lcd << "0 ";   //  Out of range
        lcd.setCursor(7, 2);  lcd << "cm";
      }

      for (int i = 0; i <= 3; i++) { // Разделитель

        lcd.setCursor(10, i);
        if (i < 3)  {
          lcd << "|";
        } else {
          lcd << "| ";
        }
      }


      lcd.setCursor(16, 2);
      lcd << "kg";

      if (Box1.length > 0 && Box1.height > 0 && Box1.width > 0 ) {
        lcd.setCursor(0, 3);
        lcd << outstr   << " "  ;
        lcd.setCursor(7, 3);  lcd << "m3 ";
      } else  {
        lcd.setCursor(0, 3);
        lcd << "Out of range";
        lcd.setCursor(7, 3);  //lcd << "m3 ";
      }

      //Формируем строку для отправки в последовательный порт - length;height;width;Volume;weight;
      to_serial = String(Box1.length + (settings.Vl1),1) + ';' + String(Box1.height + (settings.Vl2),1) + ';' + String(Box1.width + (settings.Vl3),1) + ';'
                  + String(outstr) + ';' + String(tenzoStr) + ';';
      send_data();

    }
    currentMillis = millis();   //  Обновляем таймер цикла

  }

  return;
}
/////////////////////////

void menuPageVersion()  {

  if (debug) Serial << "Show Version Page" << endl;
  lcd.clear();

  //  Обнуляем шарик
  trackBallPosition.X = -1;
  trackBallPosition.Y = -1;

  //scale.tare();

  lcd.setCursor(1, 0);
  lcd << " " << "Version 2.0";

  while (menuPage == 2)  {
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }


    if (currentMillis - previousMillis > 900) {

      previousMillis = currentMillis;

      joystick.readAll();

      tenzo.getUnits();

      if (millis() - PreviousMillisPing >= 50) {
        PreviousMillisPing = millis();
        float tmp_v1, tmp_v2, tmp_v3 = 0.0;
        tmp_v1 = sonar1.ping();
        tmp_v2 = sonar2.ping();
        tmp_v3 = sonar3.ping();

        if (tmp_v1 > 0.0) {
          sonya1.pingFloat = tmp_v1;
        }
        if (tmp_v2 > 0.0) {
          sonya2.pingFloat = tmp_v2;
        }
        if (tmp_v3 > 0.0) {
          sonya3.pingFloat = tmp_v3;
        }
      }

      //  D1
      lcd.setCursor(2, 1);
      lcd << sonya1.pingFloat / 5.7 << "   ";
      lcd.setCursor(12, 1);
      lcd << sonya1.pingFloat / 5.7 << "  ";

      if (debug) Serial << int(sonya1.pingFloat) << endl;

      //  D2
      lcd.setCursor(2, 2);
      lcd << sonya2.pingFloat / 5.7  << "  ";

      lcd.setCursor(12, 2);
      lcd << sonya2.pingFloat / 5.7  << "   ";

      // D3
      lcd.setCursor(2, 3);
      lcd << sonya3.pingFloat / 5.7 << "   ";

      lcd.setCursor(12, 3);
      lcd << sonya3.pingFloat / 5.7 << "   ";

    }
    currentMillis = millis();   //  Обновляем таймер цикла
  }
}


/////////////////////////

/////////////////////////
//  Menu Page Settings

void menuPageSettings ()  {
  if (debug) Serial << "Show Settings Menu" << endl;
  subMenuPage = 1;
  lcd.clear();
  delay(100);
  lcd.createChar(1, rus_D);                               //  Добавление спец символа.
  delay(10);
  lcd.createChar(2, rus_Y);                               //  Добавление спец символа.
  delay(100);
  lcd.createChar(3, rus_b);                               //  Добавление спец символа. (Мягкий знак)
  delay(10);
  lcd.createChar(4, rus_I);                               //  Добавление спец символа.
  delay(100);
  lcd.createChar(6, rus_L);                               //  Добавление спец символа.
  delay(10);
  lcd.createChar(7, rus_B);                               //  Добавление спец символа.
  delay(100);
  lcd.createChar(5, rus_III);                             //  Добавление спец символа.
  delay(10);

  //scale.tare();

  int i = 0;
  trackBallPosition.X = 0;
  trackBallPosition.Y = 0;

  prevpositionX = 0;
  prevpositionY = 0;

  int grade[7] = {0};                                         //  Массив значений дальномеров.
  int tenzoAccurencyDefaultValues[4] = {1, 10, 50, 100};      //  Массив элементов Кратности. Это может быть 1, 10, 50, 100 грм
  int indexOftenzoAccurencyDefaultValues = 0;                 //  Индекс для массива элементов кратности.
  int arrayDeviceMode[2] = {1, 2};                            //  Массив элементов режима работы (MODE). Это может быть 1 или 2
  int indexOfDeviceMode = 0;                                  //  Индекс для массива элементов режима работы (MODE).

  //  Записываем в переменный значения из EEPROM
  settings.Vl1                = settings.EEPROM_float_read(4);
  settings.Vl2                = settings.EEPROM_float_read(8);
  settings.Vl3                = settings.EEPROM_float_read(16);
  settings.Tenzo              = settings.EEPROM_float_read(24);
  settings.tenzoAccuracy      = settings.EEPROM_float_read(28);
  settings.TenzoCaliboration  = settings.EEPROM_float_read(32);
  settings.tempDeviceMode     = settings.EEPROM_float_read(40);

  //  Находим индекс в массиве "Кратности" по значениею settings.tenzoAccuracy
  for (int j = 0; j < 4; j++) {
    if (debug) Serial << "Tenzo Array: i " << j << "\t Value: " << tenzoAccurencyDefaultValues[j] << endl;
    if (debug) Serial << settings.tenzoAccuracy << endl;

    if (tenzoAccurencyDefaultValues[j] == settings.tenzoAccuracy) {
      indexOftenzoAccurencyDefaultValues = j;
      if (debug) Serial << "Found tenzo Accurancy index: " << indexOftenzoAccurencyDefaultValues << endl;
      break;
    }
  }


  while (menuPage == 4) {
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }
    if (currentMillis - previousMillis > JOYSTICK_MOVE) {

      previousMillis = currentMillis;

      joystick.readX();
      joystick.readY();

      //scale.set_scale(settings.tempTenzo);    //  Устанавливаем коэффициент
      //scale.set_scale(-3.4);                  //  Устанавливаем коэффициент


      //if (debug) Serial << "joystick.axisX: " << joystick.axisX << endl;
      //  Читаем позицию Х. На основе его меняем коэффициент параметров в большую сторону или меньшую.
      //  Для первых четырёх параметров
      if (trackBallPosition.Y < 4) {

        if (joystick.axisX > 822 && grade[trackBallPosition.Y] < 200) {
          grade[trackBallPosition.Y] += 1;
          //  Right
        } else if (joystick.axisX < 200 && grade[trackBallPosition.Y] > -200)  {
          grade[trackBallPosition.Y] -= 1;
          //  Left
        }
      }


      //  Настройка Кратности
      else if (trackBallPosition.Y == 4) {
        if (joystick.axisX > 1000 && indexOftenzoAccurencyDefaultValues < 3) {
          indexOftenzoAccurencyDefaultValues += 1;
          //  Right
        } else if (joystick.axisX < 10 && indexOftenzoAccurencyDefaultValues > 0)  {
          indexOftenzoAccurencyDefaultValues -= 1;
          //  Left
        }
      }
      //  Настройка MODE
      else if (trackBallPosition.Y == 5) {
        if (joystick.axisX > 1000 && indexOfDeviceMode < 20) {
          indexOfDeviceMode += 10;
          //  Right
        } else if (joystick.axisX < 10 && indexOfDeviceMode > 10)  {
          indexOfDeviceMode -= 10;
          //  Left
        }
      }
      //

      prevpositionY = trackBallPosition.Y;
      joystick.upOrDown(trackBallPosition.Y, 0, 4);


      prevpositionX = trackBallPosition.X;



      settings.tempVl1    = settings.Vl1  + (float(grade[0]) / 10);
      settings.tempVl2    = settings.Vl2  + (float(grade[1]) / 10);
      settings.tempVl3    = settings.Vl3  + (float(grade[2]) / 10);
      settings.tempTenzo  = settings.Tenzo  + (float(grade[3]) / 10);
      settings.tempTenzoAccuracy  = tenzoAccurencyDefaultValues[indexOftenzoAccurencyDefaultValues];
      settings.tempDeviceMode     = arrayDeviceMode[indexOfDeviceMode];

      //settings.checkNegativeNumber(settings.tempVl1, settings.tempVl2, settings.tempVl3);   //  Убеждаемся что нету -0.00

      lcd.setCursor(1, trackBallPosition.Y % 4);
      lcd.write(trackball);

      if (prevpositionY != trackBallPosition.Y) {
        lcd.setCursor(1, prevpositionY % 4);
        lcd << " ";
      }

      if (trackBallPosition.Y >= 0 && trackBallPosition.Y < 4) {  //  Первая страница настроек
        lcd.setCursor(3, 0);
        //  "Длина"
        //  "Д1"
        lcd.write(byte(1));
        lcd << "1: " << settings.tempVl1 << "       ";

        //  "Д2"
        lcd.setCursor(3, 1);
        lcd.write(byte(1));
        lcd << "2: " << settings.tempVl2 << "       ";

        //  "Д3"
        lcd.setCursor(3, 2);
        lcd.write(byte(1));
        lcd << "3: " << settings.tempVl3 << "       ";

        // "КАЛИБРОВКА"
        lcd.setCursor(3, 3);
        lcd  << "KA";
        lcd.write(byte(6));
        lcd.write(byte(4));
        lcd.write(byte(7));
        lcd << "POBKA     ";
      }

      if (trackBallPosition.Y >= 4 && trackBallPosition.Y < 8) {  //  Вторая страница настроек
        //  "Кратность"
        lcd.setCursor(3, 0);
        lcd << "KPATHOCT";
        lcd.write(byte(3));
        lcd << ": " << settings.tempTenzoAccuracy << "         ";

        //  Режим работы (MODE) 1 - работа с джойстиком, 2 - работа по БТ
        lcd.setCursor(3, 1);
        lcd << "MODE: " << settings.tempDeviceMode << "           ";

        //  Пустое меню
        lcd.setCursor(3, 2);
        lcd << "               ";

        //  Пустое меню
        lcd.setCursor(3, 3);
        lcd << "               ";

      }


      if (debug) Serial << "TrackBallPositionY: " << trackBallPosition.Y << "  joystick.axisY: " << joystick.axisY << "  " << "Menu Page: " << menuPage << " Prev Postion: " << prevpositionY << endl;
    }
    currentMillis = millis();
  }



} //  end menuPageSettings ()
/////////////////////////

void menuPageSettingSave  ()  {
  if (debug) Serial << "Show Save Settings?" << endl;
  lcd.clear();
  lcd.createChar(1, rus_D);                               //  Добавление спец символа.
  delay(10);
  lcd.createChar(3, rus_II);                              //  Добавление спец символа.
  delay(10);
  lcd.createChar(7, rus_b);                               //  Добавление спец символа. (Мягкий знак)
  delay(10);


  while (menuPage == 5) {
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }

    lcd.setCursor(4, 0);
    //  рус: "СОХРАНИТЬ?"
    lcd << "COXPAH";
    lcd.write(byte(3));
    lcd << "T";
    lcd.write(byte(7));
    lcd << "?";


    currentMillis = millis();

    if (currentMillis - previousMillis > JOYSTICK_MOVE) {

      previousMillis = millis();

      joystick.readY();
      prevpositionY = trackBallPosition.Y;

      if (joystick.axisY > 822 && trackBallPosition.Y > 1) {
        trackBallPosition.Y -= 1;     //  Up
      } else if (joystick.axisY < 200 && trackBallPosition.Y < 2)  {
        trackBallPosition.Y += 1;    //  Down

      } else if (joystick.axisY == 511)  {
        //  Center
        trackBallPosition.Y;
      }

      lcd.setCursor(2, trackBallPosition.Y);
      lcd.write(trackball);

      if (prevpositionY != trackBallPosition.Y) {
        lcd.setCursor(2, prevpositionY);
        lcd << " ";
      }

      lcd.setCursor(4, 1);
      lcd.write(byte(1));
      lcd << "A";
      //lcd << " Yes            ";

      lcd.setCursor(4, 2);
      lcd << "HET";
      //lcd << " No             ";

      lcd.setCursor(4, 3);
      lcd << "                ";

      if (debug) Serial << "TrackBallPositionY: " << trackBallPosition.Y << "  joystick.axisY: " << joystick.axisY << "  " << "Menu Page: " << menuPage << " Prev Postion: " << prevpositionY << endl;
    } //  end if (currentMillis - previousMillis > 50)

  } //  end while (menuPage == 4 && subMenuPage == 1)
}

void  menuPageSettingSaveYes  ()  {
  if (subMenuPage == 1) {
    settings.saveMainSettings();
    if (debug) Serial << "Saving Settings Main Values" << endl;
  } else if (subMenuPage == 2) {
    settings.saveCaliborationSettings(tenzo.tempCaliboration, tenzo.inputWeight);
    if (debug) Serial << "Saving Settings Caliboration Value" << endl;
  }

  menuPage = 0;
  subMenuPage = 0;
}



void menuPageSettingCalibration ()  {
  if (debug) Serial << "Show Calibration Settings Menu" << endl;

  //tenzo.caliboration      = settings.EEPROM_float_read(32);
  tenzo.tempCaliboration  = settings.EEPROM_float_read(32);
  tenzo.inputWeight       = settings.EEPROM_float_read(36);

  subMenuPage = 2;
  lcd.clear();

  delay(100);
  lcd.createChar(5, rus_L);                               //  Добавление спец символа.
  delay(50);
  lcd.createChar(6, rus_I);                               //  Добавление спец символа.  И
  delay(50);
  lcd.createChar(7, rus_B);                               //  Добавление спец символа.
  delay(100);
  //scale.tare();

  trackBallPosition.X = 1;
  trackBallPosition.Y = 1;

  prevpositionX = 1;
  prevpositionY = 1;

  float grade[7] = {tenzo.inputWeight};
  grade[1] = {tenzo.inputWeight};
  //int inputWeight = 1;

  while (menuPage == 7) {
    if (!unpressed && digitalRead(swPin)) {
      unpressed = true;
    }
    currentMillis = millis();

    if (currentMillis - previousMillis > JOYSTICK_MOVE) {

      previousMillis = millis();

      joystick.readX();
      joystick.readY();


      if (joystick.axisX > 822 && grade[trackBallPosition.Y] < 200) {
        grade[trackBallPosition.Y] += 1;
        //  Right
      } else if (joystick.axisX < 200 && grade[trackBallPosition.Y] > 1)  {
        grade[trackBallPosition.Y] -= 1;
        //  Left
      }


      tenzo.setCaliboration(tenzo.inputWeight);

      tenzo.inputWeight = grade[1];

      prevpositionY = trackBallPosition.Y;
      joystick.upOrDown(trackBallPosition.Y, 1, 1);

      lcd.setCursor(0, trackBallPosition.Y);
      lcd.write(trackball);

      if (prevpositionY != trackBallPosition.Y) {
        lcd.setCursor(0, prevpositionY);
        lcd << " ";
      }

      tenzo.setCaliboration(tenzo.inputWeight);
      lcd.setCursor(2, 0);
      //lcd << "Set Caliboraion";
      lcd  << "KA";
      lcd.write(byte(5));
      lcd.write(byte(6));
      lcd.write(byte(7));
      lcd << "POBKA     ";

      lcd.setCursor(2, 1);
      lcd << "BEC: " << tenzo.inputWeight << "   ";
      lcd.setCursor(15, 1);
      lcd << "kg";

      lcd.setCursor(2, 2);
      lcd << "T: ";
      if (tenzo.weight < 10 ) {
        lcd << tenzo.weight << "    ";
      } else if (tenzo.weight < 100 )  {
        lcd << tenzo.weight << "   ";
      } else if (tenzo.weight < 1000 )  {
        lcd << tenzo.weight << "  ";
      } else if (tenzo.weight < 10000 )  {
        lcd << tenzo.weight << " ";
      }

      if (debug) Serial << "inputWeight: " << tenzo.inputWeight << "\t tenzo.weight: " << tenzo.weight << endl;

      lcd.setCursor(15, 2);
      lcd << "kg";

    }
  }

}
//-----------------------------------------------------------------------------------------//
//////////////////////////  Режим работы по bluetooth MODE 1  /////////////////////////////////////

/////////////////////////   MAIN SCREEN DATA    ///////////////////////////////////////////////////
void menuBTShowOnLCDData() {
  if (debug) Serial << "Show Data On LCD" << endl;
  lcd.clear();
  tenzo.caliboration = settings.EEPROM_float_read(32);

  if (debug) Serial << "tenzo.caliboration:" << tenzo.caliboration << endl;

  // scale.set_scale(settings.Tenzo);                 //  Устанавливаем коэффициент для тензодатчика

  previousMillis = currentMillis - 1505;

  while (menuPage == 200) {
    if (currentMillis - previousMillis > 1000) {

      previousMillis = currentMillis;

      //Box1.read_three_sensors(Box1.length, Box1.width, Box1.height);
      //  Читаем сенсоры
      sonya1.pingFloat  = sonar1.ping_median(3);
      delay(100);
      sonya2.pingFloat  = sonar2.ping_median(3);
      delay(100);
      sonya3.pingFloat  = sonar3.ping_median(3);
      delay(100);

      if (debug) Serial << "sonya1: " << sonya1.pingFloat << "\t sonya2: " <<  sonya2.pingFloat << "\t sony3: " << sonya3.pingFloat << endl;

      Box1.length = float(1000.0 - sonya1.pingFloat / 5.7)   / 10.0;

      Box1.height = float(1000.0 - sonya2.pingFloat / 5.7)   / 10.0;

      Box1.width  = float(1000.0 - sonya3.pingFloat / 5.7)    / 10.0;


      if (debug) Serial << settings.Vl1 + Box1.length << " " << settings.Vl2 + Box1.height << " " << settings.Vl3 + Box1.width << endl;
      if (debug) Serial << settings.Vl1 << " " << settings.Vl2 << " " << settings.Vl3 << endl;
      if (debug) Serial << Box1.length << " " << Box1.height << " " << " " <<  Box1.width << endl;

      unsigned long total = Box1.Volume(settings.Vl1, settings.Vl2, settings.Vl3);

      //static char outstr[15];
      String outstr;
      float floatvar  = float(total) / 1000000000;

      outstr = String(floatvar, 3);

      //  Данные из тензодатчика
      tenzo.getCaliboratedUnits();

      if (debug) Serial << "scaleUnits: " << tenzo.scaleUnits << "\t caliboration: " <<  tenzo.caliboration << endl;

      //  Округляем до : {1, 10, 50, 100} Согласно настройкам.

      tenzo.weight = tenzo.weight / settings.tenzoAccuracy * settings.tenzoAccuracy;

      String tenzoStr;

      float floatTenzoVar  =  float(long(tenzo.weight * 1000) / int(settings.tenzoAccuracy) * int(settings.tenzoAccuracy)) / 1000;

      tenzoStr = String(floatTenzoVar, 3);

      if (Box1.length >= 0)  {
        lcd.setCursor(2, 0);
        lcd << String(Box1.length + (settings.Vl1),1) << "  "     ;//<< endl;
        lcd.setCursor(7, 0);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 0);
        lcd << "0 " ;//<< endl;   //  Out of range
        lcd.setCursor(7, 0);  lcd << "cm";
      }

      lcd.setCursor(12, 0);
      lcd << "BEC:";

      if (Box1.width >= 0)  {
        lcd.setCursor(2, 1);
        lcd << String(Box1.height + (settings.Vl2),1) << "  "   ;
        lcd.setCursor(7, 1);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 1);
        lcd << "0 ";   //  Out of range
        lcd.setCursor(7, 1);  lcd << "cm";
      }

      lcd.setCursor(13, 1);
      if (debug) Serial << "Tenzo weight: " << tenzo.weight << endl;
      if (tenzo.weight < 10 ) {
        lcd << tenzoStr << "   ";
      } else if (tenzo.weight < 100 )  {
        lcd << tenzoStr << "  ";
      } else if (tenzo.weight < 1000 )  {
        lcd << tenzoStr << " ";
      }

      if (Box1.height >= 0)  {
        lcd.setCursor(2, 2);
        lcd << String(Box1.width + (settings.Vl3),1) << "  "  ;
        lcd.setCursor(7, 2);  lcd << "cm";
      } else  {
        lcd.setCursor(2, 2);
        lcd << "0 ";   //  Out of range
        lcd.setCursor(7, 2);  lcd << "cm";
      }

      for (int i = 0; i <= 3; i++) { // Разделитель

        lcd.setCursor(10, i);
        if (i < 3)  {
          lcd << "|";
        } else {
          lcd << "| ";
        }
      }

      lcd.setCursor(16, 2);
      lcd << "kg";

      if (Box1.length > 0 && Box1.height > 0 && Box1.width > 0 ) {
        lcd.setCursor(0, 3);
        lcd << outstr   << " "  ;
        lcd.setCursor(7, 3);  lcd << "m3 ";
      } else  {
        lcd.setCursor(0, 3);
        lcd << "Out of range";
        lcd.setCursor(7, 3);  //lcd << "m3 ";
      }

      //Формируем строку для отправки в последовательный порт - length;height;width;Volume;weight;
      to_serial = String(Box1.length + (settings.Vl1),1) + ';' + String(Box1.height + (settings.Vl2),1) + ';' + String(Box1.width + (settings.Vl3),1) + ';'
                  + String(outstr) + ';' + String(tenzoStr) + ';';
      send_data();

    }
    currentMillis = millis();   //  Обновляем таймер цикла
    check_bt_serial();
  }

  return;
}
/////////////////////////\\\\\\\\\\\\\\\\//////////////////////////////////////////////////////////
/////////////////////////   SETTINGS    ///////////////////////////////////////////////////////////
void menuPageBTSettings ()  {
  lcd.clear();
  previousMillis = currentMillis - 1505;
  lcd.createChar(4, rus_II);                              //  Добавление спец символа.  Й
  delay(50);
  lcd.createChar(6, rus_I);                               //  Добавление спец символа.  И
  delay(50);

  while (menuPage == 201) {
    if (currentMillis - previousMillis > 2000) {
      previousMillis = currentMillis;

      lcd.setCursor(0, 3);
      lcd << "HACTPO";
      lcd.write(byte(4));     //  Й
      lcd << "K";
      lcd.write(byte(6));     //  И
      lcd << "  ";
    }
    currentMillis = millis();   //  Обновляем таймер цикла
    check_bt_serial();
  }

}
/////////////////////////\\\\\\\\\\\\\\\\//////////////////////////////////////////////////////////
/////////////////////////   SET WEIGHT    /////////////////////////////////////////////////////////
void menuPageBTSettingWeight ()  {
  lcd.clear();
  previousMillis = currentMillis - 1505;
  int valueForBTWeight = 0;
  int repeatScreen = 0;
  lcd.createChar(4, rus_B);                               //  Добавление спец символа.  Б
  delay(50);
  lcd.createChar(5, rus_D);                               //  Добавление спец символа.  Д
  delay(50);
  lcd.createChar(6, rus_I);                               //  Добавление спец символа.  И
  delay(50);
  lcd.createChar(7, rus_L);                               //  Добавление спец символа.  Л
  delay(50);

  while (menuPage == 202 && menuSubPage == 0) {
    if (currentMillis - previousMillis > 2000) {

      previousMillis = currentMillis;

      lcd.setCursor(0, 1);
      lcd << "BBE";
      lcd.write(byte(5));     //  Д
      lcd.write(byte(6));     //  И
      lcd << "TE BEC" << " "  ;
      lcd.setCursor(0, 3);
      lcd << "KA";
      lcd.write(byte(7));     //  Л
      lcd.write(byte(6));     //  И
      lcd.write(byte(4));     //  Б
      lcd << "POBKA BECA"   << " "  ;
    }
    currentMillis = millis();   //  Обновляем таймер цикла
    check_bt_serial();

  }

  while (menuPage == 202 && menuSubPage == 1) {

    if (valueForBTWeight = tempString.toInt()) valueForBTWeight = tempString.toInt();
    if (currentMillis - previousMillis > 2000 && repeatScreen < 3 && valueForBTWeight == 0) {
      lcd.setCursor(0, 3);
      lcd << "KA";
      lcd.write(byte(7));     //  Л
      lcd.write(byte(6));     //  И
      lcd.write(byte(4));     //  Б
      lcd << "POBKA BECA"   << " "  ;
      lcd.setCursor(0, 2);
      lcd << "BEC: " << valueForBTWeight << "!" ;
      lcd.setCursor(0, 1);
      lcd << "В";     //Введите вес
      lcd << "В";     //Введите вес
      lcd << "E";
      lcd.write(byte(5));     //  Д
      lcd.write(byte(6));     //  И
      lcd << "TE";
      lcd << "BEC" << valueForBTWeight << "!" ;
    } 
    if (currentMillis - previousMillis > 2000 && repeatScreen < 3 && valueForBTWeight > 0) {

      previousMillis = currentMillis;

      lcd.setCursor(0, 3);
      lcd << "KA";
      lcd.write(byte(7));     //  Л
      lcd.write(byte(6));     //  И
      lcd.write(byte(4));     //  Б
      lcd << "POBKA BECA"   << " "  ;
      lcd.setCursor(0, 2);
      lcd << "BEC: "   << valueForBTWeight  ;

      repeatScreen++;    //  Таймер. Ждём после полученных значений по бт. Затем переходим на сохранения данных.
    }
   if (repeatScreen == 3 && valueForBTWeight > 0) {
    Serial.println("REPEAT SCREEN 3");
      float units = 0;
      int scaledValue = 0;
      scale.power_up();
      units = scale.get_units(15);                                //  Получаем значения тензодатчика с предустановленным весом
      scaledValue = units / valueForBTWeight;                     //  Калибровочное значение получается как "Значение с тензодатчика" / "предустановленный вес"
      //scale.set_scale(scaledValue);                               //  Калибруем тензодатчик новым коэффициентом
      scale.power_down();
      saveCaliborationSettings(float(scaledValue), float(valueForBTWeight));    //  Запись отклабрированного значения в EEAPROM

      settings.Tenzo              = settings.EEPROM_float_read(24);
      settings.tenzoAccuracy      = settings.EEPROM_float_read(28);
      settings.TenzoCaliboration  = settings.EEPROM_float_read(32);
      tenzo.caliboration          = settings.EEPROM_float_read(32);
      returnToMainPage();                                         //  Переход в основной экран. Отображение значений датчиков
      return;
    }

    currentMillis = millis();   //  Обновляем таймер цикла

    check_bt_serial();

  }
}
/////////////////////////\\\\\\\\\\\\\\\\//////////////////////////////////////////////////////////
/////////////////////////   SET SONARS    /////////////////////////////////////////////////////////
void menuPageBTSettingSonars ()  {
  lcd.clear();
  previousMillis = currentMillis - 1505;
  int repeatScreen = 0;
  lcd.createChar(4, rus_B);                               //  Добавление спец символа.  Б
  delay(50);
  lcd.createChar(5, rus_D);                               //  Добавление спец символа.  Д
  delay(50);
  lcd.createChar(6, rus_I);                               //  Добавление спец символа.  И
  delay(50);
  lcd.createChar(7, rus_L);                               //  Добавление спец символа.  Л
  delay(50);

  while (menuPage == 203 && menuSubPage == 0) {
    if (currentMillis - previousMillis > 2000) {
      previousMillis = currentMillis;

      lcd.setCursor(0, 3);
      lcd << "KA";
      lcd.write(byte(7));     //  Л
      lcd.write(byte(6));     //  И
      lcd.write(byte(4));     //  Б
      lcd << "POBKA COHAPOB"   << " "  ;
    }
    check_bt_serial();
    currentMillis = millis();   //  Обновляем таймер цикла

  }

  while (menuPage == 203 && menuSubPage == 1 && repeatScreen < 3) {
    if (currentMillis - previousMillis > 2000) {

      previousMillis = currentMillis;

      lcd.setCursor(0, 3);
      lcd << "KA";
      lcd.write(byte(7));     //  Л
      lcd.write(byte(6));     //  И
      lcd.write(byte(4));     //  Б
      lcd << "POBKA COHAPOB"   << " "  ;

      for (int i = 1; i <= 3; i++) {  //  Вывод в каждой строке коэффициента каждого сонара
        lcd.setCursor(0, i - 1);
        lcd << 'k' << i << ": " << arrayOfSonarsValues[i];
      }
      repeatScreen++;
    }

    check_bt_serial();
    currentMillis = millis();   //  Обновляем таймер цикла

  } if (repeatScreen == 3) {
    returnToMainPage();  //  Переход в основной экран. Отображение значений датчиков
    //callArrayOfSonarsValues();
    return;
  }
}
/////////////////////////\\\\\\\\\\\\\\\\//////////////////////////////////////////////////////////

void check_bt_serial() {
  command = "";
  if (Serial3.available() > 0)  {
    while (Serial3.available() > 0) {
      char read_byte;
      read_byte = Serial3.read();
      if (read_byte == '\n') break;
      command += read_byte;
      //Serial.println(command);
      //Serial3.println(read_byte);
      // При символе "1" включаем светодиод
      if (read_byte == '1')
      {
        digitalWrite(LED, HIGH);
      }
      // При символе "0" выключаем светодиод
      if ( read_byte == '0')
      {
        digitalWrite(LED, LOW);
      }
      delay(100);
    }
    //delay(2000);
    bt_command_search();
  } else menuPage == menuPage;

  //Serial.println(command);
}

void bt_command_search() {
  Serial << "Searching for:" << command << ":" << endl;
  if (command.indexOf("MAIN") >= 0) {
    menuPage = 200;   //  GOTO MainPage DATA ON SCREEN
    bt_answer_OK();
  }
  else if (command.indexOf("SETTINGS") >= 0) {
    menuPage = 201;   //  GOTO SETTINGS SCREEN
    Serial.println("Setting Mode");
    bt_answer_OK();
    /// delay(300);
    return;
  }
  else if (command.indexOf("WEIGHT SET W")>=0) {
    tempString = "";
    for (int i = command.indexOf(":")+1; i < command.length(); i++) {
      tempString += command[i];
    }
    menuPage = 202;   //  SET WEIGHT VALUES
    menuSubPage = 1;
    // delay(300);
    return;
  }
  else if (command.indexOf("WEIGHT") >= 0) {
    lcd << " " << "Calliboration weight";
    Serial.println("Calliboration weight");
    menuPage = 202; //  GOTO SETTINGS WEIGHT SCREEN
    menuSubPage = 0;
    bt_answer_OK();
    // delay(300);
    return;
  }
  else if (command.indexOf("SONARS SET") >= 0) {
    bt_answer_OK();
    Serial.println("Sonars values:");
    tempString = "";
    for (int i = command.indexOf("k1:"); i < command.length(); i++) {
      tempString += command[i];
    }
    Serial.println(tempString);

    insertSonarsValues(tempString);
    saveIntoEEPROMSonarsValues();

    menuPage = 203;   //  SET SONARS VALUES
    menuSubPage = 1;

    return;
  }
  else if (command.indexOf("SONARS") >= 0) {
    Serial.println("Calliboration SONARS");
    //  На дисплее отображаются размеры
    Serial.println("Setting Mode");
    menuPage = 203;   //  GOTO SETTINGS SONARS  SCREEN
    menuSubPage = 0;
    bt_answer_OK();
    // delay(300);
    return;
  }

  else if (command.indexOf("SET MODE:") >= 0) {
    tempString = "";
    tempString = command.substring(command.indexOf("MODE:") + 5, command.indexOf(";", command.indexOf("MODE:")) );

    Serial << tempString.toInt() << endl;
    if (tempString.toInt() > 0) {
      changeMode(tempString.toInt());
      bt_answer_OK();
    }
    else {
      bt_answer_Error("Wrong Mode");
    }
    menuPage = 200;   //  SET SONARS VALUES
    menuSubPage = 0;

    return;
  }

  else if (command.indexOf("CANCEL") >= 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd << " " << "Canceled";

    if (menuPage == 202) menuPage = 200;
    else if (menuPage == 203) menuPage = 200;
    // delay(300);
    return;
  }

}

void bt_answer_OK () {
  command += " OK";
  Serial3.println(command/*"OK"*/);
}
void bt_answer_Error (String strError) {
  Serial3.println("Error: " + strError);
}
void changeMode(int m) {
  Serial << "Change Mode From " << settings.deviceMode << " To " << m << endl;
  if ( m > 0 && m <= 20) settings.deviceMode = m;
}

void insertSonarsValues (String tempString) {
  for (int i = 1; i <= 3; i++) {
    int valueForBTWeight = 0;
    String strTemp = "";
    strTemp = tempString.substring( tempString.indexOf("k" + String(i) + ":") + 3, tempString.indexOf(";", tempString.indexOf("k" + String(i) + ":")) );
    Serial << tempString.indexOf("k" + String(i) + ":") << '-' << tempString.indexOf(";", tempString.indexOf("k" + String(i) + ":")) << ' ';
    Serial << strTemp << ' ';
    if (valueForBTWeight = strTemp.toInt()) {
      valueForBTWeight = strTemp.toInt();
      arrayOfSonarsValues[i] = valueForBTWeight;
    }
  }
}
void saveIntoEEPROMSonarsValues () {
  EEPROM_float_write(4,   arrayOfSonarsValues[1]);
  EEPROM_float_write(8,   arrayOfSonarsValues[2]);
  EEPROM_float_write(16,  arrayOfSonarsValues[3]);
  settings.Vl1                = int(settings.EEPROM_float_read(4));
  settings.Vl2                = int(settings.EEPROM_float_read(8));
  settings.Vl3                = int(settings.EEPROM_float_read(16));
  Serial3 << settings.Vl1 << " " << settings.Vl2 << " " << settings.Vl3 << " ";
}
void saveCaliborationSettings (float caliboration, float inputWeight) {
  noInterrupts();

  EEPROM_float_write(32,  caliboration);
  tenzo.caliboration = caliboration;

  EEPROM_float_write(36,  inputWeight);
  Serial << endl << "Caliboration Saved" << '\t' << String(EEPROM_float_read(32)) << '\t' << caliboration << endl << endl;

  interrupts();
}
void returnToMainPage() {
  menuPage = 200;
  menuSubPage = 0;
  //return;
}

void callArrayOfSonarsValues() {
  for (int i = 1; i <= 3; i++) {
    Serial << arrayOfSonarsValues[i] << ' ';
  }
  Serial << endl;
}
//  Чтение
float EEPROM_float_read(int addr) {
  byte raw[4];
  for (byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr + i);
  float &num = (float&)raw;
  return num;
}
//  Запись
float EEPROM_float_write(int addr, float num) {
  byte raw[4];
  (float&)raw = num;
  for (byte i = 0; i < 4; i++) EEPROM.write(addr + i, raw[i]);
}

//-----------------------------------------------------------------------------------------//
