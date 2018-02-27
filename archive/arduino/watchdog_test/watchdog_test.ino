#include <avr/wdt.h>

/*uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));
void get_mcusr(void){
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}*/

void setup() {
  wdt_disable();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(8000);
  digitalWrite(LED_BUILTIN, LOW);
  wdt_enable (WDTO_8S); // 8 сек.


}

void loop() {
  
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  
  //wdt_reset();

}
