#define TEMP_PIN A0
int ticPerSec = 0;
int ticTic = 0;
float curTemp = 0;
float maxTemp = 0;
unsigned long strtMil = 0;
unsigned long strtMil2 = 0;
unsigned long intervalMil = 1000;
int ARvalue = 0;

float tempT1F(float analogVal){
	float tempVal = ((analogVal / 204.6) - 1.25) / 0.005;
	return tempVal;
}

// Начало функции обработки кириллических символов при выводе на монитор серийного порта
String utf8rus(String source){     // Функция для конвертации русских символов из двубайтовой кодировки в однобайтовую

    int i,k;
    String target;
    unsigned char n;
    char m[2] = { '0', '\0' };
    k = source.length(); i = 0;
    while (i < k) {
    n = source[i]; i++;
 
    if (n >= 0xBF){
        switch (n) {
            case 0xD0: {
                n = source[i]; i++;
                if (n == 0x81) { n = 0xA8; break; }
                if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
                break;
            }
            case 0xD1: {
                n = source[i]; i++;
                if (n == 0x91) { 
                    n = 0xB8; break; 
                }
                    if (n >= 0x80 && n <= 0x8F) {
                        n = n + 0x70;
                    }
                break;
            }
        }
    }
    m[0] = n; target = target + String(m); 
    }
    return target;
}
// Конец функции обработки кириллических симоволов


void setup(){
	Serial.begin(115200);

	delay(500);
	Serial.println(utf8rus(" 10 измерений 1 раз в секунду "));

	maxTemp = 0;
	ticPerSec = 0;
	strtMil = millis();
	strtMil2 = strtMil;
	while(1){
		if(millis() - strtMil2 >= 1000){

		    ARvalue = analogRead(TEMP_PIN);
		    curTemp = tempT1F(ARvalue);
		    Serial.println();
			Serial.print("   1    CUR TEMP = ");
			Serial.print(curTemp);
			Serial.print(",   MILLIS = ");
			Serial.println(millis());
		    if(curTemp > maxTemp){
		    	maxTemp = curTemp;
		    }
		    ticPerSec ++;
		    strtMil2 = millis();
		}
	    if(millis() - strtMil >= 11000){
	    	break;
	    }
	}
	Serial.println();
	Serial.print("1    MAX TEMP = ");
	Serial.print(maxTemp);
	Serial.print(",   TIC PER SEC = ");
	Serial.println(ticPerSec);






	Serial.println(utf8rus(" 500 измерений 50 раз в секунду, вывод 10 значений МАКС "));

}
void loop(){
	maxTemp = 0;
	ticPerSec = 0;
	strtMil = millis();
	strtMil2 = strtMil;
	while(1){
		if(millis() - strtMil2 >= 19){

		    ARvalue = analogRead(TEMP_PIN);
		    curTemp = tempT1F(ARvalue);
		 //    Serial.println();
			// Serial.print("   50    CUR TEMP = ");
			// Serial.print(curTemp);
			// Serial.print(",   MILLIS = ");
			// Serial.println(millis());
		    if(curTemp > maxTemp){
		    	maxTemp = curTemp;
		    }
		    ticPerSec ++;
		    strtMil2 = millis();
		}
	    if(millis() - strtMil >= intervalMil){
	    	break;
	    }
	}
	Serial.println();
	Serial.print("50    MAX TEMP = ");
	Serial.print(maxTemp);
	Serial.print(",   TIC PER SEC = ");
	Serial.println(ticPerSec);
	ticTic ++;
	if(ticTic >= 10){
	    Serial.println(utf8rus("ОКОНЧАНИЕ ТЕСТА"));
		while(1){}
	}
	

	// maxTemp = 0;
	// ticPerSec = 0;
	// strtMil = millis();
	// strtMil2 = strtMil;
	// while(1){
	// 	if(millis() - strtMil2 >= 19){

	// 	    ARvalue = analogRead(TEMP_PIN);
	// 	    curTemp = tempT1F(ARvalue);
	// 	    Serial.println();
	// 		Serial.print("   CUR TEMP = ");
	// 		Serial.print(curTemp);
	// 		Serial.print(",   MILLIS = ");
	// 		Serial.println(millis());
	// 	    if(curTemp > maxTemp){
	// 	    	maxTemp = curTemp;
	// 	    }
	// 	    ticPerSec ++;
	// 	    strtMil2 = millis();
	// 	}
	//     if(millis() - strtMil >= intervalMil){
	//     	break;
	//     }
	// }
	// Serial.println();
	// Serial.print("MAX TEMP = ");
	// Serial.print(maxTemp);
	// Serial.print(",   TIC PER SEC = ");
	// Serial.println(ticPerSec);

}