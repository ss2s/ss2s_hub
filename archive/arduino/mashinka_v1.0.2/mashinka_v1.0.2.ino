          /* ПРОШИВКА ДЛЯ МАШИНКИ v1.0.2 Arduino ProMini Optimized some functions */

#define DEF_PIN_CONFIG        DDRB |= B1111;     // pinMode(11,10,9,8,OUTPUT)
#define DEF_START_DIR_SET     PORTB |= B1001;     // digitalWrite(11,8,HIGH)
#define DEF_START_SPD_SET     PORTB &= B11111001;     // digitalWrite(10,9,LOW)

#define DEF_DIR_LEFT

#define DEF_DIR_RIGHT


void setup() {

}

void loop() {

}
