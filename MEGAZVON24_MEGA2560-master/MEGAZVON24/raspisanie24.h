////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /******************************************************************************************************\
          MEGAZVON24  Arduino MEGA2560 программа для автоматического перезвона колоколов по заданному времени.

          https://github.com/ss2s/MEGAZVON24_MEGA2560
        \******************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float g3231t;
bool flagManualPR = 0;
bool flag0m = 0;
bool flag15m = 0;
bool flag30m = 0;
bool flag45m = 0;
bool fllag12h = 0;
bool prazdnik = 1;
int rsecond;
int rminute = 61;  // Переменная для отслеживания изменения минут
int rhour;         // переменная для хранения часов
int rday;
int rdayOfWeek;
int rmonth;
int ryear;
String rdayofYear;
String temperatureDS3231; // переменная для хранения температуры
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DS3231 clock;           // Связываем объект clock с библиотекой DS3231
RTCDateTime DateTime;   // Определяем переменную DateTime, как описанную структурой RTCDateTime
RTCAlarmTime Alarm1;          // Определяем сущность структуры RTCAlarmTime (описанной в библиотеке DS3231)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chekVremya();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// температура ..
float get3231Temp(){
  byte tMSB, tLSB; 
  float temp3231;

  Wire.beginTransmission(0x68);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 2);

  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion

    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }

  return temp3231;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// функция отбивания времени колоколом
