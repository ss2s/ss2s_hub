#include <OneWire.h>

// РАСПИНОВКА Arduino ///////////////////////////////////////////////////
#define NEXTION_TX_PIN 0
#define NEXTION_RX_PIN 1

#define TERMO1_PIN 2
#define TERMO2_PIN A0

#define RELE_SUSHKI_PIN 3
#define RELE_NAGREVA_PIN 4
#define RELE_DUSHIROVANIA_PIN 5
#define RELE_DIM_PIN 6
#define RELE_PAR_PIN 7
#define RELE_KOVEKCIA_PIN 8
//////////////////////////////////////////////////////////////////////////


// НАСТРОЙКИ ПО УМОЛЧАНИЮ ////////////////////////////////////////////////
unsigned long setTimerSushki = 60;        // время работы сушки м
byte setTempJarki = 60;          // установленная температура жарки 
byte setTempVarki = 70;          // установленная температура варки
unsigned long setTimerDushirovania = 30;  // время работы душирования м
//////////////////////////////////////////////////////////////////////////

byte serialReadMasiv[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//           0x65, страница, компонент, команда, 0xFF, 0xFF, 0xFF
//byte cmdSushka[] = {0x65, 0x0, 0x1, 0x1, 0xFF, 0xFF, 0xFF};
//byte cmdSushka2[7] = {101, 0, 1, 1, 255, 255, 255};
#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(10, 11); // RX, TX

OneWire  ds(TERMO1_PIN);

// main temp value
byte tempT1 = 0;  // текущая температура термометра 1 ds18b20
byte tempT2 = 0;  // текущая температура термометра 2 ntc 10 kOm

// main time value
unsigned long hourTime = 0;
unsigned long minuteTime = 0;
unsigned long minuteTime60 = 0;
unsigned long secondTime = 0;
unsigned long secondTime60 = 0;

unsigned long prewMil = 0;     // предыдущее значение миллис
unsigned long thisMil = 0;     // текущее значение миллис
unsigned long resMil = 0;      // результат вычитания миллис

// main state flags
bool enableSushkaState = 1;
bool enableJarkaState = 1;
bool enableVarkaState = 1;
bool enableDushirovanieState = 1;
bool enableDimState = 1;
bool enableParState = 1;
bool enableKonvekciaState = 1;

byte curentOperationIndex = 0;


bool timeUpdate(){
	
	thisMil = millis();
	resMil = thisMil - prewMil;
	secondTime = resMil / 1000;
	minuteTime = secondTime / 60;
	hourTime = minuteTime / 60;
	secondTime60 = secondTime - (minuteTime * 60);
	minuteTime60 = minuteTime - (hourTime * 60);
}

void startTimeUpdate(){
	hourTime = 0;
 	minuteTime = 0;
 	minuteTime60 = 0;
 	secondTime = 0;
 	secondTime60 = 0;
 	prewMil = millis();
 	timeUpdate();
}

bool timerSushki1(){}
bool timerDushirovania2(){}

byte updateTemp1_DS18B20(){  // sys
	byte data[12];
  	byte addr[8];  
	if (!ds.search(addr)) {
	  	return 254;// No more addresses
	}
	ds.reset_search(); 
	if (OneWire::crc8(addr, 7) != addr[7]) {
	    delay(1000);
	    return 255;  // CRC is not valid!
	}
	ds.reset();            
  	ds.select(addr);        
  	ds.write(0x44);      
  	delay(1000);   
  	ds.reset();
  	ds.select(addr);    
  	ds.write(0xBE);          
  	for (int i = 0; i < 9; i++) {           
  	  	data[i] = ds.read();  
  	}
  	int raw = (data[1] << 8) | data[0];
  	// Переводим в температуру   
  	if (data[7] == 0x10) raw = (raw & 0xFFF0) + 12 - data[6];  
  	float temperature = raw / 16.0;
  	return temperature;
}

// NTC
#define RT0 10000   // Ом сопртивление терморезистора при 25 гр С
#define B 3977      // B константа
#define VCC 5       //Напряжение питания 5V
#define R 10000     //R=10 КОм сопротивление подтягывающего резистора
//переменные
float RT, VR, ln, TX, T0, VRT;

byte updateTemp2_NTC10K(){  // sys
  	VRT = analogRead(A0);              //Считываем аналоговое значение VRT
  	VRT = (5.00 / 1023.00) * VRT;      //Преобразуем в напряжение
  	VR = VCC - VRT;
  	RT = VRT / (VR / R);               //Сопротивление RT
  	ln = log(RT / RT0);
  	TX = (1 / ((ln / B) + (1 / T0))); //Температура с термистора
  	TX = TX - 273.15;                 //Преобразуем в цельсии
  	return TX;
}

void chekTemp(){  // main
	tempT1 = updateTemp1_DS18B20();
	tempT2 = updateTemp2_NTC10K();
}

byte nextionReciever(){

	serialReadMasiv[2] = 0xFF;
	byte i = 0;
	while(Serial.available()){
		delay(10);
	    serialReadMasiv[i] = Serial.read();
	    SoftSerial.print(serialReadMasiv[i]);
	    SoftSerial.print("   ");
	    i++;
	    if(i >=7){
	    	break;
	    }
	}
	if(serialReadMasiv[2] != 0xFF){
		SoftSerial.println("");
	}

	return serialReadMasiv[2];
}

void nextionSenderEnd(){
	Serial.write(0xFF);
	Serial.write(0xFF);
	Serial.write(0xFF);
}

void nextionSenderTXT(String fStr){
	Serial.print(fStr);
	nextionSenderEnd();
}
void nextionSenderNUM(int fNN, int fValVal){
	Serial.print("n");
	Serial.print(fNN);
	Serial.print(".val=");
	Serial.print(fValVal);
	nextionSenderEnd();
}
void nextionSenderPIC(int fPP, int fValVal){
	Serial.print("p");
	Serial.print(fPP);
	Serial.print(".pic=");
	Serial.print(fValVal);
	nextionSenderEnd();
}
void nextionSenderTIME(int fTT, int fHourHour, int fMinuteMinute){
	Serial.print("t");
	Serial.print(fTT);
	Serial.print(".txt=");
	Serial.print(fHourHour);
	Serial.print(":");
	Serial.print(fMinuteMinute);
	nextionSenderEnd();
}

void Sushka(){}
void Jarka(){}
void Varka(){}
void Dushirovanie(){}

void ReceptSetingStart(){}
void ReceptSeting(){
	ReceptSetingStart();
}

void Gotovka(){
	Sushka();
	Jarka();
	Varka();
	Dushirovanie();
}

void setup() {
	Serial.begin(9600);
	SoftSerial.begin(9600);
	T0 = 25 + 273.15;  //Температура T0 из даташита, преобразуем из цельсиев в кельвины

	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
}

void loop() {

	// ReceptSeting();
	// Gotovka();

	byte tv = nextionReciever();

	if(tv != 0xFF){
		if(tv == 1){
			digitalWrite(13, HIGH);
			nextionSenderPIC(1, 3);
			nextionSenderNUM(0, 22);
		}
		else if(tv == 2){
			digitalWrite(13, LOW);
			nextionSenderPIC(1, 2);
		}
	}

	chekTemp();
	SoftSerial.print("T1 ");
	SoftSerial.print(tempT1);
	SoftSerial.print("   ");
	SoftSerial.print("T2 ");
	SoftSerial.println(tempT2);
	delay(1000);
}