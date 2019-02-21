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

#define BAT_LVL_READ_TYPE 0      // тип считывателя уровня батареи: 0-аналоговый,  1-цифровой
#define BAT_LEVEL_ANALOG_PIN A3  // пин подключения аналогового сигнала с батареи, если это предусмотрено в настр. выше

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define START_STOP_BUTTON_PIN 5  // кнопка включения выключения записи. замыкать
#define LED_DATA_PIN 4 // led pin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RF24 radio(9,10);  // nrf init CE:9, CSN:10, MOSI:11, MISO:12, SCK:13
// RF24 radio(48,53);  // nrf init CE, CSN  nrf init:  MISO:50, MOSI:51, SCK:52
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long signalErrTime;  // signall err Millis start val
byte innerSignalAntVal = 100;  // внутреннее значение уровня сигнала
int receiveErrorCounter = 0;  // счетчик ошибок приема
int otherErrorCounter = 0;  // счетчик ошибок прочие

// уровень заряда батареи в %
byte val_BatteryLevel_RX = 50;

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
	byte val_BatteryLevel_TX = 50;  // уровень заряда батареи в %
	byte flagZapisiAndColorTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
};
receiveStructure rxStrctVal;
// start stop структура для передачи по радио
typedef struct transmiteStructure{
	int operationKeyRX;
};
transmiteStructure txStrctCalibrVal;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte addressNRF[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRGB iLed[1];  // Define the Array of leds, LED dataType 1 светодиод
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

		if(rxStrctVal.operationKeyTX == 1){
			signalErrTime = millis();

			Serial.println("radio receive OK");
			Serial.println("");
			Serial.print("T1: "); Serial.println(rxStrctVal.val_T1);
			Serial.print("T2: "); Serial.println(rxStrctVal.val_T2);
			Serial.print("CO2: "); Serial.println(rxStrctVal.val_CCO2);
			Serial.print("O2: "); Serial.println(rxStrctVal.val_O2);
			Serial.print("CO: "); Serial.println(rxStrctVal.val_CO);
			Serial.print("Press inh: "); Serial.println(rxStrctVal.val_Press_inh);
			Serial.print("Press exh: "); Serial.println(rxStrctVal.val_Press_exh);
			Serial.print("test time: "); Serial.println(rxStrctVal.minuteTest);
			Serial.print("receive TX bat level: "); Serial.println(rxStrctVal.val_BatteryLevel_TX);
			Serial.print("RX bat level: "); Serial.println(val_BatteryLevel_RX);
			Serial.print("record and color flag: "); Serial.println(rxStrctVal.flagZapisiAndColorTransmite);
			Serial.print("operation key: "); Serial.println(rxStrctVal.operationKeyTX);
			Serial.print("inner signal level: "); Serial.println(innerSignalAntVal);

			rxStrctVal.operationKeyTX = 0;
			receiveErrorCounter = 0;
			otherErrorCounter = 0;
			if(innerSignalAntVal < 100){innerSignalAntVal += 10;}
		}else{
			otherErrorCounter ++;
			if(otherErrorCounter >= 1200){
				otherErrorCounter = 0;
				Serial.println("other error: NRF no connect");
			}
		}
	}

	if(millis() - signalErrTime > 3000){
		signalErrTime = millis();
		if(innerSignalAntVal > 0){innerSignalAntVal -= 10;}
		receiveErrorCounter ++;
		Serial.println("radio receive failed, timeout end");
		Serial.print("inner signal level: "); Serial.println(innerSignalAntVal);
		Serial.print("err count: "); Serial.println(receiveErrorCounter);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// REC BUTTON CHANGE CHECK
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void recbuttonChangeCheckGF(){
	if(digitalRead(START_STOP_BUTTON_PIN) == LOW){  // если кнопка нажата
		delay(50);
		if(digitalRead(START_STOP_BUTTON_PIN) == LOW){
			Serial.println("button pressed");
			iLed[0] = CRGB(255, 255, 255);  // GRB white
			FastLED.show();
			// отправляем факт нажатие кнопки по радио
			txStrctCalibrVal.operationKeyRX = 1;
			bool radioWriteOk = 0;
			radio.stopListening();  // перестаем слушать эфир, для передачи
			radioWriteOk = radio.write(&txStrctCalibrVal, sizeof(txStrctCalibrVal));
			radio.startListening();  // начинаем слушать эфир, для приема

			delay(2000);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// опрос уровня заряда батареи 
float poolBatteryLevel(){
	if(BAT_LVL_READ_TYPE == 0){  // analog LVL BAT read
		int promBatLvlVal;
		promBatLvlVal = analogRead(BAT_LEVEL_ANALOG_PIN);
		promBatLvlVal = map(promBatLvlVal, 0, 1023, 0, 500);
		promBatLvlVal = constrain(promBatLvlVal, 370, 420);
		promBatLvlVal = map(promBatLvlVal, 370, 420, 0, 100);
		val_BatteryLevel_RX = promBatLvlVal;
	}
	else{  // digital LVL BAT read
	}
	return val_BatteryLevel_RX;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// led show
void ledReceiverShow(){
	switch(rxStrctVal.flagZapisiAndColorTransmite){
		case 0:  // blue no REC
		iLed[0] = CRGB(0, 0, 255);  // GRB blue
		FastLED.show();
		break;

		case 1:  // green
		iLed[0] = CRGB(255, 0, 0);  // GRB green
		FastLED.show();
		break;

		case 2:  // yelow
		iLed[0] = CRGB(255, 255, 0);  // GRB yelow
		FastLED.show();
		break;

		case 3:  // red
		iLed[0] = CRGB(0, 255, 0);  // GRB red
		FastLED.show();
		break;
	}
	if(rxStrctVal.flagZapisiAndColorTransmite != 4){rxStrctVal.flagZapisiAndColorTransmite = 4;}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displaySetup(){
	// assign default color value
	if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
		u8g.setColorIndex(255);     // white
	}
	else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
		u8g.setColorIndex(3);         // max intensity
	}
	else if ( u8g.getMode() == U8G_MODE_BW ) {
		u8g.setColorIndex(1);         // pixel on
	}
	else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
		u8g.setHiColorByRGB(255,255,255);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw(void) {
	// graphic commands to redraw the complete screen should be placed here
	u8g.drawRFrame(0,0,128,64,10);
	// u8g.drawRBox(5,40,10,10,2);
	// u8g.drawLine(15,60,35,60);
	// u8g.drawPixel(14,59);
	// u8g.drawDisc(30,45,5);
	// u8g.drawCircle(30,45,8);
	// u8g.setFont(u8g_font_unifont);
	u8g.setFont(u8g_font_osb21);
	u8g.drawStr( 5, 42, "DEZEGA");
	// u8g.setFont(u8g_font_5x8);
	// u8g.setFont(u8g_font_6x10);
	u8g.setFont(u8g_font_micro);
	u8g.drawStr( 70, 58, F("SS2S APPS"));
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
void u8g_prepare(void) {
	u8g.setFont(u8g_font_6x10);
	u8g.setFontRefHeightExtendedText();
	u8g.setDefaultForegroundColor();
	u8g.setFontPosTop();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup(){
	pinMode(START_STOP_BUTTON_PIN, INPUT_PULLUP);
	Serial.begin(250000);
	delay(500);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(iLed, 1); // 1 светодиод WS2812B
	iLed[0] = CRGB(255, 255, 255);  // GRB white
	FastLED.show();

	// radioNrfSetup();
	
	// delay(500);
	// displaySetup();
	// u8g_prepare();
	// u8g.setRot90();
}

void loop(){
	// radioReseiverGF();
	// recbuttonChangeCheckGF();
	// ledReceiverShow();
	// poolBatteryLevel();
	displayLoop();
}