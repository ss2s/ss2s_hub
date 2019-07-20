
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SERVO_CONTROLLER A328  // выбор контроллера: A328 = Atmega328, A13 = Attiny13
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SERVO_CONTROLLER == A328  // Atmega328
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>
Servo myservo;

void setup(){
	myservo.attach(10);
	myservo.write(180);
	delay(600);
	myservo.write(0);
}
void loop(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SERVO_CONTROLLER == A13  // Attiny13
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>  // принято подключать
#include <avr/interrupt.h>
#include <util/delay.h>  // нужна для функции _delay_ms();

#define TTIMER 182  // верхняя граница счетчика 182 = 50 Гц
#define OPEN 158    // открыть импульс 2 мс  серва 180
#define CLOSE 176   // закрыть импульс 0.5 мс серва 0

int main(void){
	CLKPR=0x80;       // регистр значения делителя, разрешить делитель
	CLKPR=1<<CLKPS2;  // установить делитель на 16   
	DDRB=(1<<PB1);    // настройка порта

	//настройка ШИМ 50 Гц 
	TCCR0A=(1<<WGM01)|(1<<WGM00)|(1<<COM0B0)|(1<<COM0B1);
	TCCR0B=(1<<WGM02)|(1<<CS01)|(1<<CS00);  // предделитель счетчика на 64

	OCR0A=TTIMER;      // счетчик будет считать до

	OCR0B=OPEN;        // установить серву в 180  открыть
	_delay_ms(60);     // задержка 600 мс
	OCR0B=CLOSE;       // установить серву в 0  закрыть

	while(1){}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////