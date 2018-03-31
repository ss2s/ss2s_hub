 // Скетч преобразования координат конечности в углы наклона серв. Последняя редакция: 
 -----------------------------------------------------------------------------------------------------------------------------------
 ЗДЕСЬ ЧИСТО ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ
 
#include <Servo.h>   // подключаем библиотеку управления сервами
Servo tibia1, femur1, coxa1,tibia2, femur2, coxa2,tibia3, femur3, coxa3; // создаём объекты серв 
Servo tibia4, femur4, coxa4,tibia5, femur5, coxa5,tibia6, femur6, coxa6; 
int  tibia, femur, coxa, z0, legdistanceX,legdistanceY; // объявляем глобальные константы конечности и туловища
int x,s,td1,td2; // объявляем параметры для движений вперёд/назад
int c1,f1,t1,c2,f2,t2,c3,f3,t3,c4,f4,t4,c5,f5,t5,c6,f6,t6; // объявляем номара входов для конечностей
int zbalance; // высота, на которой происходит вращение корпуса
float y;
----------------------------------------------------
 // объявляем переменные со значениями углов:
int VRxAngle, VRyAngle;
float alfa1_1,alfa2_1, beta1, gamma1; 
float alfa1_2,alfa2_2, beta2, gamma2; 
float alfa1_3,alfa2_3, beta3, gamma3; 				ЭТОТ КУСОК ДЛЯ ИСПользования. ИНТЕРФЕЙСА i2C, 
float alfa1_4,alfa2_4, beta4, gamma4; 				с помощью этого интерфейса можно было управлять туловищем. Без какого-либо управления 
													туловищем управлять невозможно просто откоментировав функцию
float alfa1_5,alfa2_5, beta5, gamma5; 				так что и2с тут большинство функцию выполняло. все что можно чисто без управления делать, тупо зациклить хождение вперед, калибровку конечностей, и поднятие корпуса 
float alfa1_6,alfa2_6, beta6, gamma6; 
------------------------------------------------------------
 // объявляем переменные со значением координат в новой системе и 
 // переменные L со значением длин отрезков от начала femur до конца конечности 
float x1,y1,z1,y11,L1;   
float x2,y2,z2,y22,L2;  
float x3,y3,z3,y33,L3;  
float x4,y4,z4,y44,L4;  
float x5,y5,z5,y55,L5;  
float x6,y6,z6,y66,L6;                                                                                 

//  Wire.begin(4);                // join i2c bus with address #4
 // Wire.onReceive(receiveEvent); // register event
 ---------------------------------------------------------------------------------------------------------------------------
НАСТРАИВАЕМ ПИНЫ НА ARDUINO MEGA 2560

