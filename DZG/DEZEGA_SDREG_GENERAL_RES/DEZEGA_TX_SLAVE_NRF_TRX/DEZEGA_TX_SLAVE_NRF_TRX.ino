// ProMini

#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"

// значение для возврата если сенсор запрещен
#define SENS_DISABLEREAD_VAL 3000

RF24 radio(9,10);  // nrf init CE:9, CSN:10, MOSI:11, MISO:12, SCK:13

// nrf init:
byte addressNRF[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

char s_d; // переменная для хранения считывания управляющего байта символа

bool operKeyFlag = 0;
byte heartRate = 99;
byte txBatLvl = 10;

typedef struct transmiteStructure{
	byte operationKeyTX = 0;
	float val_T1 = SENS_DISABLEREAD_VAL;
	float val_T2 = SENS_DISABLEREAD_VAL;
	float val_CCO2 = SENS_DISABLEREAD_VAL;
	float val_O2 = SENS_DISABLEREAD_VAL;
	short val_CO = SENS_DISABLEREAD_VAL;
	float val_Press_inh = SENS_DISABLEREAD_VAL;  // мин
	float val_Press_exh = SENS_DISABLEREAD_VAL;  // макс
	short minuteTest = 0;  // X>=<60  общее количество минут что идет тест
	byte val_BatteryLevel_TX_HM = 50;  // уровень заряда батареи в %
	byte flagZapisiAndColorTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
};
transmiteStructure txStrctVal;

// значения калибровки структура для приема по радио
typedef struct receiverStructure{
	int operationKeyRX;
};
receiverStructure rxStrctCalibrVal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NRF rx and tx F
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioNrfSetup(){
	radio.begin();                           // активировать модуль
	radio.setAutoAck(1);                     // режим подтверждения приёма, 1 вкл 0 выкл
	radio.setRetries(0,15);                  // (время между попыткой достучаться, число попыток)
	radio.enableAckPayload();                // разрешить отсылку данных в ответ на входящий сигнал
	radio.setPayloadSize(32);                // размер пакета, в байтах
	radio.openWritingPipe(addressNRF[1]);    // передаем по трубе 1
	radio.openReadingPipe(1,addressNRF[0]);  // хотим слушать трубу 0
	radio.setChannel(0x60);                  // выбираем канал (в котором нет шумов!)
	radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS);        // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	                                         // при самой низкой скорости самуf высокуf чувствительность и дальность!!
	radio.powerUp();                         // начать работу
	radio.startListening();                  // начинаем слушать эфир, приёмный модуль
	// radio.stopListening();                // стоп прослушивания для передачи
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendNRFdataF(){
	// отправляем значения за секунду по радио
	if(operKeyFlag == 0){txStrctVal.operationKeyTX = 1;}
	else{txStrctVal.operationKeyTX = 2;}
	operKeyFlag = !operKeyFlag;
	if(txStrctVal.operationKeyTX == 1){txStrctVal.val_BatteryLevel_TX_HM = txBatLvl;}
	else if(txStrctVal.operationKeyTX == 2){txStrctVal.val_BatteryLevel_TX_HM = heartRate;}

	txStrctVal.operationKeyTX = 1;
	bool radioWriteOk = 0;
	radio.stopListening();                   // перестаем слушать эфир, для передачи
	radioWriteOk = radio.write(&txStrctVal, sizeof(txStrctVal));
	radio.startListening();                  // начинаем слушать эфир, для приема
	txStrctVal.operationKeyTX = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NRF ПРИЕМ ДАННЫХ КАЛИБРОВКИ И СТАРТ СТОП:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioReceiverButChng(){
	if (radio.available()){radio.read(&rxStrctCalibrVal, sizeof(rxStrctCalibrVal));}

	// start stop recording radio button
	if(rxStrctCalibrVal.operationKeyRX == 1){
		rxStrctCalibrVal.operationKeyRX = 0;
		Serial.print("bch");  // отправка нажатой кнопки из радио в сериал
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup(){
	Serial.begin(250000);
	delay(10);
	radioNrfSetup();

	delay(990);
}

void loop(){
	while(1){
		if(Serial.available() > 0){
		    char s_d = 'y';
			String s_str = "";
			for(int i=0; i<3; i++){
		    	s_d = Serial.read();
		    	s_str += s_d;
			}
			if(s_str == "snd"){
				txStrctVal.val_T1 = Serial.parseFloat();  // float
				txStrctVal.val_T2 = Serial.parseFloat();  // float
				txStrctVal.val_CCO2 = Serial.parseFloat();  // float
				txStrctVal.val_O2 = Serial.parseFloat();  // float
				txStrctVal.val_CO = Serial.parseInt();  // short
				txStrctVal.val_Press_inh = Serial.parseFloat();  // float
				txStrctVal.val_Press_exh = Serial.parseFloat();  // float
				txStrctVal.minuteTest = Serial.parseInt();  // short
				// Serial.read();
				// txStrctVal.val_BatteryLevel_TX_HM = Serial.parseInt();  // byte
				txBatLvl = Serial.parseInt();  // byte
				// Serial.read();
				txStrctVal.flagZapisiAndColorTransmite = Serial.parseInt();  // byte

				sendNRFdataF();
			}
		}
		radioReceiverButChng();
	}
}