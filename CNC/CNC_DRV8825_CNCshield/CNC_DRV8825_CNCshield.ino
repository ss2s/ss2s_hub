int motorPins[3][2] = {{3,6},{2,5},{12, 13}};   //y{step,dir} x{step,dir} z{step,dir}     8.9 10.11 12.13 умолчания
int count;                         //подключение первого мотора Y               
int count2[3] = {0,0,0};           //пин STEP на пин ардуины D8             
int delayTime = 150;               //пин DIR на пин ардуины D9
int val = 0;
int rot=0;                          //подключение второго мотора X 
int incomingByte = 0;               //пин STEP на пин ардуины D10
int sign=1;                         //пин DIR на пин ардуины D11
int s_p=40;                                 
int laserpin = 12;
void setup() {                      //Лазер подключается на пин D2
int i;
Serial.begin(57600);  //Эта скорость должна совпадать со скоростью в программе
pinMode(laserpin, OUTPUT);
for (i=0; i<3; i++) {
 for (count = 0; count < 2; count++) {
  pinMode(motorPins[i][count], OUTPUT);}} //установка режима работы цифровых pin'ов Ардуино
 delayTime=150;}                         //задержка между шагами по умолчанию
void moveForward(int sm) {
digitalWrite(motorPins[sm][1], HIGH);
digitalWrite(motorPins[sm][0], HIGH);
digitalWrite(motorPins[sm][0], LOW);}
void moveBackward(int sm) {
digitalWrite(motorPins[sm][1], LOW);
digitalWrite(motorPins[sm][0], HIGH);
digitalWrite(motorPins[sm][0], LOW);}
void delayMicros(long wt){
 unsigned long mls;
 unsigned int mks;
 mls=(unsigned long)(wt / 1000);
 mks=(unsigned int)(wt % 1000);
 if (mls>0) delay(mls);
 if (mks>0) delayMicroseconds(mks);}
void MoveSM(long x, long y, long z) {
 long c[3], c2[3];
 double c1[3], d[3];
 long m, i;
 boolean flg;
 long ms;
 long d1, d2, d3, d4, t, dt;
 c[0] = x;
 c[1] = y;
 c[2] = z;
 m = 1;
 for (i=0; i<2; i++) {
  if (m < abs(c[i])) m = abs(c[i]);}
 for (i=0; i<2; i++) {
  c1[i] = 0;
  d[i] = 1.0 * c[i] / m;
  c2[i] = 0;}
flg = false;
 for (i=0; i<2; i++) {
  if (abs(c1[i]) < abs(c[i])) flg=true;}
 t=m;
 if (z>0 && t>0) {
  if (z>=delayTime) {
   dt=z; d1=s_p; d2=1;
   d3=0;
   if (d1==d2) d4=z/2; else d4=z;       
  } else {
   dt=delayTime;
   d3=dt*s_p*(dt-z)/dt/2;
   d2=d3/dt;
   d1=s_p-d2;
   d3=d3-d2*dt;
   d2=d2+1;
   if (d1==d2) d4=dt/2-d3; else d4=dt-d3;}
 } else {
  dt=delayTime;
  d1=-1; d2=-1; d3=0; d4=0;}
 while (flg) {
  flg=false;
  for (i=0; i<2; i++) {
   if (abs(c1[i]) < abs(c[i]))
    c1[i] += d[i];
   if (abs(c1[i]) - abs(c2[i]) >= 0.5) {
    if (c[i]>0) {
    c2[i]++;
    moveForward(i);
    } else if (c[i]<0) {
    c2[i]--;
    moveBackward(i);}}
   if (abs(c1[i]) < abs(c[i])) flg=true;}
  if (z>0) {
   if (t==d1) {
    delayMicros(d3);
    digitalWrite(laserpin, HIGH);
    delayMicros(d4);}
   if (t==d2) {
    delayMicros(d4);
    digitalWrite(laserpin, LOW);
    delayMicros(d3);}
   if (t!=d1 && t!=d2) {
    delayMicros(dt);}
  } else {
   delayMicros(dt);}
  t--;}}
void loop() {
if (Serial.available() > 0) {
 long c[5]={0,0,0,0,0};
 int i;
 sign=1;
 i=0;
 incomingByte = Serial.read();
 while (incomingByte!=';') {
  if (c[i]==0) {
  if (incomingByte=='-')
   sign=-1;}
  if (incomingByte==',') {
  c[i]*=sign;
  sign=1;
  i++;
  } else if (incomingByte>='0' && incomingByte<='9') {
  c[i]=c[i]*10+incomingByte-'0';}
  while (Serial.available() == 0) {
  delayMicroseconds(1);}
  incomingByte = Serial.read();}
 c[i]*=sign;
 if (c[3]>0) s_p=c[3]; 
 if (c[4]>0) delayTime=c[4]; 
 MoveSM(c[0],c[1],c[2]);
 Serial.println("OK");}
else
 delayMicroseconds(1); } 
