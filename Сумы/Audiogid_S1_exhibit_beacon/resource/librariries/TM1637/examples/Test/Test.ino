/*
  // нумерация портов слева направо!
  disp.display(port, num);      // вывести цифру num (0-9) в порт port (0-3)
  disp.display(ARRAY);          // вывести ARRAY, где ARRAY - массив из 4х цифр ( byte ARRAY[4] = {0, 1, 2, 3}; )
  disp.displayByte(port, Byte); // вывести символ Byte в порт port (0-3) (Byte вида 0x7d)
  disp.displayByte(ARRAY_BYTE); // ARRAY_BYTE - массив кодов ( 0x7d, 0x7d, 0x7d, 0x7d )
  disp.displayByte(Byte1, Byte2, Byte3, Byte4); // вывести побайтово
  // для вывода некоторых букв доступен вывод вида _A, _b, _h. Полный список смотри в TM1637.h
  disp.displayInt(value);       // вывести 1-4 значное число
  disp.displayIntZero(value);   // вывести 1-4 значное число с нулями СЛЕВА

  // бегущая строка символов из массива arrayBytes, число символов sizeof(arrayBytes), интервал delay_time в миллисекундах
  disp.runningString(arrayBytes, sizeof(arrayBytes), delay_time); 

  disp.point(state);        // включить/выключить двоеточие (1 и 0 соответственно)

  disp.clearDisplay();      // очистить дисплей
  disp.set(7);              // яркость, 0 - 7 (минимум - максимум)
*/

#include "TM1637.h"

#define CLK 3
#define DIO 2

TM1637 disp(CLK, DIO);

byte i = 0;

void setup() {
  Serial.begin(9600);
  disp.init();  // инициализация
  disp.set(7);  // яркость, 0 - 7 (минимум - максимум)
}

void loop() {
    byte welcome_banner[] = {_t, _E, _S, _t, _empty, _empty, _O, _F, _empty, _d, _i, _S, _P, _L, _a, _y};
    disp.runningString(welcome_banner, sizeof(welcome_banner), 300);
    delay(700);
    disp.clearDisplay();

    disp.displayByte(0x38, 0x79, 0x5e, 0x6d);  // вывод символов и текста
    delay(2000);
    disp.clearDisplay();

    disp.displayByte(_L, _E, _d, _S);   // вывести LEdS, используя буквы из библиотеки
    delay(2000);
    disp.clearDisplay();

    i = 12;  // вывести переменную с нулями слева
    disp.displayIntZero(i);
    delay(2000);
    disp.clearDisplay();
    
    i = 48;  // вывести переменную без нулей слева
    disp.displayInt(i);
    delay(2000);
    disp.clearDisplay();

    for (int j = 0; j < 1235; j += 5) {  // простой счётчик 0-1235 без нулей
        disp.displayInt(j);
        delay(20);
    }
    disp.clearDisplay();

    for (byte i = 0; i <= 3; i++) {       // вывод цифр 0-9 по очереди в каждый порт
        for (byte j = 0; j <= 9; j++) {
            disp.display(i, j);
            delay(100);
        }
        disp.clearDisplay();
    }
    disp.clearDisplay();
    
    for (byte j = 0; j < 10; j++) {
        disp.displayByte(0x64, 0x64, 0x64, 0x64);
        delay(100);
        disp.displayByte(0x49, 0x49, 0x49, 0x49);
        delay(100);
        disp.displayByte(0x52, 0x52, 0x52, 0x52);
        delay(100);
    }

    for (byte j = 0; j < 10; j++) {
        disp.displayByte(0x24, 0x24, 0x24, 0x24);
        delay(100);
        disp.displayByte(0x09, 0x09, 0x09, 0x09);
        delay(100);
        disp.displayByte(0x12, 0x12, 0x12, 0x12);
        delay(100);
    }
    
    disp.clearDisplay();
    
    for (byte j = 0; j < 5; j++) {
        disp.displayByte(1, 0x01);
        delay(100);
        disp.displayByte(1, 0x02);
        delay(100);
        disp.displayByte(1, 0x40);
        delay(100);
        disp.displayByte(1, 0x10);
        delay(100);
        disp.displayByte(1, 0x08);
        delay(100);
        disp.displayByte(1, 0x04);
        delay(100);
        disp.displayByte(1, 0x40);
        delay(100);
        disp.displayByte(1, 0x20);
        delay(100);
    }

    disp.clearDisplay();
    
    
    for (byte j = 0; j < 255; j++) {  // вывести 0-99 счётчик с нулями слева
        disp.displayByte(1, j);
        delay(200);
    }
    disp.clearDisplay();
    delay(1000);
    
}
