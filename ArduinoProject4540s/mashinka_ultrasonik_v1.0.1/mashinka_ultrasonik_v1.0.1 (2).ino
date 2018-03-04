          /* ПРОШИВКА ДЛЯ МАШИНКИ v1.0.3 Arduino ProMini with UNO bootloader */

// распиновка Arduino ProMini
#define pin_dirLeft  11  // 11 pin направление левых колес 1↑ 0↓
#define pin_dirRight  8  // 8 pin направление правых колес 1↑ 0↓
#define pin_pwmLeft  10  // 10 pin скорость левых колес ШИМ 0-255
#define pin_pwmRight  9  // 9 pin скорость правых колес ШИМ 0-255
#define pin_voltBat   4  // A4 pin вольтметр батареи

#define trigPin  2       // ultrasonik
#define echoPin  3       // ultrasonik

// макросы для двигателей

// ДВИГАТЕЛИ
// СТАРТОВЫЙ КОНФИГ
#define DEF_START_PIN_CONFIG_DEF DDRB |= B1111    // pinMode(11,10,9,8,OUTPUT)  // установить как выход
#define DEF_START_DIR_SET_DEF PORTB |= B1001      // digitalWrite(11,8,HIGH)    // направление вперед
#define DEF_START_SPD_SET_DEF PORTB &= B11111001  // digitalWrite(10,9,LOW)     // скорость 0

// УПРАВЛЕНИЕ КОЛЕСАМИ

// ЛЕВЫЕ И ПРАВЫЕ
#define DEF_FORWARD_DIR_DEF PORTB |= B1001        // digitalWrite(11,8,HIGH)    // направление вперед
#define DEF_BACKWARD_DIR_DEF PORTB &= B11110110   // digitalWrite(11,8,LOW)     // направление назад
#define DEF_SPD_H_DEF PORTB |= B0110              // digitalWrite(10,9,HIGH)    // скорость 255
#define DEF_SPD_L_DEF PORTB &= B11111001          // digitalWrite(10,9,LOW)     // скорость 0

// ЛЕВЫЕ
#define DEF_FORWARD_DIR_LEFT_DEF PORTB |= 1<<3     // digitalWrite(11,HIGH)       // направление вперед левых***
#define DEF_BACKWARD_DIR_LEFT_DEF PORTB &= 0<<3    // digitalWrite(11,LOW)        // направление назад левых
#define DEF_SPD_H_LEFT_DEF PORTB |= 1<<2           // digitalWrite(10,HIGH)       // скорость 255 левых
#define DEF_SPD_L_LEFT_DEF PORTB &= 0<<2           // digitalWrite(10,LOW)        // скорость 0 левых

// ПРАВЫЕ
#define DEF_FORWARD_DIR_RIGHT_DEF PORTB |= B1 /*PORTB = 1<<0    //*/ // digitalWrite(8,HIGH)        // направление вперед правых
#define DEF_BACKWARD_DIR_RIGHT_DEF PORTB &= B11111110 /*PORTB = 0<<0   //*/ // digitalWrite(8,LOW)         // направление назад правых
#define DEF_SPD_H_RIGHT_DEF PORTB |= 1<<1          // digitalWrite(9,HIGH)        // скорость 255 правых
#define DEF_SPD_L_RIGHT_DEF PORTB &= 0<<1          // digitalWrite(9,LOW)         // скорость 0 правых

// ULTRASONIK ДАТЧИК РАССТОЯНИЯ
#define DEF_START_PIN_USTPO_CONFIG_DEF DDRD |= B10        // pinMode(1,OUTPUT)   // установить как выход
#define DEF_START_PIN_USEPI_CONFIG_DEF DDRD &= B11111110  // pinMode(0,INPUT)    // установить как вход
// конец макросов для двигателей

#define MIN_ROTATE_TIME 500
#define MAX_ROTATE_TIME 2000
#define MIN_BACKWARD_TIME 300
#define MAX_BACKWARD_TIME 800
          
