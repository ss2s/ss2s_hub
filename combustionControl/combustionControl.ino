// Контроль горения. Контроллер горения твердотопливного котла сервозаслонкой + термостат электрокотла
// 4 кнопки: "растопка", "ВКЛ. ВЫКЛ. электро термостат", "т2 +1градус", "т2 -1градус". 

// КОДЫ ОШИБОК АВАРИИ:

// AVARIA 1           превышение температуры t1
// AVARIA 201         0.8 t1

// AVARIA 2           превышение температуры peregrevT2 (если термостат в аварии 2 типа активен. настройка переходов аварии на строках 330-335)
// AVARIA 202         превышение температуры t2

// AVARIA 3           превышение температуры t3
// AVARIA 203         0.8 t3

// AVARIA 4           превышение температуры t1
// AVARIA 204         0.8 t4

// AVARIA 5           превышение давления P1 (если термостат в аварии 2 типа активен)
// AVARIA 205         0.8 P1

// AVARIA 2 1         превышение давления P1 (может быть вызвано проседанием напряжения)
// AVARIA 2 201       0.8 P1   

// AVARIA 2 2         превышение температуры peregrevT2
// AVARIA 2 202       превышение температуры t2

// РАСПИНОВКА: /////////////////////////////////////////////////////////////////////////////////////////////////////

// термопара t1 выхлопные газы
#define T1_VIHLOP_TERMOPARA_SENSOR_SO_PIN 2   // термопара SO
#define T1_VIHLOP_TERMOPARA_SENSOR_CS_PIN 3   // термопара CS
#define T1_VIHLOP_TERMOPARA_SENSOR_SCK_PIN 4  // термопара SCK
// теплоноситель t2
#define T2_TEPLONOSITEL_SENSOR_PIN A0  // термометр теплоносителя 14
// термозащита низ t3 термопара t3 термометр защиты трубы низ
#define T3_SAFE_DOWN_TERMOPARA_SENSOR_SO_PIN 5
#define T3_SAFE_DOWN_TERMOPARA_SENSOR_CS_PIN 6
#define T3_SAFE_DOWN_TERMOPARA_SENSOR_SCK_PIN 7
// термозащита верх t4 термопара t4 термометр защиты трубы верх
#define T4_SAFE_UP_TERMOPARA_SENSOR_SO_PIN 8
#define T4_SAFE_UP_TERMOPARA_SENSOR_CS_PIN 9
#define T4_SAFE_UP_TERMOPARA_SENSOR_SCK_PIN 10
// датчик давления
#define P1_PRESSURE_SENSOR_PIN A1       // датчик давления 15
// Бипер
#define Q1_SIGNAL_BIPER_PIN 11          // сигнальный бипер
// Заслонка
#define S1_ZASLONKA_SERVO_PIN 12        // заслонка серво
// Котел
#define R1_KOTEL_RELE_PIN 13            // реле для подключения электро котла
// кнопки
#define B1_BUTTONS_PIN A2               // кнопки управления 16
// светодиоды
#define L1_RED_PIN 1
#define L1_GREEN_PIN 17                 // A3

// НАСТРОЙКИ: //////////////////////////////////////////////////////////////////////////////////////////////////////

// установленная температура MAX
int t2TeplonositelSetMaxTemp = 60;                 // желаемая температура теплоносителя
#define t1VihlopTermoparaSetMaxTemp 800            // максимальная температура выхлопных газов
#define t3SafeDownTermoparaSetMaxTemp 250          // максимальная температура дымохода НИЗ
#define t4SafeUpTermoparaSetMaxTemp 250            // максимальная температура дымохода ВЕРХ
#define gisterezisT2 1                             // +1 -1  гистерезис т2 (59-61)
#define peregrevT2 95                              // критическая температура t2

#define p1SetMaxPressure 10                         // максимально допустимое давление

#define COEF_VIHLOPA_RASTOPKI 4  // во столько раз меньше должна быть температура выхлопных газов для выхода из растопки
#define COEF_BEZOPASNOSTI 0.8    // коэфициент для сработки датчиков безопасности

