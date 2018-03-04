#define LCD_LIGHT_PIN 10  // подсветка дисплея

// НАСТРОЙКИ ИГРЫ

#define set_HEART_COUNT 5  // столько сердечек нужно собрать для победы

// НАСТРОЙКИ ИГРЫ END

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

byte heart1xPos = 0;  // х физическая позиция сердечек
byte heart1yPos = 0;  // у физическая позиция сердечек
byte heart1vPos = 0;  // виртуальная позиция сердечек
byte heart1mPos = 0;  // предыдущая виртуальная позиция сердечек
byte heartCount = 5;  // счетчик сердечек для индикации
byte loopKey;         // хранит состояние кнопки

byte human1xPos = 7;  // х позиция человечка
byte human1yPos = 0;  // у позиция человечка

byte humanChar0[8] = {  // человечек
  B01110,
  B01110,
  B00100,
  B01110,
  B10101,
  B00100,
  B01010,
  B11011
};

byte humanChar1[8] = {  // человечек с поднятыми руками
  B01110,
  B01110,
  B10101,
  B01110,
  B00100,
  B00100,
  B01010,
  B11011
};

byte heartChar2[8] = {  // сердечки
	0b00000,
	0b00000,
	0b01010,
	0b11111,
	0b11111,
	0b11111,
	0b01110,
	0b00100
};

byte lHomeChar3[8] = {  // левая половина дома
	0b00001,
	0b00010,
	0b00100,
	0b01000,
	0b11111,
	0b01000,
	0b01000,
	0b01111
};

byte rHomeChar4[8] = {  // правая половина дома
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b11111,
	0b00010,
	0b00010,
	0b11110
};

byte kaplaChar5[8] = {
	0b00100,
	0b00100,
	0b01110,
	0b01110,
	0b11101,
	0b11101,
	0b11101,
	0b01110
};

// функция проверки кнопок
byte key(){  // 1-723, 2-482, 3-133, 4-310, 5-0;
	int val = analogRead(0);
	if(val < 67) return 5;
	else if(val < 221) return 4;
	else if(val < 396) return 3;
	else if(val < 602) return 2;
	else if(val < 873) return 1;
	else if(val <= 1023) return 0;
}

// функция изменения картинки человечка
byte human1changePic(int _changeVal = 1){
	for(int i=0;i<_changeVal;i++){
		lcd.clear();
		lcd.setCursor(human1xPos, human1yPos);
		lcd.write(byte(1));
		delay(500);
		lcd.clear();
		lcd.setCursor(human1xPos, human1yPos);
		lcd.write(byte(0));	
	}
}

// изменение позиции человечка
byte human1changePos(byte _key){
	switch (_key) {
	    case 1:
	   			human1changePic();
	      break;
	    case 2:
	      	if(human1xPos > 0){
	      		human1xPos --;
	      		if(human1xPos < 0) human1xPos = 0;
	      	}
	      break;
	    case 3:
	      	if(human1yPos <= 1){
	      		human1yPos ++;
	      		if(human1yPos > 1) human1yPos = 1;
	      	}
	      break;
	    case 4:
	      	if(human1yPos != 0){
	      		human1yPos --;
	      		if(human1yPos < 0) human1yPos = 0;
	      	}
	      break;
	    case 5:
	      	if(human1xPos < 15){
	      		human1xPos ++;
	      		if(human1xPos > 15) human1xPos =15;
	      	}
	      break;
	}
	delay(200);
}
// функция назначения рандомной позиции сердечкам
void heartPosRand(){
	heart1xPos = random(0, 16);
	heart1yPos = random(0, 2);
	if(heart1yPos == 1 && heart1xPos > 13){
		heart1xPos = random(0, 14);
	}
	if(heart1xPos == human1xPos && heart1yPos == human1yPos){
		heartPosRand();
	}
}
// функция отрисовки сердечек рандом
void heartDrawRand(){
	
}

// масив для позиционирования сердечек
// 1-столбец, 2-строка
byte heart1xyPos[8][8] = {{12,3,9,5,1,0,0,0},{1,0,1,0,1,0,0,0}};

// функция отрисовки сердечек
void heartDraw(){
	heart1xPos = heart1xyPos[0][heart1vPos];  // значение Х из масива
	heart1yPos = heart1xyPos[1][heart1vPos];  // значение Y из масива

	lcd.setCursor(heart1xPos,heart1yPos);
	lcd.write(byte(2));
	lcd.setCursor(14,1);
	lcd.write(byte(3));
	lcd.write(byte(4));

	if(heart1xPos == human1xPos && heart1yPos == human1yPos){
		if(heart1vPos <= 4){	
			heart1vPos ++;
			heartCount --;
			human1changePic(3);
			lcd.clear();
			lcd.setCursor(14,1);
			lcd.print(byte(3));
			lcd.print(byte(4));
			lcd.setCursor(human1xPos, human1yPos);
	    	lcd.write(byte(0));
		}else{
			humanWin();
		}
	}
}

// функция вин. вызывается при окончании игры
void humanWin(){
	lcd.clear();
	lcd.setCursor(6, 0);
	lcd.write(byte(0));
	lcd.print("  ");
	lcd.write(byte(1));
	lcd.setCursor(2, 1);
	lcd.print("Your Win !!!");
	delay(600);
	lcd.clear();
	lcd.setCursor(6, 0);
	lcd.write(byte(1));
	lcd.print("  ");
	lcd.write(byte(0));
	lcd.setCursor(2, 1);
	lcd.print("Your Win !!!");
	delay(600);
}


void setup() {

	// инициализация пользовательских символов
  	lcd.createChar(0, humanChar0);
  	lcd.createChar(1, humanChar1);
  	lcd.createChar(2, heartChar2);
  	lcd.createChar(3, lHomeChar3);
  	lcd.createChar(4, rHomeChar4);
  	lcd.createChar(5, kaplaChar5);
 
  	lcd.begin(16, 2);
  	lcd.clear();

  	// яркость подсветки
  	// pinMode(LCD_LIGHT_PIN, OUTPUT);
  	// analogWrite(LCD_LIGHT_PIN, 55);
  	
  	lcd.setCursor(human1xPos, human1yPos);
  	lcd.write(byte(0));
  	delay(500);
  	heart1xyPos[1][1] = 0;
  	randomSeed(analogRead(1));
}

void loop() {
	loopKey = key();                              // хранит состояние кнопки
	if(loopKey > 0){                              // если кнопка нажата
		human1changePos(loopKey);                 // вызвать функцию смены позиции человечка
		lcd.clear();                              // очистить экран
		lcd.setCursor(human1xPos, human1yPos);    // установить курсор
	    lcd.write(byte(0));                       //
		heartDraw();                              //
	}else{heartDraw();}                           //
  	delay(50); 
  	loop();                                       //
}
