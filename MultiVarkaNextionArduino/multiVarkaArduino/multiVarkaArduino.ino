// РАСПИНОВКА Arduino /////////////////////////////////////////////////////////////
#define NEXTION_TX_PIN 0  //.
#define NEXTION_RX_PIN 1  //.

#define TERMO1_PIN 2
#define TERMO2_PIN A0

#define RELE_SUSHKI_PIN 3
#define RELE_NAGREVA_PIN 4
#define RELE_DUSHIROVANIA_PIN 5

#define RELE_DIM_PIN 6
#define RELE_PAR_PIN 7
#define RELE_KONVEKCIA_PIN 8

#define RELE_BEEPER_PIN 13
////////////////////////////////////////////////////////////////////////////////////

// НАСТРОЙКИ ПО УМОЛЧАНИЮ //////////////////////////////////////////////////////////
unsigned long setTimerSushki = 60;        // время работы сушки 60м
byte setTempJarki = 60;                   // установленная температура жарки 
byte setTempVarki = 70;                   // установленная температура варки
unsigned long setTimerDushirovania = 30;  // время работы душирования 30м
////////////////////////////////////////////////////////////////////////////////////
byte peregrevTempT1 = 2;      // температура перегрева для Т1
byte gisterzisTempT1 = 2;     // гистерезис для Т1
// main state flags
bool enableSushkaState = 1;
bool enableJarkaState = 1;
bool enableVarkaState = 1;
bool enableDushirovanieState = 1;
bool enableDimState = 1;
bool enableParState = 1;
bool enableKonvekciaJState = 1;
bool enableKonvekciaVState = 1;
// directDef
#define MY_HIGH 0
#define MY_LOW 1
// КОНЕЦ НАСТРОЕК //////////////////////////////////////////////////////////////////

byte curentOperationIndex = 0;

unsigned long setTimerSushkiH60 = setTimerSushki / 60;
unsigned long setTimerSushkiM60 = setTimerSushki - (setTimerSushkiH60 * 60);
unsigned long setTimerDushirovaniaH60 = setTimerDushirovania / 60;
unsigned long setTimerDushirovaniaM60 = setTimerDushirovania - (setTimerDushirovaniaH60 * 60);