void setup() {
  coxa=30; femur=60; tibia=60; z0=20; legdistanceX=203; legdistanceY=115; // задаём геометрические размеры конечности и туловища
 // приводим в соответствие цифровым входам приводы на лапах:
  c1=11; f1=10; t1=9; // 1-ая нога 
  c2=45; f2=47; t2=43; // 2-ая нога 
  c3=38; f3=34; t3=36; // 3-ая нога 
  c4=35; f4=31; t4=33; // 4-ая нога 
  c5=37; f5=41; t5=39; // 5-ая нога 
  c6=32; f6=30; t6=28; // 6-ая нога 
 // настраеваем цифровые выходы 22, 24, 26 на меге соответственно для coxa1, femur1, tibia1 
  pinMode(c1, OUTPUT); pinMode(f1, OUTPUT); pinMode(t1, OUTPUT); // 1-ая нога 
 // далее по аналогии: 
  pinMode(c2, OUTPUT); pinMode(f2, OUTPUT); pinMode(t2, OUTPUT); // 2-ая нога 
  pinMode(c3, OUTPUT); pinMode(f3, OUTPUT); pinMode(t3, OUTPUT); // 3-ая нога  
  pinMode(c4, OUTPUT); pinMode(f4, OUTPUT); pinMode(t4, OUTPUT); // 4-ая нога  
  pinMode(c5, OUTPUT); pinMode(f5, OUTPUT); pinMode(t5, OUTPUT); // 5-ая нога  
  pinMode(c6, OUTPUT); pinMode(f6, OUTPUT); pinMode(t6, OUTPUT); // 6-ая нога  
  Serial.begin(9600); 
 // присоединяем выходы к сервам лапки:   
 tibia1.attach(t1); femur1.attach(f1); coxa1.attach(c1); // 1-ая нога 
 tibia2.attach(t2); femur2.attach(f2); coxa2.attach(c2); // 2-ая нога 
 tibia3.attach(t3); femur3.attach(f3); coxa3.attach(c3); // 3-ая нога 
 tibia4.attach(t4); femur4.attach(f4); coxa4.attach(c4); // 4-ая нога 
 tibia5.attach(t5); femur5.attach(f5); coxa5.attach(c5); // 5-ая нога 
 tibia6.attach(t6); femur6.attach(f6); coxa6.attach(c6); // 6-ая нога   
 

}
------------------------------------------------------------------------------------------------------------------------------
void loop() {     // основной код Андромедки (здесь разрабатываем алгоритм хотьбы)
---------------------------------------------
 /*  Serial.println(VRxAngle); 
   Serial.println(VRyAngle); 			ТРИ ЭТИХ ПЕРЕМЕННЫХ БЫЛИ ДЛЯ i2c
   Serial.println(zbalance);  */
-------------------------------------------
//   GyroBalance(); 		ФУНКЦИЯ УПРАВЛЕНИЯ ТУЛОВИЩЕМ (ТО ЧТО НАДО ДОБАВИТЬ В БЛЮТУЗ), ИХ ОЧЕРЕДНОСТЬ НАПИШУ В КОНЦЕ
----------------------------------------------------------
  //  LegCalibration(); 	КАЛИБРОВКА КОНЕЧНОСТЕЙ (ТО ЧТО НАДО ДОБАВИТЬ В БЛЮТУЗ)
-----------------------------------------------------------  
  forward();
     
     CalculationOfAngles1(x3,y3,z3); 
     CalculationOfAngles1(x3,y3,z3); 
     CalculationOfAngles1(x3,y3,z3); 
     CalculationOfAngles1(x3,y3,z3); 	ДВИЖЕНИЕ ВПЕРЕД (ТО ЧТО НАДО ДОБАВИТЬ В БЛЮТУЗ)
     CalculationOfAngles1(x3,y3,z3); 
     CalculationOfAngles1(x3,y3,z3); 
--------------------------------------------------
//    updown();		КОНЕЧНОСТИ ПОДНИМАЮТ И ОПУСКАЮТ КОРПУС (ТО ЧТО НАДО ДОБАВИТЬ В БЛЮТУЗ)
---------------------------------------------------------
//  difangle();		НЕ ОБЯЗАТЕЛЬНАЯ ФУНКЦИЯ (НО Я НЕ УВЕРЕН, ПРОСТО МЕНЯЕТ УГЛЫ, НО ДОБАВИТЬ НУЖНО ЕЕ)

}


-----------------------------------------------------------------------------------------------------------------------------
ЗДЕСЬ ВЫПОЛНЯЮТСЯ РАССЧЕТЫ ИНВЕРСНОЙ КИНЕМАТИКИ, ЧТО-БЫ НЕ СОЗДАВАТЬ ОГРОМНЫЙ МАССИВ, В КОТОРОМ БУДУТ СОТНИ УГЛОВ КАК ПОВЕРНУТЬ КОНЕЧНОСТИ
АЛГОРИТМ САМ СЧИТАЕТ И ПО КОНЦУ КОНЕЧНОСТИ, ДРУГИЕ СЕРВЫ ПОДСТРАИВАЮТСЯ ПОД КОНЧИК ЛАПЫ. 6 РАССЧЕТОВ НА КАЖДУЮ ЛАПУ

 // функции преобразования координат в углы для каждой конечности:
void CalculationOfAngles1(float x1,float y1,float z1)
{
  // Проверяем, нет ли координат, при которой тангенс обращается в бесконечность:   
    if (y1==0) {y1=1;} 
 // вычисляем угол конечности в плоскости YoX и округляем до целого и переводим из радиан 
 // в градусы (+ 0.5 для того, чтобы округлялось по правилам алгебры): 
     gamma1=atan(x1/y1)*180/3.14159; 
 // вводим новую систему координат, учитывая изменение y при повороте ноги в YoX:      
     y11=y1/cos(gamma1*3.14159/180);  
 // Вводим поправку, в случае, если на входе координата y отрицательна:
    if (y1<0) {y11=abs(y1)/(cos(abs(gamma1)*3.14159/180)); gamma1= 180+gamma1;}
 // вычисляем длину отрезка от начала femur до конца tibia:
     L1=sqrt(sq(z0-z1)+sq(y11-coxa));
 // вычисляем угол между нормалью из точки начала femur и l:
     alfa1_1=asin((y11-coxa)/L1)*180/3.14159;
 // вычисляем угол между l и femur:
     alfa2_1=acos((sq(femur)+sq(L1)-sq(tibia))/(2*femur*L1))*180/3.14159;
 // вычисляем угол между femur и tibia:
     beta1=acos((sq(femur)+sq(tibia)-sq(L1))/(2*tibia*femur))*180/3.14159;
 // округляем вычисленные углы:
    gamma1=(int)(gamma1+0.5);
    alfa1_1=(int)(alfa1_1+0.5);
    alfa2_1=(int)(alfa2_1+0.5);
    beta1=(int)(beta1+0.5);
 // передаём вычисленные углы сервам:
     coxa1.write(180-gamma1);
     femur1.write(alfa1_1+alfa2_1);
     tibia1.write(beta1); 
}
 // далее по аналогии для остальных конечностей:
void CalculationOfAngles2(float x2,float y2,float z2)
{  
    if (y2==0) {y2=1;} 
    gamma2=atan(x2/y2)*180/3.14159;      
    y22=y2/cos(gamma2*3.14159/180);  
    if (y2<0) {y22=abs(y2)/(cos(abs(gamma2)*3.14159/180)); gamma2= 180+gamma2;}
    L2=sqrt(sq(z0-z2)+sq(y22-coxa));
    alfa1_2=asin((y22-coxa)/L2)*180/3.14159;
    alfa2_2=acos((sq(femur)+sq(L2)-sq(tibia))/(2*femur*L2))*180/3.14159;
    beta2=acos((sq(femur)+sq(tibia)-sq(L2))/(2*tibia*femur))*180/3.14159;
    gamma2=(int)(gamma2+0.5);
    alfa1_2=(int)(alfa1_2+0.5);
    alfa2_2=(int)(alfa2_2+0.5);
    beta2=(int)(beta2+0.5);
    coxa2.write(180-gamma2);
    femur2.write(alfa1_2+alfa2_2);
    tibia2.write(beta2); 
}
void CalculationOfAngles3(float x3,float y3,float z3)
{  
    if (y3==0) {y3=1;}  
    gamma3=atan(x3/y3)*180/3.14159;      
    y33=y3/cos(gamma3*3.14159/180);  
    if (y3<0) {y33=abs(y3)/(cos(abs(gamma3)*3.14159/180)); gamma3= 180+gamma3;}
    L3=sqrt(sq(z0-z3)+sq(y33-coxa));
    alfa1_3=asin((y33-coxa)/L3)*180/3.14159;
    alfa2_3=acos((sq(femur)+sq(L3)-sq(tibia))/(2*femur*L3))*180/3.14159;
    beta3=acos((sq(femur)+sq(tibia)-sq(L3))/(2*tibia*femur))*180/3.14159;
    gamma3=(int)(gamma3+0.5);
    alfa1_3=(int)(alfa1_3+0.5);
    alfa2_3=(int)(alfa2_3+0.5);
    beta3=(int)(beta3+0.5);
    coxa3.write(180-gamma3);
    femur3.write(alfa1_3+alfa2_3);
    tibia3.write(beta3); 
}
void CalculationOfAngles4(float x4,float y4,float z4)
{  
    if (y4==0) {y4=1;} 
    gamma4=atan(x4/y4)*180/3.14159;      
    y44=y4/cos(gamma4*3.14159/180);  
    if (y4<0) {y44=abs(y4)/(cos(abs(gamma4)*3.14159/180)); gamma4= 180+gamma4;}
    L4=sqrt(sq(z0-z4)+sq(y44-coxa));
    alfa1_4=asin((y44-coxa)/L4)*180/3.14159;
    alfa2_4=acos((sq(femur)+sq(L4)-sq(tibia))/(2*femur*L4))*180/3.14159;
    beta4=acos((sq(femur)+sq(tibia)-sq(L4))/(2*tibia*femur))*180/3.14159;
    gamma4=(int)(gamma4+0.5);
    alfa1_4=(int)(alfa1_4+0.5);
    alfa2_4=(int)(alfa2_4+0.5);
    beta4=(int)(beta4+0.5);
    coxa4.write(gamma4);
    femur4.write(180-(alfa1_4+alfa2_4));
    tibia4.write(180-(beta4)); 
}
void CalculationOfAngles5(float x5,float y5,float z5)
{  
    if (y5==0) {y5=1;}  
    gamma5=atan(x5/y5)*180/3.14159;      
    y55=y5/cos(gamma5*3.14159/180);  
    if (y5<0) {y55=abs(y5)/(cos(abs(gamma5)*3.14159/180)); gamma5= 180+gamma5;}
    L5=sqrt(sq(z0-z5)+sq(y55-coxa));
    alfa1_5=asin((y55-coxa)/L5)*180/3.14159;
    alfa2_5=acos((sq(femur)+sq(L5)-sq(tibia))/(2*femur*L5))*180/3.14159;
    beta5=acos((sq(femur)+sq(tibia)-sq(L5))/(2*tibia*femur))*180/3.14159;
    gamma5=(int)(gamma5+0.5);
    alfa1_5=(int)(alfa1_5+0.5);
    alfa2_5=(int)(alfa2_5+0.5);
    beta5=(int)(beta5+0.5);
    coxa5.write(gamma5);
    femur5.write(180-(alfa1_5+alfa2_5));
    tibia5.write(180-beta5); 
}
void CalculationOfAngles6(float x6,float y6,float z6)
{  
    if (y6==0) {y6=1;}   
    gamma6=atan(x6/y6)*180/3.14159;      
    y66=y6/cos(gamma6*3.14159/180);  
    if (y6<0) {y66=abs(y6)/(cos(abs(gamma6)*3.14159/180)); gamma6= 180+gamma6;}
    L6=sqrt(sq(z0-z6)+sq(y66-coxa));
    alfa1_6=asin((y66-coxa)/L6)*180/3.14159;
    alfa2_6=acos((sq(femur)+sq(L6)-sq(tibia))/(2*femur*L6))*180/3.14159;
    beta6=acos((sq(femur)+sq(tibia)-sq(L6))/(2*tibia*femur))*180/3.14159;
    gamma6=(int)(gamma6+0.5);
    alfa1_6=(int)(alfa1_6+0.5);
    alfa2_6=(int)(alfa2_6+0.5);
    beta6=(int)(beta6+0.5);
    coxa6.write(gamma6);
    femur6.write(180-(alfa1_6+alfa2_6));
    tibia6.write(180-(beta6)); 
}
----------------------------------------------------------------
ТУТ БЫЛО УПРАВЛЕНИЕ (НО ЕГО СДЕЛАЛИ ПИЗДЕЦ ГРУБО, С ПОМОЩЬЮ ПОТЕНЦИОМЕТРОВ) ТАК ЧТО ЭТОТ КУСОК КОДА ЧИСТО ДЛЯ ТОГО ЧТО БЫ ПОСМОТРЕТЬ
КАКИЕ ПАРАМЕТРЫ ШЛИ НА ПРИЕМ

// алгоритм опроса датчиков
/*void digisense()
{  
/*  float R1 = 100000.0; // сопротивление R1 (100K)
  float R2 = 10000.0; // сопротивление R2 (10K)
  // считываем значение общего напряжения:
  float vo = analogRead(0);  
  // считываем значение напряжения на 1-ой банке:
  float vb1 = analogRead(1);  
  // считываем значение напряжения на 2-ой банке:
  float vb2 = analogRead(2); 
  // считываем значение напряжения на 3-ей банке:
  float vb3 = analogRead(3); 
  // преобразуем снятое напряжение к более удобному виду:
  float voin = vo/(R2/(R1+R2));
  // обнуляем нежелательное значение:
  if (voin<0.09) {vin=0.0; } 
}
  */