void timeBellRound(int _hours = 0){
	int tbrHours = _hours;
	int tbrKolokol = remapReleNameToNumber(RELE_HOUR_BLOW);
	unsigned long tbrKolokolDelay = findNotaDelayForKolokolNumber(tbrKolokol);
	unsigned int tbrHobDelay = HOUR_OF_BLOW_DELAY / 2;
	if(tbrHours > 12){
		tbrHours -= 12;
	}
	for(int i=0;i<tbrHours;i++){
		lcd.clear();
		lcd.setCursor(7,1);
		lcd.write(byte(5));
		lcd.print(i+1);
		delay(tbrHobDelay);
		nota(tbrKolokol, tbrKolokolDelay, tbrHobDelay);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void timeToDisplay(){
	DateTime = clock.getDateTime();            // Считываем c часов текущие значения даты и времени в сущность DateTime
	if (int(DateTime.minute) != rminute){// Если текущее значение минут отличается от значения в переменной отслеживания
    	rminute = int(DateTime.minute);  //  Меняем значение в переменной отслеживания минут на текущее
		lcd.clear();
		lcd.print(clock.dateFormat("d.m.Y H:i", DateTime));
		lcd.setCursor(0,1);
		lcd.print("temp ");
		lcd.print(String(clock.readTemperature()));
		//g3231t = get3231Temp();
		//lcd.print(g3231t);

		chekVremya();
		lcd.setCursor(15,1);
		lcd.print(rdayOfWeek);
		if(flagManualPR == 0){
		if(rdayOfWeek>5){
			prazdnik = 1;
		}else{
			prazdnik = 0;
		}
	}
		
	}
	if(prazdnik == 1){
		lcd.setCursor(12,1);
		lcd.write(byte(2));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// часы: время
void chekVremya(){
	DateTime = clock.getDateTime();  // Считываем c часов текущие значения даты и времени в сущность DateTime
	rsecond = int(DateTime.second);
	rminute = int(DateTime.minute);  //  Меняем значение в переменной отслеживания минут на текущее
	rhour = int(DateTime.hour);
	rday = int(DateTime.day);
	rdayOfWeek = int(DateTime.dayOfWeek);
	rmonth = int(DateTime.month);
	ryear = int(DateTime.year);
	rdayofYear = clock.dateFormat("z", DateTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void budnichniy(){  // будничный колокол

	bool bCikl = 1;
	int bKolokol = remapReleNameToNumber(RELE_BUDNICNIY);
	unsigned long tobBKolokol = findNotaDelayForKolokolNumber(bKolokol);
	unsigned int bHobDelay = BUDNICNIY_DELAY_DEF / 2;

	lcd.clear();
	lcd.setCursor(2,0);
	lcd.write(byte(0));
	lcd.setCursor(4,0);
	lcd.print("budnicniy");
	delay(1000);

	while(bCikl){
		nota(bKolokol, tobBKolokol, bHobDelay);
		lcd.clear();
		lcd.setCursor(2,0);
		lcd.write(byte(0));
		lcd.setCursor(4,0);
		lcd.print("budnicniy");
		delay(bHobDelay);
		chekVremya();
		if(rminute == 0){
			bCikl = 0;
		}
	}

	lcd.clear();
	chekPerezvon();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// РАСПИСАНИЕ:

void chekPerezvon(){
	chekVremya();
	if(flagManualPR == 0){
		if(rdayOfWeek>5){
			prazdnik = 1;
		}else{
			prazdnik = 0;
		}
	}
	if(rminute == 0 && flag0m == 0){
		flag0m = 1;
 		flag15m = 0;
 		flag30m = 0;
 		flag45m = 0;

		if(rhour == 1){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("1H");
			delay(1000);
			lcd.clear();


			flagManualPR = 0;
			if(rdayOfWeek>5){prazdnik = 1;}else{prazdnik = 0;}
		}else if(rhour == 2){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("2H");
			delay(1000);
			lcd.clear();


		}else if(rhour == 3){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("3H");
			delay(1000);
			lcd.clear();


		}else if(rhour == 4){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("4H");
			delay(1000);
			lcd.clear();


		}else if(rhour == 5){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("5H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 5 ЧАСОВ  // МЕЛОДИЯ ДЛЯ 5 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 6){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("6H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 6 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 7){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("7H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 7 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 8){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("8H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 8 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 9){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("9H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 9 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 10){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("10H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 10 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 11){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("11H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 11 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 12){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("12H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 12 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 13){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("13H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 13 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 14){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("14H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 14 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 15){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("15H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 15 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 16){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("16H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 16 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 17){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("17H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 17 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 18){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("18H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 18 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 19){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("19H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 19 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 20){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("20H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 20 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 21){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("21H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 21 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 22){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("22H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 22 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 23){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("23H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 23 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}else if(rhour == 0){
			lcd.clear();
			lcd.setCursor(6,1);
			lcd.write(byte(1));
			lcd.setCursor(8,1);
			lcd.print("24H");
			delay(1000);
			lcd.clear();


			melodiaEX3();  // МЕЛОДИЯ ДЛЯ 24 ЧАСОВ


			delay(1000);
			timeBellRound(rhour);  // функция отбивает время колоколом
		}
		rminute = 61;
		timeToDisplay();
	}else if(rminute == 15 && flag15m == 0){
		lcd.clear();
		lcd.setCursor(6,1);
		lcd.write(byte(1));
		lcd.setCursor(8,1);
		lcd.print("15M");
		delay(1000);
		lcd.clear();

		if(rhour <= 23 && rhour >= 5){

 			melodia15();  // МЕЛОДИЯ ДЛЯ 15 МИНУТ
		}


		flag0m = 0;
 		flag15m = 1;
 		flag30m = 0;
 		flag45m = 0;
 		rminute = 61;
		timeToDisplay();

	}else if(rminute == 30 && flag30m == 0){
		lcd.clear();
		lcd.setCursor(6,1);
		lcd.write(byte(1));
		lcd.setCursor(8,1);
		lcd.print("30M");
		delay(1000);
		lcd.clear();


		if(rhour <= 23 && rhour >= 5){

 			melodia30();  // МЕЛОДИЯ ДЛЯ 30 МИНУТ
		}

		flag0m = 0;
 		flag15m = 0;
 		flag30m = 1;
 		flag45m = 0;
 		rminute = 61;
		timeToDisplay();

	}else if(rminute == 45 && flag45m == 0){
		lcd.clear();
		lcd.setCursor(6,1);
		lcd.write(byte(1));
		lcd.setCursor(8,1);
		lcd.print("45M");
		delay(1000);
		lcd.clear();


 		if(rhour <= 23 && rhour >= 5){

 			melodia45();  // МЕЛОДИЯ ДЛЯ 45 МИНУТ
		}


		flag0m = 0;
 		flag15m = 0;
 		flag30m = 0;
 		flag45m = 1;

 		#if BUDNICNIY_ENABLE == 1
 		if(rhour == 8 && prazdnik == 0){
 			budnichniy();
 		}else{
 			rminute = 61;
			timeToDisplay();
 		}
 		#else
 			rminute = 61;
			timeToDisplay();
 		#endif
	}
}

