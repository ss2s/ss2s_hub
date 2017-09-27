// АВТОЗАПУСК ГЕНЕРАТОРА

// ***НАСТРОЙКА*** //

// РАСПИНОВКА
#define IN_CETb_PIN 2              // ВХОД 5в СЕТЬ
#define IN_GENA_PIN 3              // ВЧОД 5в ГЕНЕРАТОР
#define OUT_IGNITION_PIN 4         // РЕЛЕ ЗАЖИГАНИЯ
#define OUT_PODSOS_ON_PIN 5        // РЕЛЕ ВКЛЮЧЕНИЯ ПОДСОСА
#define OUT_STARTER_PIN 6          // РЕЛЕ СТАРТЕРА
#define OUT_PODSOS_OFF_PIN 7       // РЕЛЕ ВЫКЛЮЧЕНИЯ ПОДСОСА
#define OUT_KONTAKTOR_PIN 8        // РЕЛЕ КОНТАКТОРА (СИЛОВОЕ)
#define IN_STOP_PIN 10             // ВХОД 5в СТОП
#define LED_PIN 13                 // ДИОД НА ПЛАТЕ АРДУИНО "L"

// ВРЕМЕННЫЕ ЗАДЕРЖКИ В мс
#define PODSOS_TIMEOUT 800         // ВРЕМЯ РАБОТЫ ПРИВОДА ПОДСОСА 0.8 с
#define STARTER_TIMEOUT 2000       // ВРЕМЯ РАБОТЫ СТАРТЕРА 2 с
#define PRE_KONT_TIMEOUT 15000     // ЗАДЕРЖКА ПЕРЕД ВКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ЗАКРЫТИЯ ПОДСОСА 15 с
#define KONT_OFF_TIMEOUT 10000     // ЗАДЕРЖКА ПЕРЕД ВЫКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ВОССТАНОВЛЕНИЯ СЕТИ 10 с
#define OFF_IGNITION_TIMEOUT 30000 // ЗАДЕРЖКА ВЫКЛЮЧЕНИЯ ЗАЖИГАНИЯ 30 с
#define OPERATION_TIMEOUT 500      // ЗАДЕРЖКА МЕЖДУ ОПЕРАЦИЯМИ 0.5 с
#define RELAX_TIMEOUT 5000         // ЗАДЕРЖКА МЕЖДУ ПУСКАМИ 5 с
#define LOOP_TIMEOUT 1000          // ЗАДЕРЖКА МЕЖДУ ОПРОСАМИ В ЦИКЛЕ 1 с

// ДРУГИЕ НАСТРОЙКИ
#define ZAPUSK_COUNTER_VAL 5       // КОЛИЧЕСТВО ПОПЫТОК ЗАПУСКА ГЕНЕРАТОРА

// ***КОНЕЦ НАСТРОЙКИ*** //

// настройка реле (не трогать)
// если реле управляется 1
#define HHH 1
#define LLL 0
// если реле управляется 0
//#define HHH 0
//#define LLL 1

// переменные значений входов
boolean chk_CETb;
boolean chk_GENA;
boolean chk_STOP;

// переменные
int starterCount = 0; // щетчик запусков

void blink(int bCount = 1){

	int blinkCount = bCount;

	for(int i=0; i<blinkCount; i++){
	    digitalWrite(LED_PIN, HIGH);
	    	delay(500);
	    digitalWrite(LED_PIN, LOW);
	    	delay(500);
	}

}

void errorBlink(){

	digitalWrite(OUT_IGNITION_PIN, LLL);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);

	while(1){
	Serial.println("ERROR");
		blink(5);
		delay(5000);
	}
}

