

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИИ ПРИЕМНИКА ДЛЯ ВСТРАИВАНИЯ В ПРОГРАММУ АУДИОГИД
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iarduino_IR_RX.h>                      // библиотека для работы с ИК-приёмником


iarduino_IR_RX IR_RECEIVER(7);                            // объект IR_RECEIVER, с указанием вывода к которому подключён ИК-приёмник

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА КАЖДЫЙ РАЗ КАК ЕГО ПОЛУЧАЕТ
int16_t generalIrReceiverF()
{
	if(IR_RECEIVER.check()){
		return IR_RECEIVER.data;
	}
	else{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА 1 РАЗ, СЛЕДУЮЩИЙ РАЗ ОНА ВЕРНЕТ 0 ИЛИ НОМЕР ДРУГОГО МАЯЧКА ЕСЛИ ОБНАРУЖИТ ЕГО
int16_t grwffPrevExhibitNumber = 0;
int16_t grwffExhibitNumber = 0;
int16_t generalIrReceiverWithFilterF()
{
	grwffExhibitNumber = generalIrReceiverF();
	if(grwffExhibitNumber > 0 && grwffExhibitNumber != grwffPrevExhibitNumber){
		grwffPrevExhibitNumber = grwffExhibitNumber;
		return grwffExhibitNumber;
	}
	else{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА КАЖДЫЙ РАЗ КАК ЕГО ПОЛУЧАЕТ 3 РАЗА ПОДРЯД И ОН ОДИНАКОВЫЙ (ДЛЯ ЗАЩИТЫ ОТ ПЕРЕОТРАЖЕНИЙ)
int16_t grf3PrevExhibitNumber = 0;
int16_t grf3ExhibitNumber = 0;
int8_t filterCounter = 0;
int16_t generalIrReceiverF3()
{
	grf3ExhibitNumber = generalIrReceiverF();
	if(grf3ExhibitNumber > 0 && grf3ExhibitNumber != grf3PrevExhibitNumber){
		grf3PrevExhibitNumber = grf3ExhibitNumber;
		filterCounter = 1;
	}
	else if(grf3ExhibitNumber > 0 && grf3ExhibitNumber == grf3PrevExhibitNumber){
		filterCounter ++;
		if(filterCounter >= 3){
			filterCounter = 0;
			return grf3ExhibitNumber;
		}
	}
	else{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА 1 РАЗ, СЛЕДУЮЩИЙ РАЗ ОНА ВЕРНЕТ 0 ИЛИ НОМЕР ДРУГОГО МАЯЧКА ЕСЛИ ОБНАРУЖИТ ЕГО (С ЗАЩИТОЙ ОТ ПЕРЕОТРАЖЕНИЙ)
int16_t grwff3PrevExhibitNumber = 0;
int16_t grwff3ExhibitNumber = 0;
int16_t generalIrReceiverWithFilterF3()
{
	grwff3ExhibitNumber = generalIrReceiverF3();
	if(grwff3ExhibitNumber > 0 && grwff3ExhibitNumber != grwff3PrevExhibitNumber){
		grwff3PrevExhibitNumber = grwff3ExhibitNumber;
		return grwff3ExhibitNumber;
	}
	else{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










// ТЕСТ ЧЕРЕЗ UART

int16_t exhibitNumber = 0;

void setup()
{
	Serial.begin(115200);
	
	IR_RECEIVER.begin();                                    // Инициируем работу с ИК-приёмником
	IR_RECEIVER.protocol("AeQQV~zK]Kp^KJp[@@@@@@@Bp");      // протокол передачи данных от пульта ELENBERG
}

void loop()
{
	/* ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА КАЖДЫЙ РАЗ КАК ЕГО ПОЛУЧАЕТ */
	exhibitNumber = generalIrReceiverF();
	if(exhibitNumber > 0){
		Serial.print("receive n_ ");
		Serial.println(exhibitNumber);
	}

	// /* ФУНКЦИЯ ВОЗВРАЩЯЕТ НОМЕР МАЯЧКА 1 РАЗ, СЛЕДУЮЩИЙ РАЗ ОНА ВЕРНЕТ 0 ИЛИ НОМЕР ДРУГОГО МАЯЧКА ЕСЛИ ОБНАРУЖИТ ЕГО */
	// exhibitNumber = generalIrReceiverWithFilterF();
	// if(exhibitNumber > 0){
	// 	Serial.print("play file_ ");
	// 	Serial.println(exhibitNumber);
	// }
}