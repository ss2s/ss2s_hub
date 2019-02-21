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
byte heartRate = 75;
byte txBatLvl = 10;

typedef struct transmiteStructure{
	byte operationKeyTX = 0;
	float val_T1 = 999;
	float val_T2 = 999;
	float val_CCO2 = 6;
	float val_O2 = 101;
	short val_CO = 2001;
	float val_Press_inh = -10;  // мин
	float val_Press_exh = 10;  // макс
	short minuteTest = 0;  // X>=<60  общее количество минут что идет тест
	byte val_BatteryLevel_TX_HM = 10;  // уровень заряда батареи в %
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
	radio.setPALevel (RF24_PA_MIN); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
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
	// txStrctVal.operationKeyTX = 1;

	bool radioWriteOk = 0;
	radio.stopListening();                   // перестаем слушать эфир, для передачи
	radioWriteOk = radio.write(&txStrctVal, sizeof(txStrctVal));
	radio.startListening();                  // начинаем слушать эфир, для приема
	txStrctVal.operationKeyTX = 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tstBatValTX(){
	int promBatLvlVal;
	promBatLvlVal = analogRead(A0);
	promBatLvlVal = map(promBatLvlVal, 0, 1023, 0, 500);
	promBatLvlVal = constrain(promBatLvlVal, 370, 420);
	promBatLvlVal = map(promBatLvlVal, 370, 420, 0, 100);
	txBatLvl = promBatLvlVal;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup(){
	Serial.begin(250000);
	delay(10);
	radioNrfSetup();

	delay(990);
}

void loop(){
	tstBatValTX();
	sendNRFdataF();
	delay(1000);
}