// эта функция производит чтение поступающих параметров из шины I2C (используется для отладки)
/*void receiveEvent(int howMany)
{
// код, отвечающй за приём параметров балансировки
  while(Wire.available()) 
  {
  VRxAngle=Wire.read(); 
  VRyAngle=Wire.read(); 
  zbalance=Wire.read();
  delay(10);
  }

// код, отвечающй за приём параметров хотьбы вперёд/назад
/*   x = Wire.read();       
   s = Wire.read();   
   t1 = Wire.read();    
   int d1 = Wire.read();  
   int d2 = Wire.read(); 
   int d3 = Wire.read(); 
   int d4 = Wire.read(); 
   t2=d1*1000+d2*100+d3*10+d4; */  
   
// код, отвечающй за приём параметров хотьбы вперёд/назад   
/*   int b,a;
   // получение координаты x и проверка её знака
   x1 = Wire.read();  
   a = Wire.read(); 
   if (a==0){x1=-x1;}
   // получение координаты y и проверка её знака  
   y1 = Wire.read();  
   a = Wire.read(); 
   if (a==0){ y1=-y1;}
   // получение координаты z и проверка её знака  
   z1 = Wire.read();  
   a = Wire.read(); 
   if (a==0){z1=-z1;} 
  delay(10);
}*/

