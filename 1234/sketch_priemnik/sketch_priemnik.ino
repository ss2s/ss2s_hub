#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

RF24 radio(9,10);
const uint64_t pipe = 0xF0F0F0F000LL; 
int msg[1]; 
int data; 
int pos;

void setup()
{
  
servo1.attach(3); 
servo2.attach(5); 
servo3.attach(2);
servo4.attach(4);
servo5.attach(6);
delay(50);

servo1.write(0);   // motor1 стартовая позиция при включении (0)
servo2.write(90);  // серво2 стартовая позиция при включении (90)
servo3.write(90);  // серво3 стартовая позиция при включении (90)
servo4.write(90);  // серво4 стартовая позиция при включении (90)
servo5.write(90);  // серво5 стартовая позиция при включении (90)
delay(10);

radio.begin();
radio.openReadingPipe(1,pipe); 
radio.startListening();
}

void loop()
{
  if (radio.available())radio.read(msg, 2);
  if (msg[0] <=180 && msg[0] >=0)data = msg[0], pos = map(data, 0, 180, 0, 180),servo1.write(pos);//motor
  if (msg[0] <=380 && msg[0] >=200)data = msg[0], pos = map(data, 200, 380, 5, 175),servo2.write(pos);
  if (msg[0] <=580 && msg[0] >=400)data = msg[0], pos = map(data, 400, 580, 5, 175),servo3.write(pos);
  if (msg[0] <=780 && msg[0] >=600)data = msg[0], pos = map(data, 600, 780, 5, 175),servo4.write(pos);
  if (msg[0] <=980 && msg[0] >=800)data = msg[0], pos = map(data, 800, 980, 5, 175),servo5.write(pos);
  
  
}


