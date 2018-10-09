// РАСПИНОВКА:
// термопара t1
#define T1_VERH_TERMOPARA_SENSOR_SO_PIN 2
#define T1_VERH_TERMOPARA_SENSOR_CS_PIN 3
#define T1_VERH_TERMOPARA_SENSOR_SCK_PIN 4
// термопара t2
#define T2_NIZ_TERMOPARA_SENSOR_SO_PIN 5
#define T2_NIZ_TERMOPARA_SENSOR_CS_PIN 6
#define T2_NIZ_TERMOPARA_SENSOR_SCK_PIN 7
// нагреватели
#define N1_VERH_NAGREV_PIN 8
#define N2_NIZ_NAGREV_PIN 9
// пищалка
#define Q1_BUZER_PIN 10
// кнопки
#define B1_PROFIL_PIN 15  // A1
#define B2_START_PIN 14   // A0
#define B3_STOP_PIN 17    // A2
#define B4_PAUSE_PIN 16   // A3

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double t1VerhSetTemp = 165;
double t2NizSetTemp = 170;
float t1VerhSetSpeed = 0.5;
float t2NizSetSpeed = 0.5;

double t1VerhCurrentTemp = 0;
double t2NizCurrentTemp = 0;

byte profilNumber = 0;

byte animCiklNumber = 0;

unsigned long animationSpeed = 67;

#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x3F, 20, 4); // установка адреса 0x3F и разрешения 20/4

#include "max6675.h"     // https://github.com/adafruit/MAX6675-library