#define T1_PLUS_COEF t1VihlopTermoparaSetMaxTemp * COEF_BEZOPASNOSTI
#define T3_PLUS_COEF t3SafeDownTermoparaSetMaxTemp * COEF_BEZOPASNOSTI
#define T4_PLUS_COEF t4SafeUpTermoparaSetMaxTemp * COEF_BEZOPASNOSTI
#define P1_PLUS_COEF p1SetMaxPressure * COEF_BEZOPASNOSTI

bool termostatEnable = 1;                     // разрешение использовать электро котел

// предельные углы серво: servoAngle0 - закрыто, servoAngle4 - открыто.
#define servoAngle0 10    // угол закрытого серво
#define servoAngle4 170  // угол полностью открытого серво

#define BEEPER_TIP 0     // тип бипера пищалки, 1 активный. 0 пасивный
#define SIGNAL_DL 1000   // длительность сигнала
#define SIGNAL_PS 1000   // пауза между сигналами
#define SIGNAL_TON 4000  // тон сигнала в герцах

#define AVARIYNIY_TERMOSTAT 1  // 1 при аварии ПЕРВОГО типа включится электрокотол. 0 при аварии не вкл котел
#define AVARIYNIY_TERMOSTAT_AV2 0  // 1 при аварии ВТОРОГО типа включится электрокотол. 0 при аварии не вкл котел
#define OJIDANIE_TERMOSTATA_AVARII 6000  // задержка до вкл термостата при аварии в миллисекундах

#define VREMYA_ZATUHANIA_S 600  // время затухания в секундах. до перехода в режим термостат и закрытия заслонки

// КОНЕЦ НАСТРОЕК //////////////////////////////////////////////////////////////////////////////////////////////////









#include "Servo.h"
Servo myservo;

#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27, 16, 2); // установка адреса 0x27 и разрешения 16/2

#include "max6675.h"     // https://github.com/adafruit/MAX6675-library

MAX6675 thermocouple1(T1_VIHLOP_TERMOPARA_SENSOR_SCK_PIN, T1_VIHLOP_TERMOPARA_SENSOR_CS_PIN, T1_VIHLOP_TERMOPARA_SENSOR_SO_PIN);
MAX6675 thermocouple3(T3_SAFE_DOWN_TERMOPARA_SENSOR_SCK_PIN, T3_SAFE_DOWN_TERMOPARA_SENSOR_CS_PIN, T3_SAFE_DOWN_TERMOPARA_SENSOR_SO_PIN);
MAX6675 thermocouple4(T4_SAFE_UP_TERMOPARA_SENSOR_SCK_PIN, T4_SAFE_UP_TERMOPARA_SENSOR_CS_PIN, T4_SAFE_UP_TERMOPARA_SENSOR_SO_PIN);

// текущая температура
int t1VihlopTermoparaCurentTemp = 0;
int t2TeplonositelCurentTemp = 0;
int t3SafeDownTermoparaCurentTemp = 0;
int t4SafeUpTermoparaCurentTemp = 0;
long p1CurentPressure = 0;
// предыдущая температура
int t1VihlopTermoparaPreviousTemp = 0;
int t2TeplonositelPreviousTemp = 0;
int t3SafeDownTermoparaPreviousTemp = 0;
int t4SafeUpTermoparaPreviousTemp = 0;
int p1PreviousPressure = 0;


// промежуточные углы серво
byte servoAngle2 = servoAngle4 / 2;
byte servoAngle1 = servoAngle4 / 4;
byte servoAngle3 = servoAngle1 * 3;

int servoStatePlus1 = 2;

