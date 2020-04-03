// Arduino IDE 1.8.6 MEGA2560

// НАСТРОЙКИ:

// коефициенты дальномеров 5.7 defoult
#define X_CALIBRATION_FACTOR 5.7
#define Y_CALIBRATION_FACTOR 5.7
#define Z_CALIBRATION_FACTOR 5.7

// настройки фильтра датчиков дистанции
#define MAX_AVERAGE_COUNT 10.0L  // коефициент усреднения, столько последних значений при закрытой защелке
#define TRIGGERR_TRESHOLD 1.0L   // порог сбрасывания защелки в см, открытие
#define ADAPTIVE_INCREMENT 1.0L  // инкремент для адаптивгого фильтра в см скорость закрытия 

#define RS_UART_TYPE 0  // 0-SOFT 1-HARD выбор сериал хард2 или софт

#define MAX_DISTANCE 70 // Константа для определения максимального расстояния, которое мы будем считать корректным.
// x axis
#define SX_TRIG_PIN 45
#define SX_ECHO_PIN 47
// y axis
#define SY_TRIG_PIN 51
#define SY_ECHO_PIN 49
// z axis
#define SZ_TRIG_PIN 25
#define SZ_ECHO_PIN 27


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
// #include <EEPROM.h>

// Настройки Bluetooth
int BT_VER = 2;
String BTname = "AutoWeight";
String BTpin = "1111";

// var
float weight;

float tare_x, tare_y, tare_z;

float distance_x, distance_y, distance_z;

float size_x, size_y, size_z;

float duration_x, duration_y, duration_z;

float size_volume;

float size_density;

// init obj
SoftwareSerial mySerial(10, 11); // RX, TX

LiquidCrystal_I2C lcd(0x27, 20, 4); // установка адреса 0x27 и разрешения 20/4 дисплея

NewPing sonarx(SX_TRIG_PIN, SX_ECHO_PIN, MAX_DISTANCE);
NewPing sonary(SY_TRIG_PIN, SY_ECHO_PIN, MAX_DISTANCE);
NewPing sonarz(SZ_TRIG_PIN, SZ_ECHO_PIN, MAX_DISTANCE);


void BTinit() {

  if (BT_VER == 1) {
    Serial3.print("AT+NAME=" + BTname + "\r\n");
    delay(100);
    Serial3.print("AT+PSWD=\"" + BTpin + "\"\r\n");
  } else if (BT_VER == 2) {
    Serial3.print("AT+NAME" + BTname);
    delay(200);
    Serial3.print("AT+PIN" + BTpin);
  }
}

float getWeight(){

	weight = 1;

	#if RS_UART_TYPE == 1
	if (Serial2.available()) {
		weight = Serial2.parseFloat();  // читаем значение
	}
	else{return -127.0;}
	#else
	if(mySerial.available()){
		weight = mySerial.parseFloat();  // читаем значение
		// weight = mySerial.parseFloat();  // читаем значение
		
		while(mySerial.available()){
		    mySerial.read();
		}
	}
	else{return -127.0;}
	#endif
	return weight;
}

// бегущее среднее с адаптивным коэффициентом 4rev
float expRunningAverageAdaptive4_x(float newVal) {
	static int _i_val;
	static float filVal = 0;
	static float k;
	static float adaptive_coef = 1;
	
	// резкость фильтра зависит от модуля разности значений
	if (abs(newVal - filVal) > TRIGGERR_TRESHOLD) adaptive_coef = 1;
	else adaptive_coef += ADAPTIVE_INCREMENT;

	if(adaptive_coef > MAX_AVERAGE_COUNT){adaptive_coef = MAX_AVERAGE_COUNT;}

	k = 1.0L / adaptive_coef;

	filVal += (newVal - filVal) * k;

	// отрезаем лишние символы после запятой	
	_i_val = int(filVal * 10);
	filVal = float(_i_val / 10.0L);

	return filVal;
}

float expRunningAverageAdaptive4_y(float newVal) {
	static int _i_val;
	static float filVal = 0;
	static float k;
	static float adaptive_coef = 1;
	
	// резкость фильтра зависит от модуля разности значений
	if (abs(newVal - filVal) > TRIGGERR_TRESHOLD) adaptive_coef = 1;
	else adaptive_coef += ADAPTIVE_INCREMENT;

	if(adaptive_coef > MAX_AVERAGE_COUNT){adaptive_coef = MAX_AVERAGE_COUNT;}

	k = 1.0L / adaptive_coef;

	filVal += (newVal - filVal) * k;

	// отрезаем лишние символы после запятой	
	_i_val = int(filVal * 10);
	filVal = float(_i_val / 10.0L);

	return filVal;
}

float expRunningAverageAdaptive4_z(float newVal) {
	static int _i_val;
	static float filVal = 0;
	static float k;
	static float adaptive_coef = 1;
	
	// резкость фильтра зависит от модуля разности значений
	if (abs(newVal - filVal) > TRIGGERR_TRESHOLD) adaptive_coef = 1;
	else adaptive_coef += ADAPTIVE_INCREMENT;

	if(adaptive_coef > MAX_AVERAGE_COUNT){adaptive_coef = MAX_AVERAGE_COUNT;}

	k = 1.0L / adaptive_coef;

	filVal += (newVal - filVal) * k;

	// отрезаем лишние символы после запятой
	_i_val = int(filVal * 10);
	filVal = float(_i_val / 10.0L);

	return filVal;
}


