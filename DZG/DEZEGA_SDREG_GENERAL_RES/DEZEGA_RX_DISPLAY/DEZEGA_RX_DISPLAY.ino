// ProMini

#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"

#include "FastLED.h"            // WS2812B svetodiod lib 


// #include "U8glib.h"
// U8GLIB_SSD1309_128X64 u8g(13, 11, 10, 9, 8);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define START_STOP_BUTTON_PIN 30  // кнопка включения выключения записи. замыкать
#define LED_DATA_PIN 8 // led pin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RF24 radio(48,53);  // nrf init CE, CSN  nrf init:  MISO:50, MOSI:51, SCK:52
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long signalErrTime;  // signall err Millis start val
byte innerSignalAntVal = 100;  // внутреннее значение уровня сигнала
int receiveErrorCounter = 0;  // счетчик ошибок приема
int otherErrorCounter = 0;  // счетчик ошибок прочие

// значения сенсоров за секунду структура для хранения значений и приема по радио
typedef struct receiveStructure{
	float val_T1 = 0;
	float val_T2 = 0;
	float val_CCO2 = 0;
	float val_O2 = 0;
	short val_CO = 0;
	float val_Press_inh = 0;  // мин
	float val_Press_exh = 0;  // макс
	short minuteTest = 0;  // X>=<60  общее количество минут что идет тест
	byte val_BatteryLevel_TX = 50;  // уровень заряда батареи в %
	byte signalLevel = 0;                      // щетчик для контроля качества сигнала
	byte flagZapisiAndColorTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
	byte operationKeyTX = 0;
};
receiveStructure rxStrctVal;
// уровень заряда батареи в %
byte val_BatteryLevel_TX = 50;
// значения калибровки структура для передачи по радио
typedef struct transmiteStructure{
	int operationKeyRX;
	int addrNameCalibrUnit;
	float inMin = 0;
	float inMax = 1000;
	float outMin = 0;
	float outMax = 1000;
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
			rxStrctVal.operationKeyTX = 0;
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
			Serial.print("receive signal level: "); Serial.println(rxStrctVal.signalLevel);
			Serial.print("record and color flag: "); Serial.println(rxStrctVal.flagZapisiAndColorTransmite);
			Serial.print("operation key: "); Serial.println(rxStrctVal.operationKeyTX);
			Serial.print("inner signal level: "); Serial.println(innerSignalAntVal);

			receiveErrorCounter = 0;
			otherErrorCounter = 0;
			if(innerSignalAntVal < 100){innerSignalAntVal += 10;}
		}else{
			otherErrorCounter ++;
			if(otherErrorCounter >= 2000){
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
			radio.stopListening();                   // перестаем слушать эфир, для передачи
			radioWriteOk = radio.write(&txStrctCalibrVal, sizeof(txStrctCalibrVal));
			radio.startListening();                  // начинаем слушать эфир, для приема
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// NRF rx and tx F
// NRF signal level F
// battery level F
// display sensor values and battery level and signal level
void setup(){
	pinMode(START_STOP_BUTTON_PIN, INPUT_PULLUP);
	Serial.begin(250000);
	delay(500);

	FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(iLed, 1); // 1 светодиод WS2812B
	iLed[0] = CRGB(255, 255, 255);  // GRB white
	FastLED.show();

	radioNrfSetup();

}

void loop(){
	radioReseiverGF();
	recbuttonChangeCheckGF();
}