// переменные управленияи пинами
boolean dirLeft = HIGH;       // направление левых колес 1↑ 0↓ HIGH or LOW
boolean dirRight = HIGH;      // направление правых колес 1↑ 0↓ HIGH or LOW
byte pwmLeft = 0;             // скорость левых колес ШИМ 0-255
byte pwmRight = 0;            // скорость правых колес ШИМ 0-255
boolean digitalLeft = LOW;    // разрешение движения слева 1↑↓ 0x применяется к пину pin_pwmLeft     // alternative
boolean digitalRight = LOW;   // разрешение движения справа 1↑↓ 0x применяется к пину pin_pwmRight   // alternative
byte minSpeed = 150;          // минимальная скорость движения

int stopDistance = 30;        // дистанция остановки

// переменные управления
boolean enMove = false;       // разрешение движения
int iSpd;
boolean RRflag = 0;
boolean ABflag = 0;

// переменные для вычислений ultrasonik
long duration, cm, lcm;       // ultrasonik

// переменные вольтметра
int analogVal = 0;  // значение analogRead(pin_voltBat)
int voltVal = 0;    // значение после мап 0,123,0,500 приведение к 5вольтам. 500 для большего разрешения. напр: 3.7v = 370

int fBlink(int qnt = 1){
	int bQnt = qnt;
	for(int i=bQnt; i > 0; i --){	
		digitalWrite(LED_BUILTIN, HIGH);
  		delay(200);
  		digitalWrite(LED_BUILTIN, LOW);
  		delay(500);
	}
}

int fExtBlink(int qnt = 1, int dl1 = 200, int dl2 = 500){
	int bQnt = qnt, bDl1 = dl1, bDl2 = dl2;
	for(int i=bQnt; i > 0; i --){
		digitalWrite(LED_BUILTIN, HIGH);
  		delay(bDl1);
  		digitalWrite(LED_BUILTIN, LOW);
  		delay(bDl2);
	}
}

int fExtBeeper(int qnt = 1, int dl1 = 200, int dl2 = 500, byte fpwm = 50){
	int bQnt = qnt, bDl1 = dl1, bDl2 = dl2;
	byte bFpwm = fpwm;
	for(int i=bQnt; i > 0; i --){
		analogWrite(pin_pwmLeft, bFpwm);
		analogWrite(pin_pwmRight, bFpwm);
  		delay(bDl1);
  		digitalWrite(pin_pwmLeft, LOW);
  		digitalWrite(pin_pwmRight, LOW);
  		delay(bDl2);
	}
}

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

int blinkMah(){
	// int BMdist = dist();
	int BMvolt;
	int BMBdel = 500;
	boolean ciklControl = 1;
	boolean ciklControl2 = 1;
	boolean ciklControl3 = 1;

	DEF_SPD_L_DEF;
	delay(500);

	BMvolt = testBat();
	Serial.println(BMvolt);
	if(BMvolt <= 370){
		fExtBlink(1, BMBdel, BMBdel);
	}else if((BMvolt > 370) && (BMvolt <= 380)){
		fExtBlink(2, BMBdel, BMBdel);
	}else if((BMvolt > 380) && (BMvolt <= 390)){
		fExtBlink(3, BMBdel, BMBdel);
	}else if((BMvolt > 390) && (BMvolt <= 400)){
		fExtBlink(4, BMBdel, BMBdel);
	}else if((BMvolt > 400) && (BMvolt <= 415)){
		fExtBlink(5, BMBdel, BMBdel);
	}else if(BMvolt > 415){
		while(ciklControl){
			BMvolt = testBat();
			Serial.println(BMvolt);
			if(BMvolt <= 412){
				ciklControl = 0;
				blinkMah();
			}else if((BMvolt > 412 ) && (BMvolt <= 440)){
				digitalWrite(LED_BUILTIN, HIGH);
				while(ciklControl2){
					BMvolt = testBat();
					Serial.println(BMvolt);
					if((BMvolt <= 412) || (BMvolt > 440)){
						// ciklControl2 = 0;
						blinkMah();
						digitalWrite(LED_BUILTIN, LOW);
					}
					delay(5000);
				}
			}else if(BMvolt > 440){
				while(ciklControl3){
					BMvolt = testBat();
					Serial.println(BMvolt);
					if(BMvolt <= 440){
						// ciklControl3 = 0;
						blinkMah();
						delay(3000);
					}else{
						fExtBlink(10, BMBdel, BMBdel);
						delay(500);
						fExtBeeper(5, BMBdel, BMBdel);
						delay(500);
					}


					// digitalWrite(LED_BUILTIN, HIGH);
					// delay(500);
					// digitalWrite(LED_BUILTIN, LOW);
					// delay(500);
				}
			}
		}
	}
}

