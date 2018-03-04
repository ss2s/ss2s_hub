#include <avr/io.h>
#include <util/delay.h>

#define d_SERVO_PIN_d 0

//byte s=0;
byte pos = 90;
byte lastPos;

void pulseOut( byte _pin, byte _pos){

  PORTB |= (1<<0);
  delayMicroseconds(300+_pos*(2500/180));
  PORTB &= ~(1<<0);
}

void setup(){
  DDRB |= (1<<0);
  // DDRB &= ~(1<<3);
  // DDRB &= ~(1<<4);
  pulseOut(d_SERVO_PIN_d,pos);
  pulseOut(d_SERVO_PIN_d,pos);
  pulseOut(d_SERVO_PIN_d,pos);
  pulseOut(d_SERVO_PIN_d,pos);
  pulseOut(d_SERVO_PIN_d,pos);
}

void loop(){pulseOut(d_SERVO_PIN_d,pos);}



// void loop() {
//   if (lastPos == false) {
//     if (PINB & (1<<PINB3)){
//       _delay_ms(5);
//       if (PINB & (1<<PINB3))
//       {
//         pulseOut(d_SERVO_PIN_d, pos - 30);
//         lastPos = true;
//         _delay_ms(200);
//       }
//     }
//     if (PINB & (1<<PINB4)){
//       _delay_ms(5);
//       if (PINB & (1<<PINB4))
//       {
//         pulseOut(d_SERVO_PIN_d,pos + 30);
//         lastPos = true;
//         _delay_ms(200);
//       }
//     }
//   }
//   if (lastPos == true) {
//     if (!(PINB & (1<<PINB4)) && !(PINB & (1<<PINB4))){
//       for (byte i=0;i<10;i++){
//       pulseOut(d_SERVO_PIN_d,pos);
//       _delay_ms(2);
//       }
//       lastPos = false;
//       _delay_ms(200);
//     }
//   }
//   _delay_ms(20);
// }
