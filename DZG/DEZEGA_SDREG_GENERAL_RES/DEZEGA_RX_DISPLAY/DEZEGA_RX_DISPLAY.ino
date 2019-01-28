// ProMini

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RF24 radio(9,53);  // nrf init CE, CSN  nrf init:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// значения сенсоров за секунду структура для хранения значений и передачи по радио
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
	byte flagZapisiTransmite = 0;  // если 1 то на флешку идет запись если 0 то нет
	byte operationKeyTX = 0;
};
receiveStructure rxStVal;
// уровень заряда батареи в %
byte val_BatteryLevel_TX = 50;
// значения калибровки структура для приема по радио
typedef struct transmiteStructure{
	int operationKey;
	int addrNameKey;
	float inMin = 0;
	float inMax = 1000;
	float outMin = 0;
	float outMax = 1000;
};
transmiteStructure txStCalibrVal;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte addressNRF[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
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
	radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
	radio.setDataRate (RF24_250KBPS);        // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
	                                         // при самой низкой скорости самуf высокуf чувствительность и дальность!!
	radio.powerUp();                         // начать работу
	radio.startListening();                  // начинаем слушать эфир, приёмный модуль
	// radio.stopListening();                // First, stop listening so we can talk.
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// NRF rx and tx F
// NRF signal level F
// battery level F
// encoder F
// display sensor values and battery level and signal level
// menu calibration and admin pass
void setup(){
	Serial.begin(250000);
	radioNrfSetup();
}

void loop(){
}