inline void getDistance(int _num_meas = 3){

	duration_x = sonarx.ping_median(_num_meas);
	duration_y = sonary.ping_median(_num_meas);
	duration_z = sonarz.ping_median(_num_meas);

	distance_x = int(duration_x / X_CALIBRATION_FACTOR);
	
	distance_y = int(duration_y / Y_CALIBRATION_FACTOR);
	
	distance_z = int(duration_z / Z_CALIBRATION_FACTOR);

	distance_x = float(distance_x / 10.0);
	distance_y = float(distance_y / 10.0);
	distance_z = float(distance_z / 10.0);
}

void sizeTare(int _num_meas = 30){

	lcd.clear();
	lcd.setCursor(5, 0); // (символ(0-19), строка(0-3))
	lcd.print(F("ALL SIZES"));
	lcd.setCursor(7, 1); // (символ(0-19), строка(0-3))
	lcd.print(F("RESET"));
	lcd.setCursor(8, 2); // (символ(0-19), строка(0-3))
	lcd.print(F("T O"));
	lcd.setCursor(6, 3); // (символ(0-19), строка(0-3))
	lcd.print(F("Z E R O"));

	getDistance(_num_meas);

	tare_x = distance_x;
	tare_y = distance_y;
	tare_z = distance_z;

	lcd.clear();
}

inline void getSize(){

	getDistance();

	size_x = tare_x - distance_x;
	size_y = tare_y - distance_y;
	size_z = tare_z - distance_z;

	// if(size_x < 1 && size_x > -1){size_x = 0;}
	// if(size_y < 1 && size_y > -1){size_y = 0;}
	// if(size_z < 1 && size_z > -1){size_z = 0;}
}

inline void getFilteredSize(){
	getSize();
	size_x = expRunningAverageAdaptive4_x(size_x);
	size_y = expRunningAverageAdaptive4_y(size_y);
	size_z = expRunningAverageAdaptive4_z(size_z);

	// if(size_x < 1 && size_x > -1){size_x = 0;}
	// if(size_y < 1 && size_y > -1){size_y = 0;}
	// if(size_z < 1 && size_z > -1){size_z = 0;}

	size_volume = size_x * size_y * size_z / 1000000;
}

//////////////////////////////////////////
// Переменная для отправки на ПК
String to_serial = "";


void send_data() {
  Serial3.print(String(to_serial + '\n'));
  to_serial = "";
};
//////////////////////////////////////////

inline void drawAndSendData(){
	
	// размер
	getFilteredSize();

	lcd.setCursor(0, 0);
	lcd.print("X ");
	lcd.print(size_x, 1);
	lcd.print(" ");
	// lcd.setCursor(8, 0);
	// lcd.print("cm");

	lcd.setCursor(0, 1);
	lcd.print("Y ");
	lcd.print(size_y, 1);
	lcd.print(" ");
	// lcd.setCursor(8, 1);
	// lcd.print("cm");

	lcd.setCursor(0, 2);
	lcd.print("Z ");
	lcd.print(size_z, 1);
	lcd.print(" ");
	// lcd.setCursor(8, 2);
	// lcd.print("cm");

	// обьем
	size_volume = size_x * size_y * size_z / 1000000;

	lcd.setCursor(0, 3);
	lcd.print("V ");
	lcd.print(size_volume, 6);
	lcd.print(" ");

	// |
	lcd.setCursor(11, 0);
	lcd.print("|");
	lcd.setCursor(11, 1);
	lcd.print("|");
	lcd.setCursor(11, 2);
	lcd.print("|");
	lcd.setCursor(11, 3);
	lcd.print("|");

	// вес
	weight = getWeight();

	lcd.setCursor(12, 0);
	lcd.print("BEC:  kg");
	lcd.setCursor(12, 1);
	if(weight >= 0){lcd.print(" ");}
	lcd.print(weight, 3);
	if(weight < 10 && weight > -10){
		lcd.print("  ");
	}
	else if(weight < 100 && weight > -100){
		lcd.print(" ");
	}

	// плотность
	size_density = (weight / size_volume) / 1000;

	lcd.setCursor(12, 2);
	lcd.print("dencity");
	lcd.setCursor(12, 3);
	if(size_volume <= 0.000000){
		lcd.print(" no res ");
	}
	else if(weight == 0){
		lcd.print(" no res ");
	}
	else if(size_density < 99.999 && size_density > 0.000){
		lcd.setCursor(13, 3);
		lcd.print(size_density, 3);
		if(size_density < 10){
			lcd.print(" ");
		}
		lcd.print(" ");
	}

	//Формируем строку для отправки в последовательный порт - length;height;width;Volume;weight; = x;y;z;v;w;
    to_serial = String(size_x, 1) + ';' + String(size_y, 1) + ';' + String(size_z, 1) + ';' + String(size_volume, 6) + ';' + String(weight, 3) + ';';
    send_data();
}


void debugRS232S2(){
	if (Serial2.available()) {
		Serial.write(Serial2.read());
	}

	if (Serial.available()) {
		Serial2.write(Serial.read());
	}
}

void debugRS232MS(){
	if (mySerial.available()) {
		Serial.write(mySerial.read());
	}

	if (Serial.available()) {
		mySerial.write(Serial.read());
	}
}


void setup(){

	Serial.begin(115200);  // debug

	Serial2.begin(2400);  // init UART rs232 hard
	mySerial.begin(2400);  // init UART rs232 soft

	Serial3.begin(9600);  // init bluetooth
	delay(500);
	BTinit();
	
	// дисплей
	lcd.begin();
	lcd.backlight();
	lcd.clear();

	sizeTare(30);
}

void loop(){

	// debugRS232MS();
	// debugRS232S2();
	drawAndSendData();
}