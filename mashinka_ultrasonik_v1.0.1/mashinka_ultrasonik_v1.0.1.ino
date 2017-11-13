          /* ПРОШИВКА ДЛЯ МАШИНКИ v1.0.3 Arduino ProMini */

// СТАРТОВЫЙ КОНФИГ C++
// ДВИГАТЕЛИ
#define DEF_START_PIN_CONFIG_DEF DDRB |= B1111;    // pinMode(11,10,9,8,OUTPUT)  // установить как выход
#define DEF_START_DIR_SET_DEF PORTB |= B1001;      // digitalWrite(11,8,HIGH)    // направление вперед
#define DEF_START_SPD_SET_DEF PORTB &= B11111001;  // digitalWrite(10,9,LOW)     // скорость 0

// УПРАВЛЕНИЕ КОЛЕСАМИ C++
// ЛЕВЫЕ И ПРАВЫЕ
#define DEF_FORWARD_DIR_DEF PORTB |= B1001;        // digitalWrite(11,8,HIGH)    // направление вперед
#define DEF_BACKWARD_DIR_DEF PORTB &= B11110110;   // digitalWrite(11,8,LOW)     // направление назад
#define DEF_SPD_H_DEF PORTB |= B0110;              // digitalWrite(10,9,HIGH)    // скорость 255
#define DEF_SPD_L_DEF PORTB &= B11111001;          // digitalWrite(10,9,LOW)     // скорость 0

// ЛЕВЫЕ
#define DEF_FORWARD_DIR_LEFT_DEF                    digitalWrite(11,HIGH)       // направление вперед левых
#define DEF_BACKWARD_DIR_LEFT_DEF                   digitalWrite(11,LOW)        // направление назад левых
#define DEF_SPD_H_LEFT_DEF                          digitalWrite(10,HIGH)       // скорость 255 левых
#define DEF_SPD_L_LEFT_DEF                          digitalWrite(10,LOW)        // скорость 0 левых

// ПРАВЫЕ
#define DEF_FORWARD_DIR_RIGHT_DEF                   digitalWrite(8,HIGH)        // направление вперед правых
#define DEF_BACKWARD_DIR_RIGHT_DEF                  digitalWrite(8,LOW)         // направление назад правых
#define DEF_SPD_H_RIGHT_DEF                         digitalWrite(9,HIGH)        // скорость 255 правых
#define DEF_SPD_L_RIGHT_DEF                         digitalWrite(9,LOW)         // скорость 0 правых

// ULTRASONIK ДАТЧИК РАССТОЯНИЯ
#define DEF_START_PIN_USTPO_CONFIG_DEF DDRD |= B10;        // pinMode(1,OUTPUT);  // установить как выход
#define DEF_START_PIN_USEPI_CONFIG_DEF DDRD &= B11111110;  // pinMode(0,INPUT);   // установить как вход
// КОНЕЦ КОНФИГА C++
          
// распиновка Arduino ProMini
const byte pin_dirLeft = 11;  // 11 pin направление левых колес 1↑ 0↓
const byte pin_dirRight = 8;  // 8 pin направление правых колес 1↑ 0↓
const byte pin_pwmLeft = 10;  // 10 pin скорость левых колес ШИМ 0-255
const byte pin_pwmRight = 9;  // 9 pin скорость правых колес ШИМ 0-255
const byte pin_voltBat  = 4;  // A4 pin вольтметр батареи

const byte trigPin = 1;       // ultrasonik
const byte echoPin = 0;       // ultrasonik

// переменные управленияи пинами
boolean dirLeft = HIGH;       // направление левых колес 1↑ 0↓ HIGH or LOW
boolean dirRight = HIGH;      // направление правых колес 1↑ 0↓ HIGH or LOW
byte pwmLeft = 0;             // скорость левых колес ШИМ 0-255
byte pwmRight = 0;            // скорость правых колес ШИМ 0-255
boolean digitalLeft = LOW;    // разрешение движения слева 1↑↓ 0x применяется к пину pin_pwmLeft     // alternative
boolean digitalRight = LOW;   // разрешение движения справа 1↑↓ 0x применяется к пину pin_pwmRight   // alternative

// переменные управления
boolean enMove = false;       // разрешение движения

// переменные для вычислений
long duration, cm, lcm;       // ultrasonik

// переменные вольтметра
int analogVal = 0;  // значение analogRead(pin_voltBat)
int voltVal = 0;    // значение после мап 0,123,0,500 приведение к 5вольтам. 500 для большего разрешения. напр: 3.7v = 370

int testBat(){  // функция теста напряжения батареи, возвращает значение от 0 до 500, 0-5 вольт; Li-ion 370-420, 3.7-4.2v;

	analogVal = analogRead(pin_voltBat);
    voltVal = map(analogVal,0,1023,0,500);

    return voltVal;
}

int dist(){  // функция проверки расстояния пред сенсором, возвращает количество см до препятствия

	digitalWrite(trigPin, LOW); 
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10); 
	digitalWrite(trigPin, LOW);
	
	duration = pulseIn(echoPin, HIGH); 
	cm = duration * 1.7 * 0.01 +1;

	delay (10);

	return cm;
}

void setup() {

	DEF_START_PIN_CONFIG_DEF;  // установка управляющих пинов (8,9,10,11) на выход = output = 1;
	DEF_START_DIR_SET_DEF;     // стартовая установка направление вперед. 11,8 = 1;
	DEF_START_SPD_SET_DEF;     // стартовая установка скорость ноль. 10,9 = 0;

	DEF_START_PIN_USTPO_CONFIG_DEF DDRD;  // pinMode(1,OUTPUT);  // установить 1 как выход // ultrasonik
	DEF_START_PIN_USEPI_CONFIG_DEF DDRD;  // pinMode(0,INPUT);   // установить 0 как вход  // ultrasonik
  
}

void loop() {

}









