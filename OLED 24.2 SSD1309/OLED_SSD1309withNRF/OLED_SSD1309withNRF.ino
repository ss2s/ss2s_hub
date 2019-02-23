// ProMini

#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"

#include "FastLED.h"            // WS2812B svetodiod lib 

#include "U8glib.h"
// U8GLIB_SSD1309_128X64 u8g(13, 11, 10, 9, 8);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9
#include "SoftSPI.h"

const uint8_t SOFT_SPI_MISO_PIN = 16;
const uint8_t SOFT_SPI_MOSI_PIN = 15;
const uint8_t SOFT_SPI_SCK_PIN  = 14;
const uint8_t SPI_MODE = 0;

SoftSPI<SOFT_SPI_MISO_PIN, SOFT_SPI_MOSI_PIN, SOFT_SPI_SCK_PIN, SPI_MODE> spi;
U8GLIB_SSD1309_128X64 u8g(14, 15, 8, 7, 6);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9


// #include "U8glib.h"
// U8GLIB_SSD1309_128X64 u8g(13, 11, 10, 9, 8);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BAT_LEVEL_ANALOG_PIN A3  // пин подключения аналогового сигнала с батареи, если это предусмотрено в настр. выше
#define BAT_LVL_READ_TYPE 0      // тип считывателя уровня батареи: 0-аналоговый,  1-цифровой
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define START_STOP_BUTTON_PIN 5  // кнопка включения выключения записи. замыкать
#define BUTTON_TYPE 1 // тип кнопки 1:0
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LED_DATA_PIN 4 // led pin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RF24 radio(9,10);  // nrf init CE:9, CSN:10, MOSI:11, MISO:12, SCK:13
// RF24 radio(48,53);  // nrf init CE, CSN  nrf init:  MISO:50, MOSI:51, SCK:52
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long signalErrTime;  // signall err Millis start val
byte innerSignalAntVal = 100;  // внутреннее значение уровня сигнала
int receiveErrorCounter = 0;  // счетчик ошибок приема

// уровень заряда батареи в %
byte val_BatteryLevel_RX = 50;
byte val_BatteryLevel_TTX = 50;
// серцебиение, ударов в минуту
byte heartRate = 99;