byte serialReadMasiv[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//           0x65, страница, компонент, команда, 0xFF, 0xFF, 0xFF
//byte cmdSushka[] = {0x65, 0x0, 0x1, 0x1, 0xFF, 0xFF, 0xFF};
//byte cmdSushka2[7] = {101, 0, 1, 1, 255, 255, 255};

#include <OneWire.h>
OneWire  ds(TERMO1_PIN);

#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(10, 11); // RX, TX

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

void timeUpdate(){
	
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

float updateTemp1_DS18B20(){  // sys
	byte data[12];
  	byte addr[8];  
	if (!ds.search(addr)) {
	  	return 254;  // No more addresses
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

float updateTemp2_NTC10K(){  // sys
  	VRT = analogRead(TERMO2_PIN);      //Считываем аналоговое значение VRT
  	VRT = (5.00 / 1023.00) * VRT;      //Преобразуем в напряжение
  	VR = VCC - VRT;
  	RT = VRT / (VR / R);               //Сопротивление RT
  	ln = log(RT / RT0);
  	TX = (1 / ((ln / B) + (1 / T0)));  //Температура с термистора
  	TX = TX - 273.15;                  //Преобразуем в цельсии
  	return TX;
}

void chekTemp(){  // main
	tempT1 = updateTemp1_DS18B20();
	tempT2 = updateTemp2_NTC10K();

	// SoftSerial.print("T1 ");
	// SoftSerial.print(tempT1);
	// SoftSerial.print("   ");
	// SoftSerial.print("T2 ");
	// SoftSerial.println(tempT2);
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
void nextionSenderBAR(int fJJ, int fValVal){
	Serial.print("j");
	Serial.print(fJJ);
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
	Serial.print(".txt=\"");
	Serial.print(fHourHour);
	Serial.print(":");
	Serial.print(fMinuteMinute);
	Serial.print("\"");
	nextionSenderEnd();
}


void ReceptSetingDraw(){

	if(enableSushkaState == 1){nextionSenderPIC(1, 3);}
	else{nextionSenderPIC(1, 2);}
	if(enableJarkaState == 1){nextionSenderPIC(2, 5);}
	else{nextionSenderPIC(2, 4);}
	if(enableVarkaState == 1){nextionSenderPIC(3, 7);}
	else{nextionSenderPIC(3, 6);}
	if(enableDushirovanieState == 1){nextionSenderPIC(4, 9);}
	else{nextionSenderPIC(4, 8);}
	if(enableDimState == 1){nextionSenderPIC(11, 13);}
	else{nextionSenderPIC(11, 12);}
	if(enableParState == 1){nextionSenderPIC(14, 15);}
	else{nextionSenderPIC(14, 14);}
	if(enableKonvekciaJState == 1){nextionSenderPIC(12, 17);}
	else{nextionSenderPIC(12, 16);}
	if(enableKonvekciaVState == 1){nextionSenderPIC(15, 17);}
	else{nextionSenderPIC(15, 16);}

	setTimerSushkiH60 = setTimerSushki / 60;
	setTimerSushkiM60 = setTimerSushki - (setTimerSushkiH60 * 60);
	setTimerDushirovaniaH60 = setTimerDushirovania / 60;
	setTimerDushirovaniaM60 = setTimerDushirovania - (setTimerDushirovaniaH60 * 60);

	nextionSenderNUM(0, setTimerSushkiH60);
	nextionSenderNUM(1, setTimerSushkiM60);
	nextionSenderNUM(4, setTimerDushirovaniaH60);
	nextionSenderNUM(5, setTimerDushirovaniaM60);
	nextionSenderNUM(2, setTempJarki);
	nextionSenderNUM(3, setTempVarki);
}

void gotovkaStartDraw(){
	setTimerSushkiH60 = setTimerSushki / 60;
	setTimerSushkiM60 = setTimerSushki - (setTimerSushkiH60 * 60);
	setTimerDushirovaniaH60 = setTimerDushirovania / 60;
	setTimerDushirovaniaM60 = setTimerDushirovania - (setTimerDushirovaniaH60 * 60);

	if(enableSushkaState && setTimerSushki){
		nextionSenderPIC(1, 2);
		nextionSenderTIME(0, setTimerSushkiH60, setTimerSushkiM60);
	}else{
		nextionSenderPIC(1, 0);
		nextionSenderPIC(5, 1);
		nextionSenderPIC(13, 25);
	}

	if(enableJarkaState && setTempJarki){
		nextionSenderPIC(2, 4);
		nextionSenderNUM(0, setTempJarki);
		if(enableDimState){}
		if(enableKonvekciaJState){}
	}else{
		nextionSenderPIC(2, 0);
		nextionSenderPIC(6, 1);
	}

	if(enableVarkaState && setTempVarki){
		nextionSenderPIC(3, 6);
		nextionSenderNUM(3, setTempVarki);
		if(enableParState){}
		if(enableKonvekciaVState){}
	}else{
		nextionSenderPIC(3, 0);
		nextionSenderPIC(7, 1);
	}

	if(enableDushirovanieState && setTimerDushirovania){
		nextionSenderPIC(4, 8);
		nextionSenderTIME(1, setTimerDushirovaniaH60, setTimerDushirovaniaM60);
	}else{
		nextionSenderPIC(4, 0);
		nextionSenderPIC(8, 1);
		nextionSenderPIC(12, 24);
	}

	if(curentOperationIndex == 0){
		nextionSenderTXT("j0.val=0");
		nextionSenderTXT("j1.val=0");
		nextionSenderTXT("j2.val=0");
		nextionSenderTXT("j3.val=0");
	}
	else if(curentOperationIndex == 1){
		nextionSenderTXT("j0.val=100");
		nextionSenderTXT("j1.val=0");
		nextionSenderTXT("j2.val=0");
		nextionSenderTXT("j3.val=0");
	}
	else if(curentOperationIndex == 2){
		nextionSenderTXT("j0.val=100");
		nextionSenderTXT("j1.val=100");
		nextionSenderTXT("j2.val=0");
		nextionSenderTXT("j3.val=0");
	}
	else if(curentOperationIndex == 3){
		nextionSenderTXT("j0.val=100");
		nextionSenderTXT("j1.val=100");
		nextionSenderTXT("j2.val=100");
		nextionSenderTXT("j3.val=0");
	}
	else if(curentOperationIndex == 4){
		nextionSenderTXT("j0.val=100");
		nextionSenderTXT("j1.val=100");
		nextionSenderTXT("j2.val=100");
		nextionSenderTXT("j3.val=100");
	}
}

void dopSet(){
	nextionSenderNUM(0,peregrevTempT1);
	nextionSenderNUM(1,gisterzisTempT1);
	byte curbut = 0xFF;  // !
	while(1){
	    curbut = nextionReciever();

	    if(curbut == 0xFF){}
		else if(curbut == 8){return;}

		else if(curbut == 14){
			if(peregrevTempT1 > 0){
				peregrevTempT1 -= 1;
				nextionSenderNUM(0,peregrevTempT1);
			}
		}
		else if(curbut == 13){
			if(peregrevTempT1 < 30){
				peregrevTempT1 += 1;
				nextionSenderNUM(0,peregrevTempT1);
			}
		}
		else if(curbut == 15){
			if(gisterzisTempT1 > 0){
				gisterzisTempT1 -= 1;
				nextionSenderNUM(1,gisterzisTempT1);
			}
		}
		else if(curbut == 16){
			if(gisterzisTempT1 < 15){
				gisterzisTempT1 += 1;
				nextionSenderNUM(1,gisterzisTempT1);
			}
		}
	}
}

void ReceptSetingChekBut(){
	byte curbut = 0xFF;  // !
	while(1){
		curentOperationIndex = 0;
		curbut = nextionReciever();
		if(curbut == 0xFF){}

		else if(curbut == 1){enableSushkaState = !enableSushkaState;}
		else if(curbut == 19){setTimerSushki -= 60;}
		else if(curbut == 20){setTimerSushki += 60;}
		else if(curbut == 26){setTimerSushki -= 1;}
		else if(curbut == 27){setTimerSushki += 1;}

		else if(curbut == 2){enableJarkaState = !enableJarkaState;}
		else if(curbut == 24){setTempJarki -= 1;}
		else if(curbut == 25){setTempJarki += 1;}
		else if(curbut == 12){enableDimState = !enableDimState;}
		else if(curbut == 13){enableKonvekciaJState = !enableKonvekciaJState;}

		else if(curbut == 3){enableVarkaState = !enableVarkaState;}
		else if(curbut == 29){setTempVarki -= 1;}
		else if(curbut == 30){setTempVarki += 1;}
		else if(curbut == 15){enableParState = !enableParState;}
		else if(curbut == 16){enableKonvekciaVState = !enableKonvekciaVState;}

		else if(curbut == 4){enableDushirovanieState = !enableDushirovanieState;}
		else if(curbut == 32){setTimerDushirovania -= 60;}
		else if(curbut == 33){setTimerDushirovania += 60;}
		else if(curbut == 35){setTimerDushirovania -= 1;}
		else if(curbut == 36){setTimerDushirovania += 1;}

		else if(curbut == 43){
			nextionSenderTXT("page page1");
			Gotovka();
			nextionSenderTXT("page page0");
			ReceptSetingDraw();
		}

		else if(curbut == 44){
			nextionSenderTXT("page page5");
			dopSet();
			nextionSenderTXT("page page0");
			ReceptSetingDraw();
		}
		
		if(curbut != 0xFF){ReceptSetingDraw();}
	}
}

void ReceptSetingStart(){
	nextionSenderTXT("page page0");
	ReceptSetingDraw();
	ReceptSetingChekBut();
}

bool fitPaus(){
	nextionSenderTXT("page page2");
	byte curbut = 0xFF;  // !
	while(1){
		curbut = nextionReciever();
		if(curbut == 5){return 0;}
		else if(curbut == 6){break;}
	}
	//startTimeUpdate();
	nextionSenderTXT("page page1");
	gotovkaStartDraw();
	return 1;
}


bool Sushka(){
	nextionSenderPIC(1, 3);
	byte curbut = 0xFF;  // !
	unsigned long internalTimer = setTimerSushki;
	startTimeUpdate();
	unsigned long resInternalTimer = internalTimer - minuteTime;
	unsigned long prevResInternalTimer = 0;
	unsigned long resInternalTimerH60 = resInternalTimer / 60;
	unsigned long resInternalTimerM60 = resInternalTimer - (resInternalTimerH60 * 60);

	digitalWrite(RELE_SUSHKI_PIN, MY_HIGH);

	while(resInternalTimer){
		timeUpdate();
		resInternalTimer = internalTimer - minuteTime;
		resInternalTimerH60 = resInternalTimer / 60;
		resInternalTimerM60 = resInternalTimer - (resInternalTimerH60 * 60);
		if(resInternalTimer != prevResInternalTimer){
			nextionSenderTIME(0, resInternalTimerH60, resInternalTimerM60);
			prevResInternalTimer = resInternalTimer;

			int barVal = resInternalTimer;
			barVal = map(barVal, setTimerSushki, 0, 0, 100);
			nextionSenderBAR(0, barVal); // ...
		}

		//curbut = 0xFF;  // ! PAUSE
		curbut = nextionReciever();
		if(curbut == 0xFF){}
		else if(curbut == 10){digitalWrite(RELE_SUSHKI_PIN, MY_LOW);return 0;}  // stop
		else if(curbut == 9){  // pause
			digitalWrite(RELE_SUSHKI_PIN, MY_LOW);
			internalTimer = resInternalTimer;
			bool fitPausRet = fitPaus();
			prevResInternalTimer = 0;
			startTimeUpdate();
			nextionSenderPIC(1, 3);
			if(fitPausRet == 0){return 0;}
			else{digitalWrite(RELE_SUSHKI_PIN, MY_HIGH);}
		}
		delay(500);	
	}
	digitalWrite(RELE_SUSHKI_PIN, MY_LOW);
	nextionSenderPIC(1, 2);
	return 1;
}
bool Jarka(){
	nextionSenderPIC(2, 5);
	byte curbut = 0xFF;  // !
	byte prevTempT1 = 0;
	byte prevTempT2 = 0;
	if(enableDimState){digitalWrite(RELE_DIM_PIN, MY_HIGH);}
	if(enableKonvekciaJState){digitalWrite(RELE_KONVEKCIA_PIN, MY_HIGH);}
	chekTemp();
	//byte startTemp = (tempT1 + tempT2) / 2;
	byte startTemp = tempT2;
	while(tempT2 < setTempJarki){
		chekTemp();
		if(tempT1 <= setTempJarki + peregrevTempT1 - gisterzisTempT1){
			digitalWrite(RELE_NAGREVA_PIN, MY_HIGH);
		}
		else if(tempT1 >= (setTempJarki + peregrevTempT1)){
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
		}

		if((tempT1 != prevTempT1) || (tempT2 != prevTempT2)){
			nextionSenderNUM(1, tempT1);
			nextionSenderNUM(2, tempT2);
			prevTempT1 = tempT1;
			prevTempT2 = tempT2;

			int barVal = tempT2;
			barVal = map(barVal, startTemp, setTempJarki, 0, 100);
			if(barVal < 0){barVal = 0;}
			nextionSenderBAR(1, barVal); // ...
		}

		//curbut = 0xFF;  // !
		curbut = nextionReciever();
		if(curbut == 0xFF){}
		else if(curbut == 10){  // stop
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
			digitalWrite(RELE_DIM_PIN, MY_LOW);
			digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
			return 0;
		}
		else if(curbut == 9){  // pause
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
			digitalWrite(RELE_DIM_PIN, MY_LOW);
			digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
			bool fitPausRet = fitPaus();
			startTemp = tempT2;
			prevTempT1 = 0;
			prevTempT2 = 0;
			nextionSenderPIC(2, 5);
			if(fitPausRet == 0){return 0;}
			else{digitalWrite(RELE_NAGREVA_PIN, MY_HIGH);
				if(enableDimState){digitalWrite(RELE_DIM_PIN, MY_HIGH);}
				if(enableKonvekciaJState){digitalWrite(RELE_KONVEKCIA_PIN, MY_HIGH);}
			}
		}

	    delay(500);
	}
	nextionSenderNUM(1, tempT1);
	nextionSenderNUM(2, tempT2);
	digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
	digitalWrite(RELE_DIM_PIN, MY_LOW);
	digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
	nextionSenderPIC(2, 4);
	return 1;
}

bool Varka(){
	nextionSenderPIC(3, 7);
	byte curbut = 0xFF;  // !
	byte prevTempT1 = 0;
	byte prevTempT2 = 0;
	if(enableParState){digitalWrite(RELE_PAR_PIN, MY_HIGH);}
	if(enableKonvekciaVState){digitalWrite(RELE_KONVEKCIA_PIN, MY_HIGH);}
	chekTemp();
	//byte startTemp = (tempT1 + tempT2) / 2;
	byte startTemp = tempT2;
	while(tempT2 < setTempVarki){
		chekTemp();
		if(tempT1 <= setTempVarki + peregrevTempT1 - gisterzisTempT1){
			digitalWrite(RELE_NAGREVA_PIN, MY_HIGH);
		}
		else if(tempT1 >= (setTempVarki + peregrevTempT1)){
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
		}

		if((tempT1 != prevTempT1) || (tempT2 != prevTempT2)){
			nextionSenderNUM(4, tempT1);
			nextionSenderNUM(5, tempT2);
			prevTempT1 = tempT1;
			prevTempT2 = tempT2;

			int barVal = tempT2;
			barVal = map(barVal, startTemp, setTempVarki, 0, 100);
			if(barVal < 0){barVal = 0;}
			nextionSenderBAR(2, barVal); // ...
		}

		//curbut = 0xFF;  // !
		curbut = nextionReciever();
		if(curbut == 0xFF){}
		else if(curbut == 10){  // stop
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
			digitalWrite(RELE_PAR_PIN, MY_LOW);
			digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
			return 0;
		}
		else if(curbut == 9){  // pause
			digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
			digitalWrite(RELE_PAR_PIN, MY_LOW);
			digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
			bool fitPausRet = fitPaus();
			startTemp = tempT2;
			prevTempT1 = 0;
			prevTempT2 = 0;
			nextionSenderPIC(3, 7);
			if(fitPausRet == 0){return 0;}
			else{digitalWrite(RELE_NAGREVA_PIN, MY_HIGH);
				if(enableDimState){digitalWrite(RELE_PAR_PIN, MY_HIGH);}
				if(enableKonvekciaJState){digitalWrite(RELE_KONVEKCIA_PIN, MY_HIGH);}
			}
		}

	    delay(500);
	}
	nextionSenderNUM(4, tempT1);
	nextionSenderNUM(5, tempT2);
	digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
	digitalWrite(RELE_PAR_PIN, MY_LOW);
	digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
	nextionSenderPIC(3, 6);
	return 1;
}

bool Dushirovanie(){
	nextionSenderPIC(4, 9);
	byte curbut = 0xFF;  // !
	unsigned long internalTimer = setTimerDushirovania;
	startTimeUpdate();
	unsigned long resInternalTimer = internalTimer - minuteTime;
	unsigned long prevResInternalTimer = 0;
	unsigned long resInternalTimerH60 = resInternalTimer / 60;
	unsigned long resInternalTimerM60 = resInternalTimer - (resInternalTimerH60 * 60);
	digitalWrite(RELE_DUSHIROVANIA_PIN, MY_HIGH);
	while(resInternalTimer){
		timeUpdate();
		resInternalTimer = internalTimer - minuteTime;
		resInternalTimerH60 = resInternalTimer / 60;
		resInternalTimerM60 = resInternalTimer - (resInternalTimerH60 * 60);
		if(resInternalTimer != prevResInternalTimer){
			nextionSenderTIME(1, resInternalTimerH60, resInternalTimerM60);
			prevResInternalTimer = resInternalTimer;

			int barVal = resInternalTimer;
			barVal = map(barVal, setTimerDushirovania, 0, 0, 100);
			nextionSenderBAR(3, barVal); // ...
		}

		//curbut = 0xFF;  // !
		curbut = nextionReciever();
		if(curbut == 0xFF){}
		else if(curbut == 10){digitalWrite(RELE_DUSHIROVANIA_PIN, MY_LOW);return 0;}  // stop
		else if(curbut == 9){  // pause
			digitalWrite(RELE_DUSHIROVANIA_PIN, MY_LOW);
			internalTimer = resInternalTimer;
			bool fitPausRet = fitPaus();
			prevResInternalTimer = 0;
			startTimeUpdate();
			nextionSenderPIC(4, 9);
			if(fitPausRet == 0){return 0;}
			else{digitalWrite(RELE_DUSHIROVANIA_PIN, MY_HIGH);}
		}
		delay(500);
	}
	digitalWrite(RELE_DUSHIROVANIA_PIN, MY_LOW);
	nextionSenderPIC(4, 8);
	return 1;
}

void Gotovka(){
	gotovkaStartDraw();
		
	if(enableSushkaState && setTimerSushki){
		bool retSushka = Sushka();
		if(retSushka == 0){return;}
	}
	curentOperationIndex = 1;
	gotovkaStartDraw();
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);
	
	if(enableJarkaState && setTempJarki){
		bool retJarka = Jarka();
		if(retJarka == 0){return;}
	}
	curentOperationIndex = 2;
	gotovkaStartDraw();
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);
	
	if(enableVarkaState && setTempVarki){
		bool retVarka = Varka();
		if(retVarka == 0){return;}
	}
	curentOperationIndex = 3;
	gotovkaStartDraw();
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);
	
	if(enableDushirovanieState && setTimerDushirovania){
		bool retDushirovanie = Dushirovanie();
		if(retDushirovanie == 0){return;}
	}
	curentOperationIndex = 4;
	gotovkaStartDraw();
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);
	delay(300);
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);
	delay(300);
	digitalWrite(RELE_BEEPER_PIN, MY_HIGH);
	delay(500);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);

	// ГОТОВО
	nextionSenderTXT("page page3");
	while(1){
	    byte curbut = nextionReciever();
	    if(curbut == 7){
	    	nextionSenderTXT("page page0");
	    	ReceptSetingDraw();
	    	return;
	    }
	}
}

