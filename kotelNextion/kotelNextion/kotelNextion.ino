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
byte setTimerSushki = 60;        // время работы сушки м
byte setTempJarki = 60;          // установленная температура жарки 
byte setTempVarki = 70;          // установленная температура варки
byte setTimerDushirovania = 30;  // время работы душирования м
//////////////////////////////////////////////////////////////////////////


byte tempT1 = 0;  // текущая температура термометра 1 ds18b20
byte tempT2 = 0;  // текущая температура термометра 2 ntc 10 kOm

byte hourTime = 0;
byte minuteTime = 0;
byte minuteTime90 = 0;
byte secondTime = 0;

unsigned long prewMil = 0;     // предыдущее значение миллис
unsigned long thisMil = 0;     // текущее значение миллис
unsigned long resMil = 0;      // результат вычитания миллис

// main flags

bool enableSushkaFlag = 1;
bool enableJarkaFlag = 1;
bool enableVarkaFlag = 1;
bool enableDushirovanieFlag = 1;
bool enableDimFlag = 1;
bool enableParFlag = 1;
bool enableKonvekciaFlag = 1;

byte curentOperationIndex = 0;

bool timeUpdate(){
	bool timeUpdateFlag = 0;
	thisMil = millis();
	resMil = thisMil - prewMil;
	if(resMil >= 1000){
		secondTime ++;
		prewMil = thisMil;
		timeUpdateFlag = 1;
		if(secondTime >= 60){
			minuteTime ++;
			minuteTime90 ++;
			secondTime = 0;
			if(minuteTime >= 60){
				hourTime ++;
				minuteTime = 0;
			}
		}
	}
	return timeUpdateFlag;
}

void startTimeUpdate(){
	hourTime = 0;
 	minuteTime = 0;
 	minuteTime90 = 0;
 	secondTime = 0;
 	prewMil = millis();
 	timeUpdate();
}

bool timerSushki1(){}
bool timerDushirovania2(){}

void chekTemp(){}
void displayDraw(){}

void fSushka(){}
void fJarka(){}
void fVarka(){}
void fDushirovanie(){}

void fSeting(){}
void fGotovka(){}

void setup() {

}

void loop() {

}
