#include "HX711.h"
#include "TroykaDHT.h"
#include <SoftwareSerial.h>


#define RELE_PIN 8  // РЕЛЕ
#define RAIN_SENSOR_VCC_PIN 7  // ДАТЧИК ДОЖДЯ VCC
#define RAIN_SENSOR_PIN 6  // ДАТЧИК ДОЖДЯ
#define MOTION_SENSOR_PIN 5  // ДАТЧИК ДВИЖЕНИЯ
#define DHT_DATA_PIN 4 // ДАТЧИК ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ DHT 22
#define HX711_DOUT_PIN A2  // ВЕСЫ DT
#define HX711_PD_SCK_PIN A3  // ВЕСЫ SCK

SoftwareSerial SoftSerial(10, 11); // RX, TX

// настройка портов GSM serial 0: com=hard, gsm=soft.   1: com=soft, gsm=hard.
#define GSM_SERIAL_MODE 0
#if GSM_SERIAL_MODE == 1
#define GsmModem Serial
#define ComPort SoftSerial
#else
#define GsmModem SoftSerial
#define ComPort Serial
#endif

// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(DHT_DATA_PIN, DHT21);

#define HX711_GAIN 64  // усилитель 64 chanel A
HX711 hx711Obj;

bool BEE_MOTION_DETECT = 0;
bool BEE_RAIN_DETECT = 0;

int BEE_TEMP = 0;
int BEE_HMDT = 0;
int BEE_WEIGHT = 0;

void getDhtDataInt(void)
{
  	// считывание данных с датчика
	dht.read();
	// проверяем состояние данных
	switch(dht.getState())
	{
		// всё OK
		case DHT_OK:
		// выводим показания влажности и температуры
		BEE_TEMP = dht.getTemperatureC();
		BEE_HMDT = dht.getHumidity();
		
		// выводим показания влажности и температуры
		ComPort.print("Temperature = ");
		ComPort.print(BEE_TEMP);
		// ComPort.print(dht.getTemperatureC());
		ComPort.println(" C \t");
		ComPort.print("Humidity = ");
		ComPort.print(BEE_HMDT);
		// ComPort.print(dht.getHumidity());
		ComPort.println(" %\n");
		break;
		// ошибка контрольной суммы
		case DHT_ERROR_CHECKSUM:
		ComPort.println("Checksum error");
		break;
		// превышение времени ожидания
		case DHT_ERROR_TIMEOUT:
		ComPort.println("Time out error");
		break;
		// данных нет, датчик не реагирует или отсутствует
		case DHT_ERROR_NO_REPLY:
		ComPort.println("Sensor not connected");
		break;
	}
  
  // ждём две секунды
  delay(2000);
}

int getWeight()
{
	BEE_WEIGHT = 500;
	return BEE_WEIGHT;
}

bool getMotinState()
{
	BEE_MOTION_DETECT = digitalRead(MOTION_SENSOR_PIN);
	return BEE_MOTION_DETECT;
}

bool getRainState()
{
	digitalWrite(RAIN_SENSOR_VCC_PIN, HIGH);
	delay(50);
	BEE_RAIN_DETECT = digitalRead(RAIN_SENSOR_PIN);
	digitalWrite(RAIN_SENSOR_VCC_PIN, LOW);
	return BEE_RAIN_DETECT;
}

void setup()
{	
	pinMode(RELE_PIN, OUTPUT);
	digitalWrite(RELE_PIN, LOW);

	pinMode(RAIN_SENSOR_PIN, INPUT);
	pinMode(RAIN_SENSOR_VCC_PIN, OUTPUT);

	pinMode(MOTION_SENSOR_PIN, INPUT);

	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	ComPort.begin(115200);
	GsmModem.begin(115200);

	GsmModem.print("AT");
	if (GsmModem.available() > 0)
	{
		int resiveAnswer = GsmModem.parseInt();
		if(resiveAnswer == 0){digitalWrite(13, HIGH);}
	}

	dht.begin();

	hx711Obj.begin(HX711_DOUT_PIN, HX711_PD_SCK_PIN, HX711_GAIN);
	delay(100);
	hx711Obj.set_gain(HX711_GAIN);
	delay(400);
	digitalWrite(13, LOW);
	delay(500);
	digitalWrite(13, HIGH);
	delay(500);
	digitalWrite(13, LOW);
}

void loop()
{
	
}