// Видеообзоры и уроки работы с ARDUINO на YouTube-канале IOMOIO: https://www.youtube.com/channel/UCmNXABaTjX_iKH28TTJpiqA

// Библиотеки и определения для DS3231
#include <Wire.h>       // Подключаем библиотеку для работы с I2C-устройствами
#include <DS3231.h>     // Подключаем библиотеку для работы с модулем DS3231

DS3231 clock;           // Связываем объект clock с библиотекой DS3231
RTCDateTime DateTime;   // Определяем переменную DateTime, как описанную структурой RTCDateTime (структура описана в библиотеке DS3231.h)
//
// Библиотеки и определения для дисплея
#include <SPI.h>            // Подключаем библиотеку для работы с устройством по интерфейсу SPI
#include <Adafruit_GFX.h>   // Подключаем библиотеку для работы с текстом и графикой
#include <TFT_ILI9163C.h>   // Подключаем библиотеку программных драйверов для работы с дисплеем 1.44 128x128 SPI

// В следующих 8-и строках опеределяем названия и соответствующие им RGB обозначения цветов, которые будем выводить на дисплей
#define BLACK   0x0000
#define GREEN   0x07E0
#define YELLOW  0xFFE0  
#define LYELLOW 0xFFCE
#define WHITE   0xFFFF

#define __CS 10                                           // Определяем пин к которому подключен разъем CS дисплея
#define __DC 9                                            // Определяем пин к которому подключен разъем A0 дисплея
#define __RES 8                                           // Определяем пин к которому подключен разъем RESET дисплея
TFT_ILI9163C display = TFT_ILI9163C(__CS, __DC, __RES);   // Связываем объект display с библиотекой TFT_ILI9163C и сообщаем библиотеке определенные для работы с дисплеем пины
//

int minute;                                               // Переменная для отслеживания изменения минут

void setup()
{
  display.begin();      // Иницализируем дисплей
  clock.begin();        // Инициализируем DS3231
  display.fillScreen(); // Очищаем дисплей перед выводом

  clock.setDateTime(__DATE__, __TIME__);                  // Устанавливаем время на часах, основываясь на времени компиляции скетча

  minute = 61;                                            // Присваиваем переменной с текущим значением минут заведомо большее возможного значение
}

void loop()
{
  DateTime = clock.getDateTime();                                   // Заполняем DateTime значениями, полученными при запросе текущего времени
  if (int(DateTime.minute) != minute) {                             // Если текущее целочисленное значение минут отличается от значения в переменной отслеживания
    minute = int(DateTime.minute);                                  //  Меняем значение в переменной отслеживания минут на текущее
    display.fillScreen();                                           //  Очищаем дисплей перед выводом
    display.setTextSize(1);                                         //  Определяем размер шрифта
    display.setTextColor(YELLOW);                                   //  Определяем цвет вывода на дисплей
    display.setCursor(5,5);                                         //  Устанавливаем курсор в левый верхний угол области вывода
    display.print(clock.dateFormat("d.m ",  DateTime));             //  Выводим отформатированную дату
    display.print(utf8rus(day_of_week(DateTime.dayOfWeek)));        //  Выводим обозначение дня недели на русском
    display.setCursor(100,5);                                       //  Устанавливаем курсор в левый верхний угол области вывода
    display.print(clock.readTemperature(),0);                       //  Выводим текущее значение температуры
    display.print(utf8rus("\x80\C"));                               //  Выводим значок градуса и символ С
    display.setTextSize(3);                                         //  Определяем размер шрифта
    display.setTextColor(LYELLOW);                                  //  Определяем цвет вывода на дисплей
    display.setCursor(19,47);                                       //  Устанавливаем курсор в левый верхний угол области вывода
    display.print(clock.dateFormat("H:i",  DateTime));              //  Выводим текущее время
  }
  for (int i=110; i < 115; i++) {                                   // Цикл для вывода индикатора секунд (меняется значение координаты Y для отрисовки)
    display.drawLine (4, i, DateTime.second*2+4, i, GREEN);         // Рисуем линию индикации секунд 
  }
  delay(1000);                                                      // Задержка в 1 секунду
}

// Начало функции определения написания дня недели
String day_of_week(int num_day_of_week) {
  String value;
  if (num_day_of_week == 1)
    value = "Пн";
  if (num_day_of_week == 2)
    value = "Вт";
  if (num_day_of_week == 3)
    value = "Ср";
  if (num_day_of_week == 4)
    value = "Чт";
  if (num_day_of_week == 5)
    value = "Пт";
  if (num_day_of_week == 6)
    value = "Сб";
  if (num_day_of_week == 7)
    value = "Вс";
  return value;
}
// Конец функции определения написания дня недели

// Начало функции обработки кириллических символов
String utf8rus(String source)     // Функция для конвертации русских символов из кодировки CP1251 в UTF8
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };
  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
 
    if (n >= 0xBF){
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}
// Конец функции обработки кириллических симоволов
