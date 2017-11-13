          /* ПРОШИВКА ДЛЯ МАШИНКИ v1.0.1 Arduino ProMini */
          
// распиновка Arduino ProMini
const int pin_dirLeft = 11; // 11 pin направление левых колес 1↑ 0↓
const int pin_dirRight = 8; // 8 pin направление правых колес 1↑ 0↓
const int pin_pwmLeft = 10; // 10 pin скорость левых колес ШИМ 0-255
const int pin_pwmRight = 9; // 9 pin скорость правых колес ШИМ 0-255
const int pin_voltBat = A4; // A4 pin вольтметр батареи

// переменные управленияи пинами
boolean dirLeft = HIGH; // направление левых колес 1↑ 0↓ HIGH or LOW
boolean dirRight = HIGH; // направление правых колес 1↑ 0↓ HIGH or LOW
int pwmLeft = 0; // скорость левых колес ШИМ 0-255
int pwmRight = 0; // скорость правых колес ШИМ 0-255
boolean digitalLeft = LOW; // разрешение движения слева 1↑↓ 0x применяется к пину pin_pwmLeft       // alternative
boolean digitalRight = LOW; // разрешение движения справа 1↑↓ 0x применяется к пину pin_pwmRight    // alternative

// переменные управления
boolean enMove = false; // разрешение движения
int counter = 0; // счетчик спидометра

// промежуточные переменные для вычислений
int x_padLmap; // мап левого поворота относительно максимальной скорости
int x_padRmap; // мап правого поворота относительно максимальной скорости

// переменные вольтметра
int voltCounter = 0; // счетчик вольтметра
int analogVal = 0; // значение analogRead(pin_voltage)
int voltVal = 0; // значение после маппинга 0,123,0,500 приведение к 5вольтам. 500 для большего разрешения. напр: 3.7v = 370

// переменные входных блютуз данных
char b_d; // bluetooth data. входные данные с блютуз. переменная для хранения считывания первого управляющего байта
int x_pad = 0; // входные данные с джойстика блютуз. переменная для считывания хранения данных с джойстика. формат: -255 255
int spd_car = 0; // входные данные слайдера скорости. 

void setup() {
  
  // установка управляющих пинов (8,9,10,11) на выход = output = 1;
  pinMode(pin_dirLeft,OUTPUT);
  pinMode(pin_dirRight,OUTPUT);
  pinMode(pin_pwmLeft,OUTPUT);
  pinMode(pin_pwmRight,OUTPUT);
  
  // стартовая установка направление вперед скорость ноль. 11,8 = 1; 10,9 = 0;
  digitalWrite(pin_dirLeft,HIGH);
  digitalWrite(pin_dirRight,HIGH);
  digitalWrite(pin_pwmLeft,LOW);
  digitalWrite(pin_pwmRight,LOW);

  Serial.begin(9600); // инициализация сериал соединения

}

void loop() {

  // проверяем сериал порт на наличие новых инструкций
  if (Serial.available()) {
    b_d = Serial.read(); // считывает первый управляющий байт (символ'x') определяющий дальнейшие действия в переменную b_d
    
    if (b_d == 'X') { // если X
      x_pad = Serial.parseInt(); // повороты. считывает число int в переменную x_pad до первого символа
    }
    else if (b_d == 'S') { // если S
      spd_car = Serial.parseInt(); // скорость. считывает число int в переменную spd_car до первого символа
    }
    else if (b_d == 'F') {
      dirLeft = HIGH;
      dirRight = HIGH;
      enMove = true;
    }
    else if (b_d == 'B') {
      dirLeft = LOW;
      dirRight = LOW;
      enMove = true;
    }
    else if (b_d = 'r') {
      enMove = false;
    }
  }
  //x_pad = constrain(x_pad,-255,255); //ограничивает предел x_pad
  // расчет угла поворота
  if ((x_pad == 0) && (spd_car > 0)) {
    pwmLeft = spd_car;
    pwmRight = spd_car;
  }
  else if ((x_pad < 0) && (spd_car > 0)) {
    x_padLmap = map(x_pad,0,-255,0,spd_car);
    pwmLeft = spd_car - x_padLmap;
    pwmRight = spd_car;
  }
  else if ((x_pad > 0) && (spd_car > 0)) {
    x_padRmap = map(x_pad,0,255,0,spd_car);
    pwmRight = spd_car - x_padRmap;
    pwmLeft = spd_car;
  }
  // УПРАВЛЕНИЕ МАШИНКОЙ
  if (enMove == true) { // если движение разрешено
    digitalWrite(pin_dirLeft,dirLeft);
    digitalWrite(pin_dirRight,dirRight);
    analogWrite(pin_pwmLeft,pwmLeft);
    analogWrite(pin_pwmRight,pwmRight);
  }
  else if (enMove == false) { // если движение запрещено
    digitalWrite(pin_pwmLeft,LOW);
    digitalWrite(pin_pwmRight,LOW);
  }
  // вольтметр батареи pin_voltBat = A4; analogVal analogSum analogRes voltVal voltCounter = 0; maxCounter = 10;
  if (voltCounter < 1000) {
    voltCounter ++;  
  }
  else {
    // отправка данных вольтметра на телефон
    analogVal = analogRead(pin_voltBat);
    voltVal = map(analogVal,0,1023,0,500);
    Serial.print("*v"); // отправка признаков вольтметра
    Serial.println(voltVal); // отправка показаний вольтметра в формате 0-500 = 0-5вольт
    voltCounter = 0;
  }
  // отправка данных состояния скорости на телефон
  if (counter < 100) {
    counter ++; 
  }
  else {
    Serial.print("*s"); // отправка признака скорости
    Serial.println(spd_car); // отправка скорости
    counter = 0;
  }
 //delay(100);

}