void genStart(){

	Serial.println("START GENERATORA");
	Serial.println("1");

	blink(3);
	digitalWrite(OUT_IGNITION_PIN, HHH);
		delay(OPERATION_TIMEOUT);
	digitalWrite(OUT_PODSOS_ON_PIN, HHH);
		delay(PODSOS_TIMEOUT);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
		delay(OPERATION_TIMEOUT);
	while(ZAPUSK_COUNTER_VAL > starterCount){

	    blink(2);
	    digitalWrite(OUT_STARTER_PIN, HHH);
			delay(STARTER_TIMEOUT);
		digitalWrite(OUT_STARTER_PIN, LLL);
			delay(OPERATION_TIMEOUT);
			delay(OPERATION_TIMEOUT);
			delay(OPERATION_TIMEOUT);
			delay(OPERATION_TIMEOUT);
		chk_GENA = digitalRead(IN_GENA_PIN);
		if(chk_GENA == HIGH){
		
	    	digitalWrite(OUT_PODSOS_OFF_PIN, HHH);
	        	delay(PODSOS_TIMEOUT);
	    	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	    	starterCount = ZAPUSK_COUNTER_VAL + 1;
		}else{

			blink();
			starterCount ++;
			delay(RELAX_TIMEOUT);
			Serial.println(starterCount);
		}
	}

	chk_GENA = digitalRead(IN_GENA_PIN);
	if(chk_GENA == LOW){
		delay(5);
		chk_GENA = digitalRead(IN_GENA_PIN);
	    if(chk_GENA == LOW){
	    	errorBlink();
		}
	}


}

void genStop(){

	blink(4);
		delay(OFF_IGNITION_TIMEOUT);
	blink(2);
	Serial.println("STOP GENERATORA");
	digitalWrite(OUT_IGNITION_PIN, LLL);
}

void setup() {

	pinMode(IN_CETb_PIN, INPUT);
	pinMode(IN_GENA_PIN, INPUT);
	pinMode(OUT_IGNITION_PIN, OUTPUT);
	pinMode(OUT_PODSOS_ON_PIN, OUTPUT);
	pinMode(OUT_STARTER_PIN, OUTPUT);
	pinMode(OUT_PODSOS_OFF_PIN, OUTPUT);
	pinMode(OUT_KONTAKTOR_PIN, OUTPUT);
	pinMode(IN_STOP_PIN, INPUT_PULLUP);
	pinMode(LED_PIN, OUTPUT);

	digitalWrite(OUT_IGNITION_PIN, LLL);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);
	digitalWrite(LED_PIN, LOW);

	Serial.begin(9600);
}

void loop() {

	chk_CETb = digitalRead(IN_CETb_PIN);
	chk_GENA = digitalRead(IN_GENA_PIN);
	chk_STOP = digitalRead(IN_STOP_PIN);

	if(chk_CETb == LOW && chk_GENA == LOW && chk_STOP == HIGH){

			delay(5);
		chk_CETb = digitalRead(IN_CETb_PIN);
	    chk_GENA = digitalRead(IN_GENA_PIN);
	    chk_STOP = digitalRead(IN_STOP_PIN);

	    if(chk_CETb == LOW && chk_GENA == LOW && chk_STOP == HIGH){

	    	genStart();
	    	chk_CETb = digitalRead(IN_CETb_PIN);
			chk_GENA = digitalRead(IN_GENA_PIN);
			chk_STOP = digitalRead(IN_STOP_PIN);
			if(chk_CETb == LOW && chk_GENA == HIGH & chk_STOP == HIGH){ 
					delay(5);
				chk_CETb = digitalRead(IN_CETb_PIN);
				chk_GENA = digitalRead(IN_GENA_PIN);
				chk_STOP = digitalRead(IN_STOP_PIN);
				if(chk_CETb == LOW && chk_GENA == HIGH & chk_STOP == HIGH){ 
						delay(PRE_KONT_TIMEOUT);
					digitalWrite(OUT_KONTAKTOR_PIN, HHH);
						Serial.println("CONTAKTOR ON");
   
				}   
			}
		}
	}else if(chk_CETb == HIGH && chk_GENA == HIGH && chk_STOP == HIGH){

			delay(5);
		chk_CETb = digitalRead(IN_CETb_PIN);
	    chk_GENA = digitalRead(IN_GENA_PIN);
	    chk_STOP = digitalRead(IN_STOP_PIN);

		if(chk_CETb == HIGH && chk_GENA == HIGH && chk_STOP == HIGH){

		    delay(KONT_OFF_TIMEOUT);
		    digitalWrite(OUT_KONTAKTOR_PIN, LLL);
		    	Serial.println("KONATAKTOR OFF");

		    genStop();
		}
	}else if(chk_STOP == LOW){
		
		chk_STOP = digitalRead(IN_STOP_PIN);

		if(chk_STOP == LOW){

				delay(5);
	    	chk_STOP = digitalRead(IN_STOP_PIN);

	    	if(chk_STOP == LOW){
	    		errorBlink();
	    	}
		}
	}

	delay(LOOP_TIMEOUT);
}

