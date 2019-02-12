int pinA      = 2; //Пин прерывания сигнальной линии
int pinButton = 3; //Пин прерывания нажатия кнопки
int pinB      = 4; //другой пин енкодера

long timeButtonPressed = 1000;  // Долгое удержание кнопки после 1,5 секунд

volatile int state = 0;         // Переменная хранящая статус вращения

// Переменные хранящие состояние действия до его выполнения
volatile bool flagCW            = false;     // Было ли вращение по часовой стрелке
volatile bool flagCCW           = false;     // Было ли вращение против часовой стрелки
volatile bool flagButton        = false;     // Было ли нажатие кнопки
volatile bool flagButtonLong    = false;     // Было ли долгое удержание кнопки

volatile long timeButtonDown    = 0;         // Переменная хранящая время нажатия кнопки
volatile bool isButtonDown      = false;     // Переменная хранящая время нажатия кнопки
volatile bool longPressReleased = false;     // Переменная для фиксации срабатывания долгого нажатия




void A(){

  int pinAValue = digitalRead(pinA);        // Получаем состояние пинов A и B
  int pinBValue = digitalRead(pinB);

  cli();                                    // Запрещаем обработку прерываний, чтобы не отвлекаться
  if (!pinAValue &&  pinBValue) state = 1;  // Если при спаде линии А на линии B лог. единица, то вращение в одну сторону
  if (!pinAValue && !pinBValue) state = -1; // Если при спаде линии А на линии B лог. ноль, то вращение в другую сторону
  if (pinAValue && state != 0) {
    if (state == 1 && !pinBValue || state == -1 && pinBValue) { // Если на линии А снова единица, фиксируем шаг
      if (state == 1)   flagCW = true;      // Флаг вращения по часовой стрелке
      if (state == -1) flagCCW = true;      // Флаг вращения против часовой стрелки
      state = 0;
    }
  }
  sei();                                    // Разрешаем обработку прерываний
}

void Button(){
  
  if (millis() - timeButtonDown < 50) return;
  int pinButValue = digitalRead(pinButton);   // Получаем состояние пина кнопки

  cli();                                      // Запрещаем обработку прерываний, чтобы не отвлекаться
  timeButtonDown = millis();                  // Запоминаем время нажатия/отжатия

  if (!pinButValue) {                         // При нажатии подается инвертированный сигнал
    isButtonDown = true;                      // Устанавливаем флаг нажатия кнопки
  }
  else if (isButtonDown) {                    // Если кнопка отжата, смотрим не было ли выполнено действие
    if (!longPressReleased) {                 // Если долгое нажатие не было ни разу отработано, то...
      flagButton = true;                      // Если не было удержания, ставим флаг события обычного нажатия
    }
    isButtonDown = false;                     // Сбрасываем флаг нажатия
    longPressReleased = false;                // Сбрасываем флаг длительного удержания
  }
  sei();                                      // Разрешаем обработку прерываний
}

byte enkoderChek(){

  if (millis() - timeButtonDown > timeButtonPressed && isButtonDown) { // Время длительного удержания наступило
    flagButtonLong = true;
  }

  if (flagCW) {                           // Шаг вращения по часовой стрелке
    // ...
    Serial.println("turn_right");
    flagCW = false;                       // Действие обработано - сбрасываем флаг
  }
  if (flagCCW) {                          // Шаг вращения против часовой стрелки
    // ...
    Serial.println("turn_left");
    flagCCW = false;                      // Действие обработано - сбрасываем флаг
  }
  if (flagButton) {                       // Кнопка нажата
    // ...
    Serial.println("short_press");
    flagButton = false;                   // Действие обработано - сбрасываем флаг
  }
  if (flagButtonLong && isButtonDown) {   // Кнопка удерживается
    if (!digitalRead(pinButton) && millis() - timeButtonDown > timeButtonPressed) { // Защита от ложного срабатывания
      // ...
      Serial.println("long_press");
    }
    
    //=========================================== Настраиваем реакцию на долгое удержание кнопки ===============================================
    // Чтобы событие long_press во время удержания срботало только один раз, необходимо раскомментировать блок и закомментировать следующий
    isButtonDown = false;                 // Программно "отжимаем" кнопку

    // Эти две строки отвечают за то, чтобы при долгом удержании кнопки, событие long_press повторялось каждые 1,5 секунды
    // Для того, чтобы изменить это поведение нужно закомментировать две эти строки и раскомментировать строку из предыдущего блока
    //timeButtonDown = millis();            // Сбрасываем таймер
    //longPressReleased = true;             // Флаг срабатывания долгого удержания, чтобы отсечь генерацию обычного нажатия при отпускании кнопки
    //==========================================================================================================================================

    flagButtonLong = false;               // Действие обработано - сбрасываем флаг
  }
}

void setup(){

  pinMode(pinA, INPUT);           // Пины в режим приема INPUT
  pinMode(pinB, INPUT);           // Пины в режим приема INPUT
  pinMode(pinButton, INPUT);      // Пины в режим приема INPUT

  attachInterrupt(0, A, CHANGE);        // Настраиваем обработчик прерываний по изменению сигнала на линии A
  attachInterrupt(1, Button, CHANGE);   // Настраиваем обработчик прерываний по изменению сигнала нажатия кнопки

  Serial.begin(9600);             // Включаем Serial
}

void loop(){
  
  enkoderChek();
}




// struct str
// {
//   byte s1;
//   byte s2;
//   byte s3;
//   byte s4;
//   byte s5;
//   byte s6;
//   byte s7;
//   byte s8;
//   byte s9;
//   byte s10;
// };

// str myStr;

// byte s1;
// byte s2;
// byte s3;
// byte s4;
// byte s5;
// byte s6;
// byte s7;
// byte s8;
// byte s9;
// byte s10;

// byte s[10];

// void setup(){
//   //myStr = {1,1,1,1,1,1,1,1,1,1};

//   s1 = 1;
//   s2 = 1;
//   s3 = 1;
//   s4 = 1;
//   s5 = 1;
//   s6 = 1;
//   s7 = 1;
//   s8 = 1;
//   s9 = 1;
//   s10 = 1;
    
//   // s[0] = 1;
//   // s[1] = 1;
//   // s[2] = 1;
//   // s[3] = 1;
//   // s[4] = 1;
//   // s[5] = 1;
//   // s[6] = 1;
//   // s[7] = 1;
//   // s[8] = 1;
//   // s[9] = 1;
// }
// void loop(){}
// void loop(){}
// void loop(){}
    
