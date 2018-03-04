// проект автозапуск бензинового генератора на ARDURINO Ver 090717


    //задаю константы

const int analogPin = 3;     // номер порта к которому подключен датчик уровня топлива
 
const int city = 2;  // напряжение из города

const  int he_voltage = 4; //напряжение на генераторе

const int starter_relay = 5;              //реле стартера

const int choke_on = 6;                   // подсос включить

const int choke_of = 7;                   // подсос выключить

const int magnetic = 8;                  // включает пускатель

const int  disconnection = 9;            //реле датчика масла

const int magnetic_of = 10;                  // вЫключает пускатель
const int ledPin_11 = 11;     // номер порта к которому подключен желтый диод
const int ledPin_12 = 12;     // номер порта к которому подключен красный диод
const int ledPin_13 = 13;     // номер порта к которому подключен авария диод
const int ton = 1;            // номер порта к которому подключена пищалка



// задаю переменные

int t = 0; //отображает работу генератора
int m = 0; //отображает наличие напряжения в сети
int a = 0; //количество попыток запуска ген
int val = 0;  // переменная для хранения считываемого значения с
int rav = 0;  //переменная для хранения диапозона уровня топлива
int pop = 0; //отображает тревогу
int g = 0; //количество команд на пускатель

void setup()

{
  //pinMode(ledPin_4, OUTPUT);      // устанавливает режим работы - выход
  pinMode(ledPin_11, OUTPUT);      // устанавливает режим работы - выход
  pinMode(ledPin_12, OUTPUT);      // устанавливает режим работы - выход
  pinMode(ledPin_13, OUTPUT);      // устанавливает режим работы - выход
  pinMode(ton, OUTPUT);           // устанавливает режим работы - выход
  
  pinMode(city, INPUT);         // назначаем  входом  напряжение из города  в схеме
                  
  pinMode(he_voltage, INPUT);         // назначаем the_voltage входом  напряжение с генератора

  pinMode(starter_relay, OUTPUT);         //назначаем  starter_relay выходом реле стартера

  pinMode(choke_on, OUTPUT);         //назначаем choke_on выходом подсос включает

  pinMode(choke_of, OUTPUT);         //назначаем choke_of выходом подсос вЫключает

  pinMode(magnetic, OUTPUT);         //назначаем magnetic выходом переключает пускатели на генератор

  pinMode(disconnection, OUTPUT);         //назначаем disconnection выходом включает и выключает  зажигание
 
  pinMode(magnetic_of, OUTPUT);         //назначаем magnetic выходом переключает пускатели на генератор

  // Serial.begin(9600);              //  установка связи по serial
digitalWrite(magnetic, HIGH);
digitalWrite(magnetic_of, HIGH);
digitalWrite(disconnection, HIGH);
digitalWrite(starter_relay, HIGH);


}

void loop()
{
 //  val = analogRead(analogPin);     // считываем значение уровня топлива
// Serial.println(rav);             // выводим полученное значение

// if (val<180)   rav = 1 ;
// if (val>120 && val < 350)    rav = 2 ;
// if (val>340 && val < 420)   rav = 3 ;
// if (val>500)  rav = 4 ;
rav=4; // пока датчика топлива нет задаем что бак полный

// Serial.println(val);
 // if (rav == 1) digitalWrite(ledPin_4, HIGH);   // включает светодиод5
  if (rav==2) digitalWrite(ledPin_11, HIGH);   // включает светодиод6
if (rav==3)  digitalWrite(ledPin_12, HIGH);   // включает светодиод7
  if (rav==4 || a >= 4 )  digitalWrite(ledPin_13, HIGH);   // включает светодиод8
delay(1000);                  // ожидаем
         digitalWrite(ledPin_11, LOW);   // выключает светодиод6
     digitalWrite(ledPin_12, LOW);   // выключает светодиод6
     digitalWrite(ledPin_13, LOW);   // выключает светодиод6
// Serial.println(rav);             // выводим полученное значение

  // Serial.println(a);
  
   m =  digitalRead(city);
   t =  digitalRead(he_voltage);
   if (t == LOW && m == LOW && a < 4 && rav != 3)

{
  if (a==0)
  {
   digitalWrite(ton, HIGH);         //подаюм команду  тон
    delay(2000);                           //пауза 2 сек
    digitalWrite(ton, LOW);       //подаюм команду - тон
  }
  digitalWrite(disconnection, LOW);         //подаюм команду + на включение зажигания
    delay(700);                           //пауза 0,7 сек
  digitalWrite(choke_on, HIGH);         //подаюм команду + на открытие подсоса
    delay(700);                           //пауза 0,7 сек
    digitalWrite(choke_on, LOW);       //прекращаем команду - на открытие подсоса
    delay(1000);
    digitalWrite(starter_relay, LOW);         //подаюм команду + на реле стартера
    delay(1800);                           //пауза 1,8сек
    digitalWrite(starter_relay, HIGH);         //прекращаем команду - на реле стартера
    delay(2500);
    digitalWrite(choke_of, HIGH);        //подаюм команду +  на закрытие  подсоса
    delay(500);                           //пауза 0,5 сек
    digitalWrite(choke_of, LOW);        //прекращаем команду - на закрытие подсоса
    a++;
  
}
if (t == LOW && m == LOW  && rav == 3 && pop == 0 || t == LOW && m == LOW && a >= 4 && pop == 0 )
{
digitalWrite(ton, HIGH);         //подаюм команду  тон
    delay(1500);                           //пауза 2 сек
    digitalWrite(ton, LOW);       //подаюм команду - тон
    delay(1000);
    digitalWrite(ton, HIGH);         //подаюм команду  тон
    delay(1500);                           //пауза 2 сек
    digitalWrite(ton, LOW);       //подаюм команду - тон
    delay(1000);
    digitalWrite(ton, HIGH);         //подаюм команду  тон
   delay(1500);                           //пауза 2 сек
    digitalWrite(ton, LOW);       //подаюм команду - тон
    delay(1000);
    digitalWrite(ton, HIGH);         //подаюм команду  тон
   delay(1500);                           //пауза 2 сек
    digitalWrite(ton, LOW);       //подаюм команду - тон
    pop++;
    digitalWrite(disconnection, HIGH);
}
if (t == HIGH && m == LOW && g == 0 )
{
   digitalWrite(disconnection, LOW);
   delay(60000);
   digitalWrite(magnetic, LOW);
delay(1000);  //пауза 1 сек
digitalWrite(magnetic, HIGH);
g++;
}
if (t == HIGH && m == HIGH || rav == 3 && t == HIGH )
{
digitalWrite(magnetic_of, LOW);
delay(1000);  //пауза 1 сек
digitalWrite(magnetic_of, HIGH);
delay(4000);  //пауза 4 сек
digitalWrite(disconnection, HIGH);
  g = 0;
}
if (t == LOW && m == HIGH || t == HIGH && m == LOW )
{
   a = 0;
 
}
if (t == LOW && m == HIGH || t == HIGH && m == LOW )
{
   pop = 0;
 
}

}
