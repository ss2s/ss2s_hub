#define KNOPKA 7
#define LED 13

unsigned long pulsVal = 5000; // ДЛИТЕЛЬНОСТЬ ИМПУЛЬСА В мкс. ДЛИТЕЛЬНОСТЬ НАЖАТИЯ ДЛЯ СРАБОТКИ КНОПКИ
unsigned long duration;
unsigned long durationTimeout = 50000000; // ДЛИТЕЛЬНОСТЬ ТАЙМАУТА (ДОЛЖНА БЫТЬ В 50 И БОЛЕЕ РАЗ БОЛЬШЕ ЧЕМ ДЛИТЕЛЬНОСТЬ ИМПУЛЬСА)
boolean lastLed;

void setup() {

  Serial.begin(9600);

	pinMode(KNOPKA, INPUT_PULLUP);
	//pinMode(KNOPKA, INPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
}

void loop() {

	duration = pulseIn(KNOPKA, LOW, durationTimeout);
  lastLed = digitalRead(LED);
	if (pulsVal < duration){
		digitalWrite(LED, !digitalRead(LED));
	}
	
    if(duration > 0){
        Serial.print("DURATION  ");
        Serial.print(duration/1000000);
        Serial.print(" s   ");
        Serial.print(duration/1000);
        Serial.print(" ms   ");
        Serial.print(duration);
        Serial.print(" mks   ");
        Serial.print(digitalRead(LED));
        if(digitalRead(LED) != lastLed){
        	Serial.println("+");
    	}
   		else{
   			Serial.println();
        }
    }
}