// пользовательские символы:
byte customCharGradus0[8] = {  // gradus
	0b01100,
	0b10010,
	0b10010,
	0b01100,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

byte customCharT1[8] = {  // t1
	0b00100,
	0b00100,
	0b00110,
	0b01110,
	0b01111,
	0b11011,
	0b11001,
	0b10001
};

byte customCharT2[8] = {  // t2
	0b00100,
	0b01110,
	0b00100,
	0b00110,
	0b00000,
	0b01010,
	0b01010,
	0b01010
};

byte customCharT3[8] = {  // t3
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b11001,
	0b11001,
	0b10001
};

byte customCharT4[8] = {  // t4
	0b10001,
	0b11001,
	0b11001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001
};

void myBeeper(int beep){
	for(int i=0; i<beep; i++){
	    if(BEEPER_TIP > 0){
	    	digitalWrite(Q1_SIGNAL_BIPER_PIN, HIGH);
	    }
	    else{
	    	tone(Q1_SIGNAL_BIPER_PIN, SIGNAL_TON);
	    }

	    delay(SIGNAL_DL);
	    if(beep >= 4){delay(SIGNAL_DL);}

	    if(BEEPER_TIP > 0){
	    	digitalWrite(Q1_SIGNAL_BIPER_PIN, LOW);
	    }
	    else{
	    	noTone(Q1_SIGNAL_BIPER_PIN);
	    	digitalWrite(Q1_SIGNAL_BIPER_PIN, HIGH);
	    }

	    delay(SIGNAL_PS);
	}
}

// проверка температуры Т1
void t1VihlopTermoparaCheckTemp(){
	t1VihlopTermoparaPreviousTemp = t1VihlopTermoparaCurentTemp;
	t1VihlopTermoparaCurentTemp = thermocouple1.readCelsius();
}
// проверка температуры Т2 NTC
#define RT0_NTC 10000   // Ом сопртивление терморезистора при 25 гр С
#define BB_NTC 3977      // B константа
#define VCC_NTC 5       //Напряжение питания 5V
#define RR_NTC 10000     //R=10 КОм сопротивление подтягывающего резистора
//переменные
float RT_NTC, VR_NTC, ln_NTC, TX_NTC, VRT_NTC;
float T0_NTC = 298.15;  //Температура T0 из даташита, преобразуем из цельсиев в кельвины 25 + 273.15
void t2TeplonositelCheckTemp(){  // sys
  	VRT_NTC = analogRead(T2_TEPLONOSITEL_SENSOR_PIN);      //Считываем аналоговое значение VRT_NTC
  	VRT_NTC = (5.00 / 1023.00) * VRT_NTC;      //Преобразуем в напряжение
  	VR_NTC = VCC_NTC - VRT_NTC;
  	RT_NTC = VRT_NTC / (VR_NTC / RR_NTC);               //Сопротивление RT
  	ln_NTC = log(RT_NTC / RT0_NTC);
  	TX_NTC = (1 / ((ln_NTC / BB_NTC) + (1 / T0_NTC)));  //Температура с термистора
  	TX_NTC = TX_NTC - 273.15;                  //Преобразуем в цельсии

  	t2TeplonositelPreviousTemp = t2TeplonositelCurentTemp;
  	t2TeplonositelCurentTemp = TX_NTC;
}
// проверка температуры Т3
void t3SafeDownTermoparaCheckTemp(){
	t3SafeDownTermoparaPreviousTemp = t3SafeDownTermoparaCurentTemp;
	t3SafeDownTermoparaCurentTemp = thermocouple3.readCelsius();
}
// проверка температуры Т4
void t4SafeUpTermoparaCheckTemp(){
	t4SafeUpTermoparaPreviousTemp = t4SafeUpTermoparaCurentTemp;
	t4SafeUpTermoparaCurentTemp = thermocouple4.readCelsius();
}
// проверка давления Р1
void p1CheckPressure(){
	p1CurentPressure = analogRead(P1_PRESSURE_SENSOR_PIN);
	p1CurentPressure = p1CurentPressure * 100 / 2046;
	if(p1CurentPressure < 5){p1CurentPressure = -1;return;}
	else if(p1CurentPressure > 45){p1CurentPressure = 13;return;}
	p1CurentPressure = map(p1CurentPressure,5,45,0,120);
	p1CurentPressure = p1CurentPressure / 10;

	p1PreviousPressure = p1CurentPressure;

	//p1CurentPressure = 7;
}

// проверка всех температур и давления
void FullSensorCheck(){
	t1VihlopTermoparaCheckTemp();
	t2TeplonositelCheckTemp();
	t3SafeDownTermoparaCheckTemp();
	t4SafeUpTermoparaCheckTemp();
	p1CheckPressure();
}

void lcdDrawSensorVal(){  // функция вывода показаний датчиков на экран
	FullSensorCheck();

	lcd.setCursor(0,0);
	lcd.write(byte(1));  // t1
	lcd.print(t1VihlopTermoparaCurentTemp);
	lcd.write(byte(0));  // gradus
	lcd.print("  ");

	lcd.setCursor(6,0);
	lcd.write(byte(3));  // t3
	lcd.print(t3SafeDownTermoparaCurentTemp);
	lcd.write(byte(0));  // gradus
	lcd.print("  ");
	
	lcd.setCursor(11,0);
	lcd.write(byte(4));  // t4
	lcd.print(t4SafeUpTermoparaCurentTemp);
	lcd.write(byte(0));  // gradus
	lcd.print("  ");

	lcd.setCursor(0,1);
	lcd.write(byte(2));  // t2
	lcd.print(t2TeplonositelCurentTemp);
	lcd.write(byte(0));  // gradus
	lcd.print(" ");

	lcd.setCursor(5,1);
	lcd.print("P");  // P1
	lcd.print(p1CurentPressure);
	lcd.print(" ");

	lcd.setCursor(8,1);
	lcd.print("W");
	if(termostatEnable){
		lcd.print(digitalRead(R1_KOTEL_RELE_PIN));
	}
	lcd.print("  ");

	lcd.setCursor(11,1);
	lcd.print("S");
	lcd.print(servoStatePlus1);
	lcd.print("  ");

	lcd.setCursor(14,1);
	lcd.print(t2TeplonositelSetMaxTemp);
	lcd.print(" ");

}

bool avariaALARM(byte tip){

	digitalWrite(L1_GREEN_PIN, LOW);
	digitalWrite(L1_RED_PIN, HIGH);

	int avariaVal = 0;
	if(tip == 1 || tip == 201){avariaVal = t1VihlopTermoparaCurentTemp;}
	else if(tip == 2 || tip == 202){avariaVal = t2TeplonositelCurentTemp;}
	else if(tip == 3 || tip == 203){avariaVal = t3SafeDownTermoparaCurentTemp;}
	else if(tip == 4 || tip == 204){avariaVal = t4SafeUpTermoparaCurentTemp;}
	else if(tip == 5 || tip == 205){avariaVal = p1CurentPressure;}

	myservo.write(servoAngle0);
	servoStatePlus1 = 0;
	lcd.clear();
	lcd.print("AVARIA ");
	lcd.print(tip);
	lcd.print(" ");
	lcd.print(avariaVal);

	myBeeper(4);

	delay(OJIDANIE_TERMOSTATA_AVARII);
	while(1){
		
		if(AVARIYNIY_TERMOSTAT > 0){
			termostatElectroKotla();
			lcd.setCursor(0,0);
			lcd.print("                ");
			lcd.setCursor(0,0);
			lcd.print("AVARIA ");
			lcd.print(tip);
			lcd.print(" ");
			lcd.print(avariaVal);

			myBeeper(4);

			delay(2000);
			digitalWrite(L1_RED_PIN, HIGH);
			delay(2000);
			digitalWrite(L1_RED_PIN, LOW);
			delay(2000);
			digitalWrite(L1_RED_PIN, HIGH);
			delay(2000);
			digitalWrite(L1_RED_PIN, LOW);
			delay(2000);
			digitalWrite(L1_RED_PIN, HIGH);
		}
	}
}

bool avariaALARM2(byte tip){
	digitalWrite(L1_GREEN_PIN, LOW);
	digitalWrite(L1_RED_PIN, HIGH);
	int avariaVal = 0;
	if(tip == 1 || tip == 201){avariaVal = p1CurentPressure;}
	else if(tip == 2 || tip == 202){avariaVal = t2TeplonositelCurentTemp;}
	myservo.write(servoAngle0);
	servoStatePlus1 = 0;
	digitalWrite(R1_KOTEL_RELE_PIN, LOW);
	lcd.clear();
	lcd.print("  AVARIA 2 ");
	lcd.print(tip);
	lcd.setCursor(0,1);
	lcd.print("     ");
	lcd.print(avariaVal);
	myBeeper(5);
	if(AVARIYNIY_TERMOSTAT_AV2 > 0){
		if(tip == 0){}
		else if(tip == 1){avariaALARM(5);}
		else if(tip == 201){avariaALARM(205);}
		else if(tip == 2 ){avariaALARM(2);}
		else if(tip == 202 ){avariaALARM(202);}
	}
	while(1){
		delay(10000);
		myBeeper(5);
	}
}

bool rastopkaPechi(){

	myBeeper(1);

	digitalWrite(R1_KOTEL_RELE_PIN, LOW);

	digitalWrite(L1_GREEN_PIN, HIGH);
	digitalWrite(L1_RED_PIN, LOW);

	bool ledState = 0;
	byte safeCounter = 0;
	FullSensorCheck();
	myservo.write(servoAngle4);
	servoStatePlus1 = 4;

	while((t2TeplonositelCurentTemp <= t2TeplonositelSetMaxTemp) 
	|| (t1VihlopTermoparaCurentTemp < (t1VihlopTermoparaSetMaxTemp / COEF_VIHLOPA_RASTOPKI)))
	{
		lcdDrawSensorVal();
		// проверка датчиков безопасности
		if(t1VihlopTermoparaCurentTemp > T1_PLUS_COEF 
		|| t3SafeDownTermoparaCurentTemp > T3_PLUS_COEF 
		|| t4SafeUpTermoparaCurentTemp > T4_PLUS_COEF 
		|| p1CurentPressure > P1_PLUS_COEF 
		|| t2TeplonositelCurentTemp > (t2TeplonositelSetMaxTemp + gisterezisT2))
		{
			if(safeCounter < 20){
				myservo.write(servoAngle3);
				servoStatePlus1 = 3;
			}
			else if(safeCounter < 40){
				myservo.write(servoAngle2);
				servoStatePlus1 = 2;
			}
			else if(safeCounter < 60){
				myservo.write(servoAngle1);
				servoStatePlus1 = 1;
			}
			else if(safeCounter < 100){
				myservo.write(servoAngle0);
				servoStatePlus1 = 0;
			}
			else if(safeCounter < 200){
				myservo.write(servoAngle0);
				servoStatePlus1 = 0;
				if(t1VihlopTermoparaCurentTemp > T1_PLUS_COEF){ 
					avariaALARM(201);
				}
				else if(t3SafeDownTermoparaCurentTemp > T3_PLUS_COEF){
					avariaALARM(203);
				}
				else if(t4SafeUpTermoparaCurentTemp > T4_PLUS_COEF){
					avariaALARM(204);
				}
				else if(p1CurentPressure > P1_PLUS_COEF){
					avariaALARM2(201);
				}
				else if(t2TeplonositelCurentTemp > (t2TeplonositelSetMaxTemp + gisterezisT2)){
					avariaALARM2(202);
				}
				else{
					avariaALARM(200);
				}
			}

			safeCounter ++;
		}
		else
		{
			myservo.write(servoAngle4);
			servoStatePlus1 = 4;
			safeCounter = 0;
		}

		if(t1VihlopTermoparaCurentTemp >= t1VihlopTermoparaSetMaxTemp 
		|| t3SafeDownTermoparaCurentTemp >= t3SafeDownTermoparaSetMaxTemp 
		|| t4SafeUpTermoparaCurentTemp >= t4SafeUpTermoparaSetMaxTemp 
		|| p1CurentPressure >= p1SetMaxPressure 
		|| t2TeplonositelCurentTemp >= peregrevT2)
		{
			if(t1VihlopTermoparaCurentTemp >= t1VihlopTermoparaSetMaxTemp){ 
				avariaALARM(1);
			}
			else if(t3SafeDownTermoparaCurentTemp >= t3SafeDownTermoparaSetMaxTemp){
				avariaALARM(3);
			}
			else if(t4SafeUpTermoparaCurentTemp >= t4SafeUpTermoparaSetMaxTemp){
				avariaALARM(4);
			}
			else if(p1CurentPressure >= p1SetMaxPressure){
				avariaALARM2(1);
			}
			else if(t2TeplonositelCurentTemp >= peregrevT2){
				avariaALARM2(2);
			}
			else{
				avariaALARM(0);
			}
		}


		// delay(10000);
		for(int i=0; i<10; i++){
			zaderjkaSecG(1);
			digitalWrite(L1_GREEN_PIN, ledState);
			ledState = !ledState;
		}
		//lcd.clear();delay(1000);lcd.print("test");delay(1000);  // test
	}
}

bool osnovnoyCiklGoreniaPechi(){

	myBeeper(2);

	digitalWrite(L1_GREEN_PIN, HIGH);
	digitalWrite(L1_RED_PIN, LOW);

	byte zth = 0;
	byte safeCounter = 0;
	byte safeCounter2 = 0;
	bool servoStateFlag = 0;
	FullSensorCheck();
	servoStatePlus1 = 2;
	while(1){
		lcdDrawSensorVal();

		// проверка датчиков безопасности
		if(t1VihlopTermoparaCurentTemp > T1_PLUS_COEF 
		|| t3SafeDownTermoparaCurentTemp > T3_PLUS_COEF 
		|| t4SafeUpTermoparaCurentTemp > T4_PLUS_COEF 
		|| p1CurentPressure > P1_PLUS_COEF 
		|| t2TeplonositelCurentTemp > (peregrevT2 * COEF_BEZOPASNOSTI) 
		|| t2TeplonositelCurentTemp > (t2TeplonositelSetMaxTemp + gisterezisT2))
		{
			if(safeCounter < 10 && servoStateFlag == 0){
				servoStatePlus1 --;

			}
			else if(safeCounter < 20 && servoStateFlag == 0){
				servoStatePlus1 --;
			}
			else if(safeCounter < 30 && servoStateFlag == 0){
				servoStatePlus1 --;
			}
			else if(safeCounter < 60 && servoStateFlag == 0){
				servoStatePlus1 --;
			}
			else if(safeCounter == 10
			|| safeCounter == 20
			|| safeCounter == 30)
				{
					servoStateFlag = 0;
				}

			safeCounter ++;
			safeCounter2 = 0;
		}
		else if(t2TeplonositelCurentTemp <= (t2TeplonositelSetMaxTemp - gisterezisT2))
		{
			servoStateFlag = 0;
			safeCounter = 0;
			if(safeCounter2 == 0){
				servoStatePlus1 ++;
			}
			safeCounter2 ++;
			if(safeCounter2 >= 3){  // ускорение открытия серво
			    safeCounter2 = 0;
			}
		}
		
		switch (servoStatePlus1) {
		    case 5:
		    servoStatePlus1 = 4;
		    myservo.write(servoAngle4);
		    break;
		    case 4:
		    myservo.write(servoAngle4);
		    break;
		    case 3:
		    myservo.write(servoAngle3);
		    break;
		    case 2:
		    myservo.write(servoAngle2);
		    break;
		    case 1:
		    myservo.write(servoAngle1);
		    break;
		    case 0:
		    myservo.write(servoAngle0);
		    break;
		    case -1:
		    servoStatePlus1 = 0;
		    myservo.write(servoAngle0);
		    break;
		}

		if(t1VihlopTermoparaCurentTemp >= t1VihlopTermoparaSetMaxTemp 
		|| t3SafeDownTermoparaCurentTemp >= t3SafeDownTermoparaSetMaxTemp 
		|| t4SafeUpTermoparaCurentTemp >= t4SafeUpTermoparaSetMaxTemp 
		|| p1CurentPressure >= p1SetMaxPressure 
		|| t2TeplonositelCurentTemp >= peregrevT2)
		{
			avariaALARM(0);
		}

		if(t2TeplonositelCurentTemp < (t2TeplonositelSetMaxTemp - gisterezisT2) 
		&& t1VihlopTermoparaCurentTemp <= t1VihlopTermoparaPreviousTemp 
		&& servoStatePlus1 == 4)
		{
			zth ++;
		}
		else
		{
			zth = 0;
		}

		if(zth >= 10){return;}

		// delay(10000);
		zaderjkaSecG(10);
	}
}

bool zatuhahiePechi(){

	myBeeper(3);

	bool ledState = 0;
	myservo.write(servoAngle1);
	servoStatePlus1 = 1;

	for(int i=0; i<VREMYA_ZATUHANIA_S; i++){  // 10 minut
		digitalWrite(L1_GREEN_PIN, ledState);
		digitalWrite(L1_RED_PIN, ledState);
		ledState = !ledState;
	    termostatElectroKotla();
	    if(zaderjkaSecY(1)){return 1;}
	}

	return 0;
}

bool termostatElectroKotla(){

	t2TeplonositelCheckTemp();
	p1CheckPressure();

	if(termostatEnable && (t2TeplonositelCurentTemp <=(t2TeplonositelSetMaxTemp - gisterezisT2)) && p1CurentPressure < p1SetMaxPressure){
		digitalWrite(R1_KOTEL_RELE_PIN, HIGH);
	}
	else if(!termostatEnable || (t2TeplonositelCurentTemp > (t2TeplonositelSetMaxTemp + gisterezisT2)) || p1CurentPressure >= p1SetMaxPressure){
		digitalWrite(R1_KOTEL_RELE_PIN, LOW);
	}

	lcdDrawSensorVal();
}

void glavniyCikl(){
	myservo.write(servoAngle0);
	servoStatePlus1 = 0;

	digitalWrite(L1_GREEN_PIN, HIGH);
	digitalWrite(L1_RED_PIN, HIGH);

	while(1){
		termostatElectroKotla();
		if(zaderjkaSecY(10)){return 1;}
	}
}

// функция проверки кнопок
byte key(){
	int val = analogRead(B1_BUTTONS_PIN);
	byte keyVal = 0;
	if(val <= 254) keyVal = 1;              // rastopka            1v=204.6
	else if(val <= 459) keyVal = 2;         // termostate enable   2v=409.2
	else if(val <= 663) keyVal = 3;         // up                  3v=613.8
	else if(val <= 868) keyVal = 4;         // down                4v=818.4
	else if(val <= 1023) keyVal = 0;        // no press            5v=1023

	// lcd.setCursor(0,0);
	// lcd.print(keyVal);
	// lcd.print(" = ");
	// lcd.print(val);
	// lcd.print("         ");

	return keyVal;
}

bool buttonChekForYelowloop(){  // обработчик нажатий кнопок в цикле Yelowloop
	
	unsigned int buttonDelay = 150;  // задержка для меню
	unsigned int buttonDelayIn = 50;  // задержка для меню
	byte valB = key();
	byte prevValB = valB;

	if(valB == 1){  // rastopka
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				rastopkaPechi();
				osnovnoyCiklGoreniaPechi();
				zatuhahiePechi();
				return 1;
		}
	}else if(valB == 2){  // termostatEnable
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				termostatEnable = !termostatEnable;
		}
	}else if(valB == 3){  // up
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				t2TeplonositelSetMaxTemp ++;
				if(t2TeplonositelSetMaxTemp > (peregrevT2 - 5)){t2TeplonositelSetMaxTemp = peregrevT2 - 5;}
		}
	}
	else if(valB == 4){  // down
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				t2TeplonositelSetMaxTemp --;
				if(t2TeplonositelSetMaxTemp < 0){t2TeplonositelSetMaxTemp = 0;}
		}
	}

	if(valB > 0){
		lcdDrawSensorVal();
		delay(buttonDelay);
	}
	return 0;
}