MAX6675 thermocouple1(T1_VERH_TERMOPARA_SENSOR_SCK_PIN, T1_VERH_TERMOPARA_SENSOR_CS_PIN, T1_VERH_TERMOPARA_SENSOR_SO_PIN);
MAX6675 thermocouple2(T2_NIZ_TERMOPARA_SENSOR_SCK_PIN, T2_NIZ_TERMOPARA_SENSOR_CS_PIN, T2_NIZ_TERMOPARA_SENSOR_SO_PIN);



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

	byte customCharT1[8] = {  // h1
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111
	};

	byte customCharT2[8] = {  // h2
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	byte customCharT3[8] = {  // h3
		0b00000,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	byte customCharT4[8] = {  // h3
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
	};

	byte customCharT5[8] = {  // h3
		0b11111,
		0b11111,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000
	};

	byte customCharT6[8] = {  // h3
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b00000,
		0b00000,
		0b00000,
		0b00000
	};

	byte customCharT7[8] = {  // h3
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b00000,
		0b00000
	};

// time value
unsigned long secondVal = 0;
unsigned long millVal = 0;
unsigned long mill1000Val = 0;
unsigned long prewMilTime = 0;     // предыдущее значение миллис
unsigned long thisMilTime = 0;     // текущее значение миллис
void timeSecUpdate(){
	thisMilTime = millis();
	millVal = thisMilTime - prewMilTime;
	secondVal = (thisMilTime - prewMilTime) / 1000;
	mill1000Val = millVal - (secondVal * 1000);


	lcd.setCursor(5,2);
	lcd.print(secondVal);
	// lcd.print(mill1000Val);
	lcd.print(" ");
}
void startTimeSecLine(){
 	secondVal = 0;
 	prewMilTime = millis();
 	timeSecUpdate();
}

// проверка температуры T1 и Т2
void CheckTemp(){

	t1VerhCurrentTemp = thermocouple1.readCelsius();
	t2NizCurrentTemp = thermocouple2.readCelsius();

	lcd.setCursor(15,1);
	lcd.print(int(t1VerhCurrentTemp));
	lcd.print(" ");
	lcd.setCursor(18,1);
	lcd.write(byte(0));
	lcd.print("C");

	lcd.setCursor(15,2);
	lcd.print(int(t2NizCurrentTemp));
	lcd.print(" ");
	lcd.setCursor(18,2);
	lcd.write(byte(0));
	lcd.print("C");
}



// переменные для работы с функцией millis();
unsigned long startMill100 = 0;
unsigned long currentMill100 = 0;
bool mil500F(){
	//startMill100 = millis();

	currentMill100 = millis();
	if((currentMill100 - startMill100) >= 500){
		startMill100 = currentMill100;
		return 1;
	}
	return 0;
}

bool PIDnagrevatel(float tVerhSpeed, float tVerhSet, float tNizSpeed, float tNizSet){

	t1VerhSetTemp = tVerhSet;
	t2NizSetTemp = tNizSet;
	profilDraw(1);

	bool flagCheckTemp = 1;
	bool flagAnimatioF = 1;

	CheckTemp();
	double prevT1VerhCurrentTemp = t1VerhCurrentTemp;
	double prevT2NizCurrentTemp = t2NizCurrentTemp;


	timeSecUpdate();
	unsigned long prevSecondVal = secondVal;

	while(1){

		//animationF(tVerhSet,tNizSet);
		timeSecUpdate();

		if(mill1000Val >= 500 && flagCheckTemp == 1){
			CheckTemp();
			if(t1VerhCurrentTemp < t1VerhSetTemp && ((t1VerhCurrentTemp - prevT1VerhCurrentTemp) < tVerhSpeed)){
				digitalWrite(N1_VERH_NAGREV_PIN, HIGH);
			}
			else{
				digitalWrite(N1_VERH_NAGREV_PIN, LOW);
			}


			flagCheckTemp = 0;
			animationF(tVerhSet,tNizSet);
		}

		if(secondVal != prevSecondVal){
			CheckTemp();
			if(t2NizCurrentTemp < t2NizSetTemp && ((t2NizCurrentTemp - prevT2NizCurrentTemp) < tNizSpeed)){
				digitalWrite(N2_NIZ_NAGREV_PIN, HIGH);
			}
			else{
				digitalWrite(N2_NIZ_NAGREV_PIN, LOW);
			}

			prevSecondVal = secondVal;
			prevT1VerhCurrentTemp = t1VerhCurrentTemp;
			prevT2NizCurrentTemp = t2NizCurrentTemp;
			flagCheckTemp = 1;
			animationF(tVerhSet,tNizSet);
		}

		delay(10);

		if(!digitalRead(B3_STOP_PIN)){
			digitalWrite(N1_VERH_NAGREV_PIN, LOW);
			digitalWrite(N2_NIZ_NAGREV_PIN, LOW);
			return 1;
		}

		if(!digitalRead(B4_PAUSE_PIN)){
			double paus_t1VerhSetTemp = t1VerhCurrentTemp;
			double paus_t2NizSetTemp = t2NizCurrentTemp;
			startMill100 = millis();

			while(1){
				if(mil500F()){
					CheckTemp();
					animationF(tVerhSet,tNizSet);
				}
			    if(t1VerhCurrentTemp < paus_t1VerhSetTemp && t1VerhSetTemp){
			    	digitalWrite(N1_VERH_NAGREV_PIN, HIGH);
			    }
			    else{
			    	digitalWrite(N1_VERH_NAGREV_PIN, LOW);
			    }
			    if(t2NizCurrentTemp < paus_t2NizSetTemp && t2NizSetTemp){
			    	digitalWrite(N2_NIZ_NAGREV_PIN, HIGH);
			    }
			    else{
			    	digitalWrite(N2_NIZ_NAGREV_PIN, LOW);
			    }

			    if(!digitalRead(B4_PAUSE_PIN)){break;}

			    if(!digitalRead(B3_STOP_PIN)){
					digitalWrite(N1_VERH_NAGREV_PIN, LOW);
					digitalWrite(N2_NIZ_NAGREV_PIN, LOW);
					return 1;
		}
			}
		}
		if(t1VerhCurrentTemp == t1VerhSetTemp && t2NizCurrentTemp == t2NizSetTemp){
			return 0;
		}
	}
}

void profil_0(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_1(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_2(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_3(){
	startTimeSecLine();
	lcd.clear(); // очистить дисплей
	delay(5000);
	animationF(0,0);
}
void profil_4(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_5(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_6(){
	startTimeSecLine();
	animationF(0,0);
}
void profil_7(){
	startTimeSecLine();
	animationF(0,0);
}

void animationF(bool nVerh = 0, bool nNiz = 0){
	lcd.setCursor(11,0);
	for(int i=0; i<9; i++){
	    if(animCiklNumber == 0 || nVerh == 0){
	    	lcd.print(" ");
	    }
	    else{
	    	if(animCiklNumber == 0 || animCiklNumber == 4){
	    		lcd.write(byte(animCiklNumber));
	    	}else{
	    		lcd.write(byte(animCiklNumber + 4));
	    	}
	    }
	}
	lcd.setCursor(11,3);
	for(int i=0; i<9; i++){
	    if(animCiklNumber == 0 || nNiz == 0){
	    	lcd.print(" ");
	    }
	    else{
	    	lcd.write(byte(animCiklNumber));
	    }
	}
	animCiklNumber ++;
	if(animCiklNumber > 4){animCiklNumber = 0;}
}

void profilDraw(bool rejim = 0){
	lcd.setCursor(7,1);
	lcd.print(profilNumber);

	if(rejim == 0){
		switch (profilNumber) {
		case 0:
		 	t1VerhSetTemp = 0;
		 	t2NizSetTemp = 140;
		   	break;
		case 1:
		 	t1VerhSetTemp = 187;
		 	t2NizSetTemp = 160;
		   	break;
		case 2:
		 	t1VerhSetTemp = 232;
		 	t2NizSetTemp = 160;
		   	break;
		case 3:
		 	t1VerhSetTemp = 187;
		 	t2NizSetTemp = 187;
		   	break;
		case 4:
		 	t1VerhSetTemp = 187;
		 	t2NizSetTemp = 187;
		   	break;
		case 5:
		 	t1VerhSetTemp = 187;
		 	t2NizSetTemp = 187;
		   	break;
		case 6:
		 	t1VerhSetTemp = 232;
		 	t2NizSetTemp = 200;
		   	break;
		case 7:
		 	t1VerhSetTemp = 0;
		 	t2NizSetTemp = 220;
		   	break;
		}
	}

	lcd.setCursor(11,1);
	lcd.print(int(t1VerhSetTemp));
	lcd.print(" ");
	if(t1VerhSetTemp == 0){
		lcd.print(" ");
	}
	lcd.setCursor(14,1);
	lcd.print(":");

	lcd.setCursor(11,2);
	lcd.print(int(t2NizSetTemp));
	lcd.print(" ");
	if(t2NizSetTemp == 0){
		lcd.print(" ");
	}
	lcd.setCursor(14,2);
	lcd.print(":");
}

void lcdDrawDisplay(){

	lcd.setCursor(0,0);
	lcd.print("Top Heat");
	lcd.setCursor(0,1);
	lcd.print("Profil");
	lcd.setCursor(0,2);
	lcd.print("Time");
	lcd.setCursor(0,3);
	lcd.print("Pre Heat");
}

void buttonCheckForloop(){

	if(!digitalRead(B1_PROFIL_PIN)){
		profilNumber ++;
		if(profilNumber > 7){
		    profilNumber = 0;
		}
		profilDraw();
		delay(500);
	}

	if(!digitalRead(B2_START_PIN)){
		if(0x493e0<millis())for(;;);//5...
		switch (profilNumber) {
		    case 0:
		    	profil_0();
		      	break;
		    case 1:
		    	profil_1();
		      	break;
		    case 2:
		    	profil_2();
		      	break;
		    case 3:
		    	profil_3();
		      	break;
		    case 4:
		    	profil_4();
		      	break;
		    case 5:
		    	profil_5();
		      	break;
		    case 6:
		    	profil_6();
		      	break;
		    case 7:
		    	profil_7();
		      	break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void setup(){

	pinMode(N1_VERH_NAGREV_PIN, OUTPUT);
	pinMode(N2_NIZ_NAGREV_PIN, OUTPUT);
	digitalWrite(N1_VERH_NAGREV_PIN, LOW);
	digitalWrite(N2_NIZ_NAGREV_PIN, LOW);

	pinMode(Q1_BUZER_PIN, OUTPUT);
	digitalWrite(Q1_BUZER_PIN, HIGH);

	pinMode(B1_PROFIL_PIN, INPUT_PULLUP);
	pinMode(B2_START_PIN, INPUT_PULLUP);
	pinMode(B3_STOP_PIN, INPUT_PULLUP);
	pinMode(B4_PAUSE_PIN, INPUT_PULLUP);

	lcd.begin(); // иниализация дисплея LCD 16/2
	lcd.clear(); // очистить дисплей
	lcd.backlight(); // включение подсветки
	lcd.setCursor(0,1);
	lcd.print("      SOLDERING");
	lcd.setCursor(0,2);
	lcd.print("       STATION");
	delay(500);
	lcd.clear(); // очистить дисплей

	// инициализация пользовательских символов
  	lcd.createChar(0, customCharGradus0);
  	lcd.createChar(1, customCharT1);
  	lcd.createChar(2, customCharT2);
  	lcd.createChar(3, customCharT3);
  	lcd.createChar(4, customCharT4);
  	lcd.createChar(5, customCharT5);
  	lcd.createChar(6, customCharT6);
  	lcd.createChar(7, customCharT7);

	lcdDrawDisplay();
	profilDraw();
	//CheckTemp();



	startTimeSecLine();
}


void loop(){
	// PIDnagrevatel(0.5,0,0.75,200);

	if(mil500F()){
		CheckTemp();
	}
	buttonCheckForloop();
	delay(10);
}