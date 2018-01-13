
#define greenPikVariantDatchika 1  // вариант подключения датчика 0 аналог 1 цифровой
#define ignitionPinD 2
#define ignitionPinA A1
#define datchikRazrejeniaPin A0
#define dvaObDEF 720UL
#define kolichestvoWhiteRiseVDvuhOborotah 5
#define gisterezisDatchikaRazrejenia 5
#define gisterezisDatchikaZajigania 1000

unsigned long tStartForGrenPik = 0;
unsigned long tStartForWhiteRise = 0;
unsigned long tStopForWhiteRise = 0;
unsigned long otstavanieVgr = 0;
unsigned long calibrVal = 0;

// unsigned long otstavanieVms = 0;
// unsigned long dvaOborotaVms = 0;
// unsigned long koefVmsgr = 0;

byte greenPikVariantDVS = 1;  // вариант двигателя
byte whiteRiseCounter = 0;
byte whiteZeroCrossCounter = 0;
boolean greenPikFlag = 0;

inline void greenPikVD1();
inline void greenPikVD2();
inline void greenPikVD3();
inline void greenPikVA1();
inline void greenPikVA2();
inline void greenPikVA3();
inline void greenPik();
inline void whiteRise();
inline void whiteZeroCross();
inline void raschetOtstavania();
////////////////////////////////
inline void greenPikVD1(){
	boolean whControl = 0;
	while(whControl == 0){
		whControl = digitalRead(ignitionPinD);
	}
}
////////////////////////////////
inline void greenPikVA1(){
	int previousAnalogVal = 1;
	int thisAnalogVal = 0;
	// while(previousAnalogVal > 0){
	// 	previousAnalogVal = analogRead(ignitionPinA);
	// }
	while(thisAnalogVal <= gisterezisDatchikaZajigania){
		thisAnalogVal = analogRead(ignitionPinA);
	}
}
////////////////////////////////
inline void greenPik(){
	#if greenPikVariantDatchika != 0
	switch(greenPikVariantDVS){
		case 1:
		greenPikVD1();
		break;
		case 2:
		greenPikVD2();
		break;
		case 3:
		greenPikVD3();
		break;
	}
	#elif greenPikVariantDatchika = 0
	switch(greenPikVariantDVS){
		case 1:
		greenPikVA1();
		break;
		case 2:
		greenPikVA2();
		break;
		case 3:
		greenPikVA3();
		break;
	}
	#endif
}
////////////////////////////////
inline void whiteRise(){
	int previousAnalogVal = 1;
	int thisAnalogVal = 0;

	if(whiteRiseCounter != 0){
		while(previousAnalogVal > 0){
			previousAnalogVal = analogRead(datchikRazrejeniaPin);
		}
	}
	while(thisAnalogVal <= gisterezisDatchikaRazrejenia){
		thisAnalogVal = analogRead(datchikRazrejeniaPin);
		whiteRiseCounter ++;
	}
}
////////////////////////////////
inline void raschetOtstavania(){

	whiteRiseCounter = 0;
	greenPik();
	tStartForGrenPik = millis();
	whiteRise();
	tStartForWhiteRise = millis();
	while(whiteRiseCounter < kolichestvoWhiteRiseVDvuhOborotah){
		whiteRise();
	}
	tStopForWhiteRise = millis();

	otstavanieVgr = (tStartForWhiteRise - tStartForGrenPik - calibrVal) * (dvaObDEF / (tStopForWhiteRise - tStartForWhiteRise));
	// otstavanieVms = tStartForWhiteRise - tStartForGrenPik;
	// dvaOborotaVms = tStopForWhiteRise - tStartForWhiteRise;
	// koefVmsgr = dvaObDEF / dvaOborotaVms;
	// otstavanieVgr = (otstavanieVms - calibrVal) * koefVmsgr;
}

void setup() {
}

void loop() {
}