bool buttonChekForGreenloop(){  // обработчик нажатий кнопок в цикле Greenloop
	
	unsigned int buttonDelay = 150;  // задержка для меню
	unsigned int buttonDelayIn = 50;  // задержка для меню
	byte valB = key();
	byte prevValB = valB;

	if(valB == 1){  // rastopka
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				// rastopkaPechi();
				// osnovnoyCiklGoreniaPechi();
				// zatuhahiePechi();
				return 1;
		}
	}else if(valB == 2){  // termostatEnable
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				termostatEnable = !termostatEnable;
		}
	}else if(valB == 3){  // up
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				t2TeplonositelSetMaxTemp ++;
				if(t2TeplonositelSetMaxTemp > (peregrevT2 - 5)){t2TeplonositelSetMaxTemp = peregrevT2 - 5;}
		}
	}
	else if(valB == 4){  // down
		delay(buttonDelayIn);
		valB = key();
		if(valB == prevValB){
				t2TeplonositelSetMaxTemp --;
				if(t2TeplonositelSetMaxTemp < 0){t2TeplonositelSetMaxTemp = 0;}
		}
	}

	if(valB > 0){
		lcdDrawSensorVal();
		delay(buttonDelay);
	}
	return 0;
}

bool zaderjkaSecY(int zdr){
	int ciklCount = zdr * 100;
	for(int i=0; i<=ciklCount; i++){
		if(buttonChekForYelowloop()){return 1;}
		delay(10);
	}
	return 0;
}
bool zaderjkaSecG(int zdr){
	int ciklCount = zdr * 100;
	for(int i=0; i<=ciklCount; i++){
		if(buttonChekForGreenloop()){return 1;}
		delay(10);
	}
	return 0;
}