void setup() {

	pinMode(RELE_SUSHKI_PIN, OUTPUT);
	pinMode(RELE_NAGREVA_PIN, OUTPUT);
	pinMode(RELE_DUSHIROVANIA_PIN, OUTPUT);
	pinMode(RELE_DIM_PIN, OUTPUT);
	pinMode(RELE_PAR_PIN, OUTPUT);
	pinMode(RELE_KONVEKCIA_PIN, OUTPUT);
	pinMode(RELE_BEEPER_PIN, OUTPUT);

	digitalWrite(RELE_SUSHKI_PIN, MY_LOW);
	digitalWrite(RELE_NAGREVA_PIN, MY_LOW);
	digitalWrite(RELE_DUSHIROVANIA_PIN, MY_LOW);
	digitalWrite(RELE_DIM_PIN, MY_LOW);
	digitalWrite(RELE_PAR_PIN, MY_LOW);
	digitalWrite(RELE_KONVEKCIA_PIN, MY_LOW);
	digitalWrite(RELE_BEEPER_PIN, MY_LOW);

	Serial.begin(9600);
	SoftSerial.begin(9600);
	T0 = 25 + 273.15;  //Температура T0 из даташита, преобразуем из цельсиев в кельвины

	delay(100);
}

void loop() {
	ReceptSetingStart();
}