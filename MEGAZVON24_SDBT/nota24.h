////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /******************************************************************************************************\
          MEGAZVON24  Arduino MEGA2560 программа для автоматического перезвона колоколов по заданному времени.

          https://github.com/ss2s/MEGAZVON24_MEGA2560
        \******************************************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// НОТЫ ДЛЯ МЕЛОДИЙ

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\\
// \\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ТЕХНИЧЕСКИЙ РАЗДЕЛ:

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// переменные для работы с функцией millis();
unsigned long startMill = 1UL;
unsigned long currentMill = 1UL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  ВРЕМЕННЫЕ ЗАДЕРЖЕКИ ПО УМОЛЧАНИЮ: настройки вынесены в отдельный файл "MELODIA_ZVONA.h"

// для каждой ноты в отдельности, 2 набор нот
// настройка в файле "config24.h"
/*unsigned long timtOfBlowUnicNota_1 = DEF_TIME_OF_BLOW_UNIC_NOTA_1;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 1 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_2 = DEF_TIME_OF_BLOW_UNIC_NOTA_2;   // ВРЕМЯ УДАРА ЯЗЫЧКА ВО 2 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_3 = DEF_TIME_OF_BLOW_UNIC_NOTA_3;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 3 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_4 = DEF_TIME_OF_BLOW_UNIC_NOTA_4;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 4 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_5 = DEF_TIME_OF_BLOW_UNIC_NOTA_5;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 5 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_6 = DEF_TIME_OF_BLOW_UNIC_NOTA_6;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 6 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_7 = DEF_TIME_OF_BLOW_UNIC_NOTA_7;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 7 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_8 = DEF_TIME_OF_BLOW_UNIC_NOTA_8;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 8 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_9 = DEF_TIME_OF_BLOW_UNIC_NOTA_9;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 9 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_10 = DEF_TIME_OF_BLOW_UNIC_NOTA_10;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 10 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_11 = DEF_TIME_OF_BLOW_UNIC_NOTA_11;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 11 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_12 = DEF_TIME_OF_BLOW_UNIC_NOTA_12;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 12 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_13 = DEF_TIME_OF_BLOW_UNIC_NOTA_13;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 13 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_14 = DEF_TIME_OF_BLOW_UNIC_NOTA_14;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 14 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_15 = DEF_TIME_OF_BLOW_UNIC_NOTA_15;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 15 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_16 = DEF_TIME_OF_BLOW_UNIC_NOTA_16;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 16 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_17 = DEF_TIME_OF_BLOW_UNIC_NOTA_17;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 17 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_18 = DEF_TIME_OF_BLOW_UNIC_NOTA_18;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 18 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_19 = DEF_TIME_OF_BLOW_UNIC_NOTA_19;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 19 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_20 = DEF_TIME_OF_BLOW_UNIC_NOTA_20;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 20 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_21 = DEF_TIME_OF_BLOW_UNIC_NOTA_21;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 21 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_22 = DEF_TIME_OF_BLOW_UNIC_NOTA_22;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 22 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_23 = DEF_TIME_OF_BLOW_UNIC_NOTA_23;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 23 КОЛОКОЛ (ВРЕМЯ НОТЫ)
unsigned long timtOfBlowUnicNota_24 = DEF_TIME_OF_BLOW_UNIC_NOTA_24;   // ВРЕМЯ УДАРА ЯЗЫЧКА В 24 КОЛОКОЛ (ВРЕМЯ НОТЫ)
*/
unsigned long timtOfBlowUnicNota_1;
unsigned long timtOfBlowUnicNota_2;
unsigned long timtOfBlowUnicNota_3;
unsigned long timtOfBlowUnicNota_4;
unsigned long timtOfBlowUnicNota_5;
unsigned long timtOfBlowUnicNota_6;
unsigned long timtOfBlowUnicNota_7;
unsigned long timtOfBlowUnicNota_8;
unsigned long timtOfBlowUnicNota_9;
unsigned long timtOfBlowUnicNota_10;
unsigned long timtOfBlowUnicNota_11;
unsigned long timtOfBlowUnicNota_12;
unsigned long timtOfBlowUnicNota_13;
unsigned long timtOfBlowUnicNota_14;
unsigned long timtOfBlowUnicNota_15;
unsigned long timtOfBlowUnicNota_16;
unsigned long timtOfBlowUnicNota_17;
unsigned long timtOfBlowUnicNota_18;
unsigned long timtOfBlowUnicNota_19;
unsigned long timtOfBlowUnicNota_20;
unsigned long timtOfBlowUnicNota_21;
unsigned long timtOfBlowUnicNota_22;
unsigned long timtOfBlowUnicNota_23;
unsigned long timtOfBlowUnicNota_24;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// функция возврата номера реле по имени
inline int remapReleNameToNumber(int _kolokolName){
	int kolokolNumber = _kolokolName;

	switch(_kolokolName){
		case RELE_K1:
		kolokolNumber = 1;
		break;
		case RELE_K2:
		kolokolNumber = 2;
		break;
		case RELE_K3:
		kolokolNumber = 3;
		break;
		case RELE_K4:
		kolokolNumber = 4;
		break;
		case RELE_K5:
		kolokolNumber = 5;
		break;
		case RELE_K6:
		kolokolNumber = 6;
		break;
		case RELE_K7:
		kolokolNumber = 7;
		break;
		case RELE_K8:
		kolokolNumber = 8;
		break;
		case RELE_K9:
		kolokolNumber = 9;
		break;
		case RELE_K10:
		kolokolNumber = 10;
		break;
		case RELE_K11:
		kolokolNumber = 11;
		break;
		case RELE_K12:
		kolokolNumber = 12;
		break;
		case RELE_K13:
		kolokolNumber = 13;
		break;
		case RELE_K14:
		kolokolNumber = 14;
		break;
		case RELE_K15:
		kolokolNumber = 15;
		break;
		case RELE_K16:
		kolokolNumber = 16;
		break;
		case RELE_K17:
		kolokolNumber = 17;
		break;
		case RELE_K18:
		kolokolNumber = 18;
		break;
		case RELE_K19:
		kolokolNumber = 19;
		break;
		case RELE_K20:
		kolokolNumber = 20;
		break;
		case RELE_K21:
		kolokolNumber = 21;
		break;
		case RELE_K22:
		kolokolNumber = 22;
		break;
		case RELE_K23:
		kolokolNumber = 23;
		break;
		case RELE_K24:
		kolokolNumber = 24;
		break;
	}

	return kolokolNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// функция возврата имени реле по номеру
inline int remapReleNumberToName(int _kolokolNumber){
	int kolokolName = _kolokolNumber;

	switch(_kolokolNumber){
		case 1:
		kolokolName = RELE_K1;
		break;
		case 2:
		kolokolName = RELE_K2;
		break;
		case 3:
		kolokolName = RELE_K3;
		break;
		case 4:
		kolokolName = RELE_K4;
		break;
		case 5:
		kolokolName = RELE_K5;
		break;
		case 6:
		kolokolName = RELE_K6;
		break;
		case 7:
		kolokolName = RELE_K7;
		break;
		case 8:
		kolokolName = RELE_K8;
		break;
		case 9:
		kolokolName = RELE_K9;
		break;
		case 10:
		kolokolName = RELE_K10;
		break;
		case 11:
		kolokolName = RELE_K11;
		break;
		case 12:
		kolokolName = RELE_K12;
		break;
		case 13:
		kolokolName = RELE_K13;
		break;
		case 14:
		kolokolName = RELE_K14;
		break;
		case 15:
		kolokolName = RELE_K15;
		break;
		case 16:
		kolokolName = RELE_K16;
		break;
		case 17:
		kolokolName = RELE_K17;
		break;
		case 18:
		kolokolName = RELE_K18;
		break;
		case 19:
		kolokolName = RELE_K19;
		break;
		case 20:
		kolokolName = RELE_K20;
		break;
		case 21:
		kolokolName = RELE_K21;
		break;
		case 22:
		kolokolName = RELE_K22;
		break;
		case 23:
		kolokolName = RELE_K23;
		break;
		case 24:
		kolokolName = RELE_K24;
		break;
	}

	return kolokolName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// функция подбора задержки к колоколу по его номеру
unsigned long findNotaDelayForKolokolNumber(int _kolokolNumber = 0){
	unsigned long fDelay = DEF_TIME_OF_BLOW_TO_THE_BELL;

	switch(_kolokolNumber){
		case 1:
		fDelay = timtOfBlowUnicNota_1;
		break;
		case 2:
		fDelay = timtOfBlowUnicNota_2;
		break;
		case 3:
		fDelay = timtOfBlowUnicNota_3;
		break;
		case 4:
		fDelay = timtOfBlowUnicNota_4;
		break;
		case 5:
		fDelay = timtOfBlowUnicNota_5;
		break;
		case 6:
		fDelay = timtOfBlowUnicNota_6;
		break;
		case 7:
		fDelay = timtOfBlowUnicNota_7;
		break;
		case 8:
		fDelay = timtOfBlowUnicNota_8;
		break;
		case 9:
		fDelay = timtOfBlowUnicNota_9;
		break;
		case 10:
		fDelay = timtOfBlowUnicNota_10;
		break;
		case 11:
		fDelay = timtOfBlowUnicNota_11;
		break;
		case 12:
		fDelay = timtOfBlowUnicNota_12;
		break;
		case 13:
		fDelay = timtOfBlowUnicNota_13;
		break;
		case 14:
		fDelay = timtOfBlowUnicNota_14;
		break;
		case 15:
		fDelay = timtOfBlowUnicNota_15;
		break;
		case 16:
		fDelay = timtOfBlowUnicNota_16;
		break;
		case 17:
		fDelay = timtOfBlowUnicNota_17;
		break;
		case 18:
		fDelay = timtOfBlowUnicNota_18;
		break;
		case 19:
		fDelay = timtOfBlowUnicNota_19;
		break;
		case 20:
		fDelay = timtOfBlowUnicNota_20;
		break;
		case 21:
		fDelay = timtOfBlowUnicNota_21;
		break;
		case 22:
		fDelay = timtOfBlowUnicNota_22;
		break;
		case 23:
		fDelay = timtOfBlowUnicNota_23;
		break;
		case 24:
		fDelay = timtOfBlowUnicNota_24;
		break;
	}
	return fDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ТЕХНИЧЕСКИЙ РАЗДЕЛ:

// 1 НАБОР НОТ
// функция нота,принимает три параметра через запятую(номер колокола, время звучания ноты, время до вызова следующей ноты)
// по умолчанию: колокол 1, время звучания ноты 300мс, таймаут между нотами 3000мс
// пример использования функции:[enota(7, 250, 2000);] 7 колокол, время ноты 250мс, таймаут до следующей ноты 2с

int enota(unsigned int _kolokol = 1,
unsigned long _timeOfBlowToTheBell = DEF_TIME_OF_BLOW_TO_THE_BELL,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	int kNumber = _kolokol;
	int kName = remapReleNumberToName(_kolokol);
	unsigned long thisTimeOfBlowToTheBell = _timeOfBlowToTheBell;
		unsigned int thisDelayUntilNextNota = _delayUntilNextNota;


	lcd.clear();
	lcd.setCursor(3,0);
	lcd.print("del ");
	lcd.print(thisTimeOfBlowToTheBell);
	lcd.print(" ms");
	lcd.setCursor(2,1);
	lcd.write(byte(0));
	lcd.print(" ");
	lcd.print(kNumber);
	lcd.print("  PIN ");
	lcd.print(kName);

	digitalWrite(kName, HHH);
	delay(thisTimeOfBlowToTheBell);
	digitalWrite(kName, LLL);
	delay(thisDelayUntilNextNota);
	lcd.clear();
}

int nota(unsigned int _kolokol = 1, unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	int kNumber = _kolokol;
	int kName = remapReleNumberToName(_kolokol);
	unsigned long thisTimeOfBlowToTheBell = findNotaDelayForKolokolNumber(kNumber);
	unsigned int thisDelayUntilNextNota = _delayUntilNextNota;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.print("del ");
	lcd.print(thisTimeOfBlowToTheBell);
	lcd.print(" ms");
	lcd.setCursor(2,1);
	lcd.write(byte(0));
	lcd.print(" ");
	lcd.print(kNumber);
	lcd.print("  PIN ");
	lcd.print(kName);

	digitalWrite(kName, HHH);
	delay(thisTimeOfBlowToTheBell);
	digitalWrite(kName, LLL);
	delay(thisDelayUntilNextNota);
	lcd.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 2 НАБОР НОТ
// НОТЫ ПО УМОЛЧАНИЮ: для каждой ноты можно настроить время работы реле по умолчанию
// задается в настройках вверху скетча
// функция нота по умолчанию,принимает один параметр (время до вызова следующей ноты)

void nota1(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота1 
	enota(1, timtOfBlowUnicNota_1, _delayUntilNextNota);}

void nota2(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота2 
	enota(2, timtOfBlowUnicNota_2, _delayUntilNextNota);}

void nota3(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота3 
	enota(3, timtOfBlowUnicNota_3, _delayUntilNextNota);}

void nota4(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота4 
	enota(4, timtOfBlowUnicNota_4, _delayUntilNextNota);}

void nota5(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота5 
	enota(5, timtOfBlowUnicNota_5, _delayUntilNextNota);}

void nota6(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота6 
	enota(6, timtOfBlowUnicNota_6, _delayUntilNextNota);}

void nota7(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота7 
	enota(7, timtOfBlowUnicNota_7, _delayUntilNextNota);}

void nota8(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота8 
	enota(8, timtOfBlowUnicNota_8, _delayUntilNextNota);}

void nota9(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота9 
	enota(9, timtOfBlowUnicNota_9, _delayUntilNextNota);}

void nota10(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота10 
	enota(10, timtOfBlowUnicNota_10, _delayUntilNextNota);}

void nota11(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота11 
	enota(11, timtOfBlowUnicNota_11, _delayUntilNextNota);}

void nota12(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота12 
	enota(12, timtOfBlowUnicNota_12, _delayUntilNextNota);}

void nota13(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота13 
	enota(13, timtOfBlowUnicNota_13, _delayUntilNextNota);}

void nota14(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота14 
	enota(14, timtOfBlowUnicNota_14, _delayUntilNextNota);}

void nota15(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота15 
	enota(15, timtOfBlowUnicNota_15, _delayUntilNextNota);}

void nota16(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота16 
	enota(16, timtOfBlowUnicNota_16, _delayUntilNextNota);}

void nota17(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота17 
	enota(17, timtOfBlowUnicNota_17, _delayUntilNextNota);}

void nota18(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота18 
	enota(18, timtOfBlowUnicNota_18, _delayUntilNextNota);}

void nota19(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота19 
	enota(19, timtOfBlowUnicNota_19, _delayUntilNextNota);}

void nota20(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота20 
	enota(20, timtOfBlowUnicNota_20, _delayUntilNextNota);}

void nota21(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота21 
	enota(21, timtOfBlowUnicNota_21, _delayUntilNextNota);}

void nota22(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота22 
	enota(22, timtOfBlowUnicNota_22, _delayUntilNextNota);}

void nota23(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота23 
	enota(23, timtOfBlowUnicNota_23, _delayUntilNextNota);}

void nota24(unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){  // нота24 
	enota(24, timtOfBlowUnicNota_24, _delayUntilNextNota);}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// КОМБО НОТЫ: для одновременного вызова нескольких колоколов принимает параметры номера колоколов + задержка до сл. нот

void combo2(
int _kolokol1 = 1,
int _kolokol2 = 2,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo2");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo3(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo3");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo4(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
int _kolokol4 = 4,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;
	int kNumber4 = _kolokol4;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);
	int kName4 = remapReleNumberToName(_kolokol4);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);
	unsigned long kDel4 = findNotaDelayForKolokolNumber(kNumber4);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;
	bool comboDS4 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo4");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	lcd.print(" ");
	lcd.print(kNumber4);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);
	digitalWrite(kName4, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((currentMill - startMill) >= kDel4){
			digitalWrite(kName4, LLL);
			comboDS4 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0) && (comboDS4 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo5(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
int _kolokol4 = 4,
int _kolokol5 = 5,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;
	int kNumber4 = _kolokol4;
	int kNumber5 = _kolokol5;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);
	int kName4 = remapReleNumberToName(_kolokol4);
	int kName5 = remapReleNumberToName(_kolokol5);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);
	unsigned long kDel4 = findNotaDelayForKolokolNumber(kNumber4);
	unsigned long kDel5 = findNotaDelayForKolokolNumber(kNumber5);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;
	bool comboDS4 = 1;
	bool comboDS5 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo5");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	lcd.print(" ");
	lcd.print(kNumber4);
	lcd.print(" ");
	lcd.print(kNumber5);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);
	digitalWrite(kName4, HHH);
	digitalWrite(kName5, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((currentMill - startMill) >= kDel4){
			digitalWrite(kName4, LLL);
			comboDS4 = 0;
		}

		if((currentMill - startMill) >= kDel5){
			digitalWrite(kName5, LLL);
			comboDS5 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0) && (comboDS4 == 0) 
		&& (comboDS5 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo6(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
int _kolokol4 = 4,
int _kolokol5 = 5,
int _kolokol6 = 6,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;
	int kNumber4 = _kolokol4;
	int kNumber5 = _kolokol5;
	int kNumber6 = _kolokol6;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);
	int kName4 = remapReleNumberToName(_kolokol4);
	int kName5 = remapReleNumberToName(_kolokol5);
	int kName6 = remapReleNumberToName(_kolokol6);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);
	unsigned long kDel4 = findNotaDelayForKolokolNumber(kNumber4);
	unsigned long kDel5 = findNotaDelayForKolokolNumber(kNumber5);
	unsigned long kDel6 = findNotaDelayForKolokolNumber(kNumber6);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;
	bool comboDS4 = 1;
	bool comboDS5 = 1;
	bool comboDS6 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo6");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	lcd.print(" ");
	lcd.print(kNumber4);
	lcd.print(" ");
	lcd.print(kNumber5);
	lcd.print(" ");
	lcd.print(kNumber6);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);
	digitalWrite(kName4, HHH);
	digitalWrite(kName5, HHH);
	digitalWrite(kName6, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((currentMill - startMill) >= kDel4){
			digitalWrite(kName4, LLL);
			comboDS4 = 0;
		}

		if((currentMill - startMill) >= kDel5){
			digitalWrite(kName5, LLL);
			comboDS5 = 0;
		}

		if((currentMill - startMill) >= kDel6){
			digitalWrite(kName6, LLL);
			comboDS6 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0) && (comboDS4 == 0) 
		&& (comboDS5 == 0) && (comboDS6 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo7(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
int _kolokol4 = 4,
int _kolokol5 = 5,
int _kolokol6 = 6,
int _kolokol7 = 7,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;
	int kNumber4 = _kolokol4;
	int kNumber5 = _kolokol5;
	int kNumber6 = _kolokol6;
	int kNumber7 = _kolokol7;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);
	int kName4 = remapReleNumberToName(_kolokol4);
	int kName5 = remapReleNumberToName(_kolokol5);
	int kName6 = remapReleNumberToName(_kolokol6);
	int kName7 = remapReleNumberToName(_kolokol7);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);
	unsigned long kDel4 = findNotaDelayForKolokolNumber(kNumber4);
	unsigned long kDel5 = findNotaDelayForKolokolNumber(kNumber5);
	unsigned long kDel6 = findNotaDelayForKolokolNumber(kNumber6);
	unsigned long kDel7 = findNotaDelayForKolokolNumber(kNumber7);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;
	bool comboDS4 = 1;
	bool comboDS5 = 1;
	bool comboDS6 = 1;
	bool comboDS7 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo7");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	lcd.print(" ");
	lcd.print(kNumber4);
	lcd.print(" ");
	lcd.print(kNumber5);
	lcd.print(" ");
	lcd.print(kNumber6);
	lcd.print(" ");
	lcd.print(kNumber7);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);
	digitalWrite(kName4, HHH);
	digitalWrite(kName5, HHH);
	digitalWrite(kName6, HHH);
	digitalWrite(kName7, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((currentMill - startMill) >= kDel4){
			digitalWrite(kName4, LLL);
			comboDS4 = 0;
		}

		if((currentMill - startMill) >= kDel5){
			digitalWrite(kName5, LLL);
			comboDS5 = 0;
		}

		if((currentMill - startMill) >= kDel6){
			digitalWrite(kName6, LLL);
			comboDS6 = 0;
		}

		if((currentMill - startMill) >= kDel7){
			digitalWrite(kName7, LLL);
			comboDS7 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0) && (comboDS4 == 0) 
		&& (comboDS5 == 0) && (comboDS6 == 0) && (comboDS7 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}

void combo8(
int _kolokol1 = 1,
int _kolokol2 = 2,
int _kolokol3 = 3,
int _kolokol4 = 4,
int _kolokol5 = 5,
int _kolokol6 = 6,
int _kolokol7 = 7,
int _kolokol8 = 8,
unsigned int _delayUntilNextNota = DEF_DEL_UNT_NEXT_NOTA){

	bool comboCikl = 1;

	int kNumber1 = _kolokol1;
	int kNumber2 = _kolokol2;
	int kNumber3 = _kolokol3;
	int kNumber4 = _kolokol4;
	int kNumber5 = _kolokol5;
	int kNumber6 = _kolokol6;
	int kNumber7 = _kolokol7;
	int kNumber8 = _kolokol8;

	int kName1 = remapReleNumberToName(_kolokol1);
	int kName2 = remapReleNumberToName(_kolokol2);
	int kName3 = remapReleNumberToName(_kolokol3);
	int kName4 = remapReleNumberToName(_kolokol4);
	int kName5 = remapReleNumberToName(_kolokol5);
	int kName6 = remapReleNumberToName(_kolokol6);
	int kName7 = remapReleNumberToName(_kolokol7);
	int kName8 = remapReleNumberToName(_kolokol8);

	unsigned long kDel1 = findNotaDelayForKolokolNumber(kNumber1);
	unsigned long kDel2 = findNotaDelayForKolokolNumber(kNumber2);
	unsigned long kDel3 = findNotaDelayForKolokolNumber(kNumber3);
	unsigned long kDel4 = findNotaDelayForKolokolNumber(kNumber4);
	unsigned long kDel5 = findNotaDelayForKolokolNumber(kNumber5);
	unsigned long kDel6 = findNotaDelayForKolokolNumber(kNumber6);
	unsigned long kDel7 = findNotaDelayForKolokolNumber(kNumber7);
	unsigned long kDel8 = findNotaDelayForKolokolNumber(kNumber8);

	bool comboDS1 = 1;  // digitalState
	bool comboDS2 = 1;
	bool comboDS3 = 1;
	bool comboDS4 = 1;
	bool comboDS5 = 1;
	bool comboDS6 = 1;
	bool comboDS7 = 1;
	bool comboDS8 = 1;

	lcd.clear();
	lcd.setCursor(3,0);
	lcd.write(byte(0));
	lcd.print(" combo8");
	lcd.setCursor(0,1);
	lcd.print(kNumber1);
	lcd.print(" ");
	lcd.print(kNumber2);
	lcd.print(" ");
	lcd.print(kNumber3);
	lcd.print(" ");
	lcd.print(kNumber4);
	lcd.print(" ");
	lcd.print(kNumber5);
	lcd.print(" ");
	lcd.print(kNumber6);
	lcd.print(" ");
	lcd.print(kNumber7);
	lcd.print(" ");
	lcd.print(kNumber8);
	
	digitalWrite(kName1, HHH);
	digitalWrite(kName2, HHH);
	digitalWrite(kName3, HHH);
	digitalWrite(kName4, HHH);
	digitalWrite(kName5, HHH);
	digitalWrite(kName6, HHH);
	digitalWrite(kName7, HHH);
	digitalWrite(kName8, HHH);

	startMill = millis();
	while(comboCikl){

		currentMill = millis();

		if((currentMill - startMill) >= kDel1){
			digitalWrite(kName1, LLL);
			comboDS1 = 0;
		}

		if((currentMill - startMill) >= kDel2){
			digitalWrite(kName2, LLL);
			comboDS2 = 0;
		}

		if((currentMill - startMill) >= kDel3){
			digitalWrite(kName3, LLL);
			comboDS3 = 0;
		}

		if((currentMill - startMill) >= kDel4){
			digitalWrite(kName4, LLL);
			comboDS4 = 0;
		}

		if((currentMill - startMill) >= kDel5){
			digitalWrite(kName5, LLL);
			comboDS5 = 0;
		}

		if((currentMill - startMill) >= kDel6){
			digitalWrite(kName6, LLL);
			comboDS6 = 0;
		}

		if((currentMill - startMill) >= kDel7){
			digitalWrite(kName7, LLL);
			comboDS7 = 0;
		}

		if((currentMill - startMill) >= kDel8){
			digitalWrite(kName8, LLL);
			comboDS8 = 0;
		}

		if((comboDS1 == 0) && (comboDS2 == 0) && (comboDS3 == 0) && (comboDS4 == 0) 
		&& (comboDS5 == 0) && (comboDS6 == 0) && (comboDS7 == 0) && (comboDS8 == 0)){
			comboCikl = 0;
		}
	}

	delay(_delayUntilNextNota);
}
