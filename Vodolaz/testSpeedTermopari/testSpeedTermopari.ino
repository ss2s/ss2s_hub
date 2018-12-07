#define TEMP_PIN A0
int ticPerSec = 0;
float curTemp = 0;
float maxTemp = 0;
unsigned long strtMil = 0;
unsigned long strtMil2 = 0;
unsigned long intervalMil = 1000;
int ARvalue = 0;

float tempT1F(int analogVal){
	float voltVal = analogVal / 204.6;
	float tempVal = (voltVal - 1.25) / 0.005;
	return tempVal;
}

void setup(){
	Serial.begin(115200);
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
		    Serial.println();
			Serial.print("   CUR TEMP = ");
			Serial.print(curTemp);
			Serial.print(",   MILLIS = ");
			Serial.println(millis());
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
	Serial.print("MAX TEMP = ");
	Serial.print(maxTemp);
	Serial.print(",   TIC PER SEC = ");
	Serial.println(ticPerSec);
	
}