void mashinaRider(int dir = 1, int spd = 1){
	int rDir = dir;
	int rSpd = spd;
	if(rDir == 1){
		DEF_FORWARD_DIR_DEF;
	}else{
		DEF_BACKWARD_DIR_DEF;
	}

	if(rSpd == 1){
		DEF_SPD_H_DEF;
	}else if(rSpd == 0){
		DEF_SPD_L_DEF;
	}else if(rSpd > 1){
		if(rSpd < minSpeed){
			rSpd = minSpeed;
		}else if(rSpd > 255){
			rSpd = 255;
		}
		analogWrite(pin_pwmLeft, rSpd);
		analogWrite(pin_pwmRight, rSpd);
	}
}

void mashinaRotate(int dir = 0, int dur = 1000){
	int MRdir = dir;
	int MRdur = dur;
	if(dir == 0){
		DEF_BACKWARD_DIR_LEFT_DEF;
		DEF_FORWARD_DIR_RIGHT_DEF;
		DEF_SPD_H_DEF;
	}else if(dir == 1){
		DEF_FORWARD_DIR_LEFT_DEF;
		DEF_BACKWARD_DIR_RIGHT_DEF;
		DEF_SPD_H_DEF;
	}else{
		DEF_BACKWARD_DIR_DEF;
		DEF_SPD_H_DEF;
	}
	delay(dur);
}

void randRotate(){
	long randNumber1 = random(0, 3);
	long randNumber2 = 0;
	long randNumber3;
	if(randNumber1 < 2){
		randNumber2 = random(MIN_ROTATE_TIME, MAX_ROTATE_TIME);
		ABflag = 0;
	}else{
		if(ABflag == 0){
			randNumber2 = random(MIN_BACKWARD_TIME, MAX_BACKWARD_TIME);
			RRflag = 1;
			ABflag = 1;
		}
	}
	mashinaRotate(randNumber1,randNumber2);

	if(RRflag == 1){
		RRflag = 0;
		randNumber3 = random(0, 5);
		if(randNumber3 > 0){
			randRotate();
		}
	}
}

void robotRider(void){
	int distance = dist();
	int stopDistance2 = stopDistance + 10;
	int rrtSpd = 0;
	if(distance < 6){
		delay(50);
		distance = dist();
		if(distance < 6){
		blinkMah();
		}
	}else if(distance < stopDistance){
		rrtSpd = 0;
	}else if(distance >= (stopDistance2)){
		rrtSpd = 1;
	}
	mashinaRider(1, rrtSpd);
	if(rrtSpd == 0){
		delay(50);
		randRotate();
	}
}

void robotRiderTest(void){
	int distance = dist();
}

void setup() {

	DEF_START_PIN_CONFIG_DEF;  // установка управляющих пинов (8,9,10,11) на выход = output = 1;
	DEF_START_DIR_SET_DEF;     // стартовая установка направление вперед. 11,8 = 1;
	DEF_START_SPD_SET_DEF;     // стартовая установка скорость ноль. 10,9 = 0;

	DEF_START_PIN_USTPO_CONFIG_DEF;  // pinMode(1,OUTPUT);  // установить 1 как выход // ultrasonik
	DEF_START_PIN_USEPI_CONFIG_DEF;  // pinMode(0,INPUT);   // установить 0 как вход  // ultrasonik

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	randomSeed(analogRead(0));

	Serial.begin(9600);

	int Lvolt = testBat();
	if(voltVal > 412){
		delay(1000);
		fExtBeeper(15, 30, 30);
		blinkMah();
	}else{

		tone(pin_pwmLeft, 1000);
		delay(150);
		noTone(pin_pwmLeft);
		tone(pin_pwmRight, 2000);
		delay(150);
		noTone(pin_pwmRight);
		tone(pin_pwmLeft, 3000);
		delay(150);
		noTone(pin_pwmLeft);
	}
}

void loop() {

	robotRider();
	delay(50);
}









