// ФУНКЦИИ ДЛЯ СОЕДЕНЕНИЯ ПО БЛЮТУЗ

#define bluetooth Serial1 // блютуз подключен к Serial1

void GyroBalance();
void forward();

//////////////////////////////////////////////////////////////////////////////

void bluetoothSetup(){   // ВЫЗВАТЬ ЭТУ ФУНКЦИЮ В void setup ДЛЯ ИНИЦИАЛИЗАЦИИ

	bluetooth.begin(9600);
}

//////////////////////////////////////////////////////////////////////////////

char direction = 'x';
char b_d = 'x';

void bluetoothLoop(){   // ВЫЗВАТЬ ЭТУ ФУНКЦИЮ В void loop ДЛЯ РАБОТЫ С БЛЮТУЗ

	// ЗДЕСЬ НАСТРАИВАЕТСЯ ПОВЕДЕНИЕ ПРИ НАЖАТИИ КНОПОК
	// СТРОЧКИ С ':' ПОД НИМИ МОЖНО ПИСАТЬ КОМАНДЫ ЧТО БУДУТ ВЫПОЛНЯТСЯ

	if(bluetooth.available()){

		b_d = bluetooth.read();

		if(b_d == 'S'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА "Leg Calibration":
			Serial.println("press S");
		}
		else if(b_d == 'U'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА ВПЕРЕД ВЫПОЛНЯЕТСЯ 1 РАЗ КОГДА НАЖИМАЕШЬ: ↑
			direction = b_d;
			Serial.println("press U");
		}
		else if(b_d == 'D'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА НАЗАД ВЫПОЛНЯЕТСЯ 1 РАЗ КОГДА НАЖИМАЕШЬ: ↓
			direction = b_d;
			Serial.println("press D");
		}
		else if(b_d == 'L'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА ВЛЕВО ВЫПОЛНЯЕТСЯ 1 РАЗ КОГДА НАЖИМАЕШЬ: ←
			direction = b_d;
			Serial.println("press L");
		}
		else if(b_d == 'R'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА ВПРАВО ВЫПОЛНЯЕТСЯ 1 РАЗ КОГДА НАЖИМАЕШЬ: →
			direction = b_d;
			Serial.println("press R");
		}
		else if(b_d == 'x'){
			// ДЕЙСТВИЕ ЕСЛИ ОТЖАТА КНОПКА ДЖОЙСТИКА:
			direction = b_d;
			Serial.println("release X");
		}
		else if(b_d == 'G'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА "Gyro Balance":
			Serial.println("press G");
			GyroBalance();
		}
		else if(b_d == 'V'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА КОРПУС ВВЕРХ:
			Serial.println("press V");
		}
		else if(b_d == 'N'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА КНОПКА КОРПУС ВНИЗ:
			Serial.println("press N");
		}
		else if(b_d == 'A'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА ДОПОЛНИТЕЛЬНАЯ КНОПКА A:
			Serial.println("press A");
		}
		else if(b_d == 'B'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА ДОПОЛНИТЕЛЬНАЯ КНОПКА B:
			Serial.println("press B");
		}
		else if(b_d == 'C'){
			// ДЕЙСТВИЕ ЕСЛИ НАЖАТА ДОПОЛНИТЕЛЬНАЯ КНОПКА C:
			Serial.println("press C");
		}
	}

	if(direction == 'U'){
		// ДЕЙСТВИЕ ЕСЛИ НАЖАТА И УДЕРЖИВАЕТСЯ КНОПКА ВПЕРЕД, ВЫПОЛНЯЕТСЯ ПОКРУГУ ПОКА НАЖАТА КНОПКА ВПЕРЕД: ↑↑
		Serial.println("hold U");
		forward();
	}
	else if(direction == 'D'){
		// ДЕЙСТВИЕ ЕСЛИ НАЖАТА И УДЕРЖИВАЕТСЯ КНОПКА НАЗАД, ВЫПОЛНЯЕТСЯ ПОКРУГУ ПОКА НАЖАТА КНОПКА НАЗАД : ↓↓
		Serial.println("hold D");
	}
	else if(direction == 'L'){
		// ДЕЙСТВИЕ ЕСЛИ НАЖАТА И УДЕРЖИВАЕТСЯ КНОПКА ВЛЕВО, ВЫПОЛНЯЕТСЯ ПОКРУГУ ПОКА НАЖАТА КНОПКА ВЛЕВО : ←←
		Serial.println("hold L");
	}
	else if(direction == 'R'){
		// ДЕЙСТВИЕ ЕСЛИ НАЖАТА И УДЕРЖИВАЕТСЯ КНОПКА ВПРАВО, ВЫПОЛНЯЕТСЯ ПОКРУГУ ПОКА НАЖАТА КНОПКА ВПРАВО: →→
		Serial.println("hold R");
	}
}

//////////////////////////////////////////////////////////////////////////////

void bluetoothPrintVoltBat(byte voltBat = 100){   //  ЭТА ФУНКЦИЯ ДЛЯ ОТПРАВКИ % ЗАРЯДА БАТАРЕИ

	if(voltBat > 100){voltBat = 100;}
	bluetooth.print("*P");
	bluetooth.print(voltBat);
	bluetooth.println("*");
}