----------------------------------------------------------------------------------------------------------
ФУНКЦИЯ ДВИЖЕНИЯ ВПЕРЕД (САМЫЙ ВАЖНЫЙ МОМЕНТ, В УПРАВЛЕНИИ ЧТО ВЫШЕ ЕСТЬ ДВИЖЕНИЕ НАЗАД, НО ЗДЕСЬ БЕЗ УПРАВЛЕНИЯ, ТОЛЬКО ВПЕРЕД
НО ВСЕ АНАЛОГИЧНО, НУЖНО СДЕЛАТЬ ДВИЖЕНИЕ ВПЕРЕД/НАЗАД/ВЛЕВО/ВПРАВО

 // функци базовых движний

// движение вперёд
void forward() // x(мм)-отдаление конечностей от корпуса, s(мм, чётное)-ширина шага, t(мс)-задержка фазы движения 
{ 
  s=40; x=70; td1=10; td2=400; 
  y=s/2; y1=y; y2=-y; 
  
  int zmin=-30; // этим числом задаём минимальную высоту, на которой происходит движение вперёд
  int zmax=-130; // этим числом задаём максимальную высоту, на которой происходит движение вперёд
// Фаза 1,6: 1,3,5(опущены, y1=50), 2,4,6(опущены, y2=-50)    
  z1=zmax;
  CalculationOfAngles1(x,y1,z1); 
  CalculationOfAngles3(x,y1,z1); 
  CalculationOfAngles5(x,y1,z1); 
  delay(t2); //ждём, пока робот успеет поменять ноги

// Фаза 2: 1,3,5(опущены, y1=50), 2,4,6(подняты, y2=-50) 
  z2=zmin;
  CalculationOfAngles2(x,y2,z2); 
  CalculationOfAngles4(x,y2,z2); 
  CalculationOfAngles6(x,y2,z2);
  
// Фаза 3: 1,3,5(опущены) передвигаем по y на 100 назад, 2,4,6(подняты) на 100 вперёд 
  while (y1>-y)      //этим числом регулируем ширину шага  
  {
    CalculationOfAngles1(x,y1,z1);
    CalculationOfAngles3(x,y1,z1); 
    CalculationOfAngles5(x,y1,z1);  
    CalculationOfAngles2(x,y2,z2); 
    CalculationOfAngles4(x,y2,z2); 
    CalculationOfAngles6(x,y2,z2);
    delay(t1); 
    y1--;
    y2++;
  } 

// Фаза 4: 1,3,5(подняты, y1=-50), 2,4,6(опущены, y2=50) 
  z2=zmax;
  CalculationOfAngles2(x,y2,z2); 
  CalculationOfAngles4(x,y2,z2); 
  CalculationOfAngles6(x,y2,z2);
  delay(t2); //ждём, пока робот успеет поменять ноги
  z1=zmin;
  CalculationOfAngles1(x,y1,z1);
  CalculationOfAngles3(x,y1,z1); 
  CalculationOfAngles5(x,y1,z1); 
   
// Фаза 5: 1,3,5(подняты, y1=50) передвигаем по y на 100 вперёд, 2,4,6(опущены, y1=50) на 100 назад    
   while (y2>-y)
  {
    CalculationOfAngles1(x,y1,z1);
    CalculationOfAngles3(x,y1,z1); 
    CalculationOfAngles5(x,y1,z1); 
    CalculationOfAngles2(x,y2,z2); 
    CalculationOfAngles4(x,y2,z2); 
    CalculationOfAngles6(x,y2,z2);
    delay(t1); 
    y1++;
    y2--;
  }  


---------------------------------------------------------------------------------------
ТАЖЕ ПРОБЛЕМА VRxAngle=90; VRyAngle=60; zbalance=130; ТРИ ПЕРЕМЕННЫЕ ЭТИ ВИСЕЛИ НА I2C, И ТУПО ПРИВЯЗАНЫ К УПРАВЛЕНИЮ

// функция для управления туловищем
void GyroBalance()
{ 
  y=0; x=160; VRxAngle=90; VRyAngle=60; zbalance=130;
  int z_otkl=0.5*legdistanceY*cos(VRyAngle*3.14159/180);
  z2=zbalance+z_otkl;
  z5=zbalance-z_otkl; 
  z3=zbalance-legdistanceX*cos(VRxAngle*3.14159/180)+z_otkl;
  z1=2*legdistanceX*cos(VRxAngle*3.14159/180)+z3+z_otkl; 
  z4=zbalance-legdistanceX*cos(VRxAngle*3.14159/180)-z_otkl;
  z6=2*legdistanceX*cos(VRxAngle*3.14159/180)+z4-z_otkl; 
  
  
  CalculationOfAngles1(x,y,z1);
  CalculationOfAngles2(x,y,z2); 
  CalculationOfAngles3(x,y,z3); 
  CalculationOfAngles4(x,y,z4); 
  CalculationOfAngles5(x,y,z5); 
  CalculationOfAngles6(x,y,z6);
  Serial.println(VRxAngle);
  Serial.println(VRyAngle);
  Serial.println(zbalance);
  Serial.println(z1);
  Serial.println(z2);
  Serial.println(z3);
  Serial.println(z4);
  Serial.println(z5);
  Serial.println(z6);
  
}
---------------------------------------------------------------------------------------
ТУТ ВСЕ ПРОСТО, ПРОСТО КОНЕЧНОСТИ ВВЕРХ ПОДНИМАЕТ, ЧТО БЫ ВСЕ ОДИНАКОВО СТОЯЛИ. ЧТО БЫ НАЧАТЬ ЧТО-ТО ДЕЛАТЬ
ПРОСТО ВСЕ ВЫРОВНЯЛИ
// алгоритм для калибровки конечностей (выставляет все углы femur и tibia в 120 град.,
// а coxa в 90), к выставленным углам добавляются поправки

void LegCalibration() 
{
  /*
    CalculationOfAngles2(x1,y1,z1);
    CalculationOfAngles3(x1,y1,z1);
    CalculationOfAngles4(x1,y1,z1);
    CalculationOfAngles5(x1,y1,z1);
    CalculationOfAngles6(x1,y1,z1);  */
    int f=120; int g=90;
           
    tibia1.write(f);
    tibia2.write(f); 
    tibia3.write(f); 
    tibia4.write(180-(f));
    tibia5.write(180-(f)); 
    tibia6.write(180-(f));

    femur1.write(f);
    femur2.write(f);
    femur3.write(f);
    femur4.write(180-(f));
    femur5.write(180-(f));
    femur6.write(180-(f));

    coxa1.write(180-(g+1));
    coxa2.write(180-(g-4));
    coxa3.write(180-(g-1));
    coxa4.write(g+2);
    coxa5.write(g-7);
    coxa6.write(g+3); 
    
    delay(1000);  
}

-----------------------------------------------------------------
ПОДНЯТИЕ ТУЛОВИЩА ЛАПАМИ ВВЕРХ И ВНИЗ
ДОЛЖНО ВЫПОЛНЯТЬСЯ ПО КНОПКЕ, НАЖАЛ ПОДНЯЛСЯ, НАЖАЛ ОПУСТИЛСЯ
void updown()
{
  int x33=100; int y33=0; int z33=-100;
  CalculationOfAngles1(x33,y33,z33);
  CalculationOfAngles2(x33,y33,z33);
  CalculationOfAngles3(x33,y33,z33);
  CalculationOfAngles4(x33,y33,z33);
  CalculationOfAngles5(x33,y33,z33);
  CalculationOfAngles6(x33,y33,z33);
  delay(1000);
  z33=-130;
  CalculationOfAngles1(x33,y33,z33);
  CalculationOfAngles2(x33,y33,z33);
  CalculationOfAngles3(x33,y33,z33);
  CalculationOfAngles4(x33,y33,z33);
  CalculationOfAngles5(x33,y33,z33);
  CalculationOfAngles6(x33,y33,z33);
  delay(1000);
}
-------------------------------------------------------------
ТУТ Я ЧУТЬ ПОЗЖЕ ДОПИШУ. НУЖНО ЛИ ДОБАВЛЯТЬ ПАРАМЕТР, ЕЩЕ НЕ ПРОВЕРИЛ ЕГО ПОЛНОСТЬЮ
void difangle()
{
  int x55=60; int y55=0; int z55=-130;
  CalculationOfAngles1(x55,y55,z55);
  CalculationOfAngles2(x55,y55,z55);
  CalculationOfAngles3(x55,y55,z55);
  CalculationOfAngles4(x55,y55,z55);
  CalculationOfAngles5(x55,y55,z55);
  CalculationOfAngles6(x55,y55,z55);
  delay(500);
  CalculationOfAngles1(x55,y55,z55-20);
  CalculationOfAngles2(x55,y55,z55);
  CalculationOfAngles3(x55,y55,z55+20);
  CalculationOfAngles4(x55,y55,z55+20);
  CalculationOfAngles5(x55,y55,z55);
  CalculationOfAngles6(x55,y55,z55-20);
  delay(500);
}


ЗНАЧИТ ВСЕ УПРАВЛЕНИЕ РАЗДЕЛЯЕТСЯ НА ТАКИЕ ЧАСТИ, 
1. СНАЧАЛА МЫ ДОЛЖНЫ ОТКАЛИБРОВАТЬ КОНЕЧНОСТИ

			void LegCalibration() 

2. ДВИЖЕНИЕ ВПЕРЕД/НАЗАД/ВЛЕВО/ВПРАВО
			void forward()
			
3. ПОДНЯТИЕ КОРПУСА ВВЕРХ/ВНИЗ
			void updown()
			
4. УПРАВЛЕНИЕ ТУЛОВИЩЕМ (ПРИМЕР КАК МЫ ДВИГАЕМ БЕДРАМИ, ТАК И КОНЕЧНОСТИ БУДУТ ДВИГАТЬСЯ, НЕ ДВИГАЯ КОРПУС, ПОХОЖЕ НА СТАБИЛИЗАЦИЮ, КОГДА ПОСЕРЕДИНЕ ТЕЛО НЕПОДВИЖНО

			void GyroBalance()


НА ВСЕ НУЖНЫ ОТДЕЛЬНЫЕ КНОПКИ, ЖМЕШЬ ВПЕРЕД ИДЕТ, НЕ ЖМЕШЬ, НЕ ИДЕТ (НО ЭТО В ИДЕАЛЕ КОНЕЧНО) ВАЖНО ЧТО-БЫ ХОТЯ БЫ ШЕЛ ВПЕРЕД ПРИ ЗАЖАТИИ КНОПКИ