void setup(){

	pinMode(Q1_SIGNAL_BIPER_PIN, OUTPUT);
	if(BEEPER_TIP == 0){digitalWrite(Q1_SIGNAL_BIPER_PIN, HIGH);}
	else{digitalWrite(Q1_SIGNAL_BIPER_PIN, LOW);}

	pinMode(R1_KOTEL_RELE_PIN, OUTPUT);
	digitalWrite(R1_KOTEL_RELE_PIN, LOW);

	// промежуточные углы серво
	if(servoAngle0 < servoAngle4){
		servoAngle1 = servoAngle4 / 4;
		servoAngle2 = servoAngle4 / 2;
		servoAngle3 = servoAngle4 / 4 * 3;
	}else{
		servoAngle1 = servoAngle0 / 4 * 3;
		servoAngle2 = servoAngle0 / 2;
		servoAngle3 = servoAngle0 / 4;
	}

	lcd.begin(); // иниализация дисплея LCD 16/2
	lcd.clear(); // очистить дисплей
	lcd.backlight(); // включение подсветки
	lcd.setCursor(0,0);
	lcd.print("   COMBUSTION");
	lcd.setCursor(0,1);
	lcd.print("    CONTROL");

	myservo.attach(S1_ZASLONKA_SERVO_PIN);
	myservo.write(servoAngle0);

	delay(1000);
	lcd.clear(); // очистить дисплей

	// инициализация пользовательских символов
  	lcd.createChar(0, customCharGradus0);
  	lcd.createChar(1, customCharT1);
  	lcd.createChar(2, customCharT2);
  	lcd.createChar(3, customCharT3);
  	lcd.createChar(4, customCharT4);

	pinMode(L1_RED_PIN, OUTPUT);
	digitalWrite(L1_RED_PIN, LOW);
	pinMode(L1_GREEN_PIN, OUTPUT);
	digitalWrite(L1_GREEN_PIN, LOW);
}

void loop(){
	glavniyCikl();
}