// значения сенсоров за секунду структура для хранения значений и приема по радио
typedef struct receiveStructure{
	byte operationKeyTX = 0;
	float val_T1 = 0;
	float val_T2 = 0;
	float val_CCO2 = 0;
	float val_O2 = 0;
	short val_CO = 0;
	float val_Press_inh = 0;  // мин
	float val_Press_exh = 0;  // макс
	short minuteTest = 0;  // X>=<60  общее количество минут что идет тест
	byte val_BatteryLevel_TX_HM = 50;  // уровень заряда батареи в %
	byte flagZapisiAndColorTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
};
receiveStructure rxStrctVal;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte addressNRF[][2] = {"1Node", "2Node"/*, "3Node", "4Node", "5Node", "6Node"*/}; //возможные номера труб
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NRF rx and tx F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioNrfSetup(){
	radio.begin();                           // активировать модуль
	radio.setAutoAck(1);                     // режим подтверждения приёма, 1 вкл 0 выкл
	radio.setRetries(0,15);                  // (время между попыткой достучаться, число попыток)
	radio.enableAckPayload();                // разрешить отсылку данных в ответ на входящий сигнал
	radio.setPayloadSize(32);                // размер пакета, в байтах
	radio.openWritingPipe(addressNRF[0]);    // передаем по трубе 1
	radio.openReadingPipe(1,addressNRF[1]);  // хотим слушать трубу 0
	radio.setChannel(0x60);                  // выбираем канал (в котором нет шумов!)
	radio.setPALevel (RF24_PA_MIN); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS);        // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	                                         // при самой низкой скорости самуf высокуf чувствительность и дальность!!
	radio.powerUp();                         // начать работу
	radio.startListening();                  // начинаем слушать эфир, приёмный модуль
	// radio.stopListening();                // First, stop listening so we can talk.
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// val struct radio receiver
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioReseiverGF(){

	if (radio.available()){

		radio.read(&rxStrctVal, sizeof(rxStrctVal));

		if(rxStrctVal.operationKeyTX == 1 || rxStrctVal.operationKeyTX == 2){

			// rxStrctVal.val_BatteryLevel_TX_HM = 15;
			// rxStrctVal.operationKeyTX = 2;

			signalErrTime = millis();

			// Serial.println("radio OK");
			// Serial.println("");
			// Serial.print("T1: "); Serial.println(rxStrctVal.val_T1);
			// Serial.print("T2: "); Serial.println(rxStrctVal.val_T2);
			// Serial.print("CO2: "); Serial.println(rxStrctVal.val_CCO2);
			// Serial.print("O2: "); Serial.println(rxStrctVal.val_O2);
			// Serial.print("CO: "); Serial.println(rxStrctVal.val_CO);
			// Serial.print("Pres inh: "); Serial.println(rxStrctVal.val_Press_inh);
			// Serial.print("Pres exh: "); Serial.println(rxStrctVal.val_Press_exh);
			// Serial.print("test time: "); Serial.println(rxStrctVal.minuteTest);
			// Serial.print("TX bat level: "); Serial.println(rxStrctVal.val_BatteryLevel_TX_HM);
			// Serial.print("RX bat level: "); Serial.println(val_BatteryLevel_RX);
			// Serial.print("rec and color flag: "); Serial.println(rxStrctVal.flagZapisiAndColorTransmite);
			// Serial.print("op key: "); Serial.println(rxStrctVal.operationKeyTX);
			// Serial.print("inner sig lvl: "); Serial.println(innerSignalAntVal);

			if(rxStrctVal.operationKeyTX == 1){
				val_BatteryLevel_TTX = rxStrctVal.val_BatteryLevel_TX_HM;
			}
			if(rxStrctVal.operationKeyTX == 2){
				heartRate = rxStrctVal.val_BatteryLevel_TX_HM;
			}

			rxStrctVal.operationKeyTX = 0;
			receiveErrorCounter = 0;
			if(innerSignalAntVal < 100){innerSignalAntVal += 25;}
		}
	}

	if(millis() - signalErrTime > 2000){
		signalErrTime = millis();
		if(innerSignalAntVal > 0){innerSignalAntVal -= 25;}
		receiveErrorCounter ++;
		// Serial.println("radio receive failed, time end");
		// Serial.print("inner sig lvl: "); Serial.println(innerSignalAntVal);
		// Serial.print("err count: "); Serial.println(receiveErrorCounter);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void batAnim(byte x, byte y, byte v=0){
	u8g.drawRFrame(x,y-7,16,7,0);
	u8g.drawLine(x+17,y-6,x+17,y-2);
	if(v >= 1){u8g.drawRBox(x,y-7,4,7,0);}
	if(v >= 26){u8g.drawRBox(x,y-7,8,7,0);}
	if(v >= 51){u8g.drawRBox(x,y-7,12,7,0);}
	if(v >= 76){u8g.drawRBox(x,y-7,16,7,0);}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void antAnim(byte x, byte y){
	if(innerSignalAntVal >= 1){u8g.drawLine(x,y,x,y);u8g.drawLine(x+1,y,x+1,y-1);}
	if(innerSignalAntVal >= 26){u8g.drawLine(x+3,y,x+3,y-2);u8g.drawLine(x+4,y,x+4,y-3);}
	if(innerSignalAntVal >= 51){u8g.drawLine(x+6,y,x+6,y-4);u8g.drawLine(x+7,y,x+7,y-5);}
	if(innerSignalAntVal >= 76){u8g.drawLine(x+9,y,x+9,y-6);u8g.drawLine(x+10,y,x+10,y-7);}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void antIconAndSig(byte x, byte y){
	u8g.drawLine(x+4,y,x+4,y-7);
	u8g.drawLine(x,y-7,x+8,y-7);
	u8g.drawLine(x,y-7,x+4,y-3);
	u8g.drawLine(x+8,y-7,x+4,y-3);
	antAnim(x+8, y);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void testTimeVithHourDraw(byte x, byte y){
	// rxStrctVal.minuteTest = 67;

	byte xh = x;
	byte xm = x;
	String sstr;
	char schr[8];
	int dHour = rxStrctVal.minuteTest / 60;
	int dMinute = rxStrctVal.minuteTest - (dHour * 60);

	sstr = String(dHour);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;

	if(dHour < 10){
		u8g.drawStr( x, y, "0");
		xh += 6;
	}

	u8g.drawStr( xh, y, schr);

	u8g.drawStr( x+12, y-1, ":");

	sstr = String(dMinute);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;

	if(dMinute < 10){
		u8g.drawStr( x+18, y, "0");
		xm += 6;
	}
	u8g.drawStr( xm+18, y, schr);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawSETUP(void) {
	u8g.drawRFrame(0,0,128,64,10);
	// u8g.setFont(u8g_font_osb21);
	u8g.setFont(u8g_font_6x10r);
	u8g.drawStr( 4, 42, "DEZEGA");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw(void) {
	// u8g.drawRFrame(0,0,128,64,10);// u8g.drawRBox(5,40,10,10,2);// u8g.drawLine(15,60,35,60);// u8g.drawPixel(14,59);
	// u8g.drawDisc(30,45,5);// u8g.drawCircle(30,45,8);// u8g.setFont(u8g_font_unifont);// u8g.setFont(u8g_font_5x8);
	// u8g.setFont(u8g_font_osb21);// u8g.setFont(u8g_font_micro);// u8g.setFont(u8g_font_6x10);

	String sstr;
	char schr[8];

	// u8g.setFont(u8g_font_6x10r);  //--------------------------------------------->>>



	batAnim(0, 7, val_BatteryLevel_TTX);
	u8g.drawStr( 20, 7, "СВЕТ");
	// sstr = String(val_BatteryLevel_TTX);
	// for(int i=0; i<7; i++){schr[i]=sstr[i];}
	// schr[7]=0;
	// u8g.drawStr( 15, 7, schr);

	antIconAndSig(53, 7);
	// u8g.drawStr( 42, 7, "ant");
	// sstr = String(innerSignalAntVal);
	// for(int i=0; i<7; i++){schr[i]=sstr[i];}
	// schr[7]=0;
	// u8g.drawStr( 63, 7, schr);

	u8g.drawStr( 90, 7, "BAT");
	batAnim(110, 7, val_BatteryLevel_RX);
	// sstr = String(val_BatteryLevel_RX);
	// for(int i=0; i<7; i++){schr[i]=sstr[i];}
	// schr[7]=0;
	// u8g.drawStr( 111, 7, schr);




	u8g.drawLine(64,11,64,128);
	



	u8g.drawStr( 0, 19, "T1");
	sstr = String(rxStrctVal.val_T1);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 25, 19, schr);

	u8g.drawStr( 68, 19, "T2");
	sstr = String(rxStrctVal.val_T2);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 93, 19, schr);




	u8g.drawStr( 0, 30, "CO2");
	sstr = String(rxStrctVal.val_CCO2);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 25, 30, schr);

	u8g.drawStr( 68, 30, "TEST TIME");




	u8g.drawStr( 0, 41, "O2");
	sstr = String(rxStrctVal.val_O2);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 25, 41, schr);

	testTimeVithHourDraw(80, 41);
	// sstr = String(rxStrctVal.minuteTest);
	// for(int i=0; i<7; i++){schr[i]=sstr[i];}
	// schr[7]=0;
	// u8g.drawStr( 85, 41, schr);




	u8g.drawStr( 0, 52, "CO");
	sstr = String(rxStrctVal.val_CO);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 25, 52, schr);

	u8g.drawStr( 68, 52, "HR");
	sstr = String(heartRate);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 93, 52, schr);




	u8g.drawStr( 0, 63, "PRI");
	sstr = String(rxStrctVal.val_Press_inh);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 25, 63, schr);

	u8g.drawStr( 68, 63, "PRE");
	sstr = String(rxStrctVal.val_Press_exh);
	for(int i=0; i<7; i++){schr[i]=sstr[i];}
	schr[7]=0;
	u8g.drawStr( 93, 63, schr);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displaySetup(void) {
  // picture loop
  u8g.firstPage();  
  do {
    drawSETUP();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayLoop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup(){
	displaySetup();
	
	// Serial.begin(250000);
	delay(500);
	radioNrfSetup();
	delay(500);
}

void loop(){
	radioReseiverGF();
	displayLoop();
}