void setup(){
	Serial.begin(250000);
}
void loop(){
	if (Serial.available() > 0) {
	    int a = Serial.parseInt();
	    int b = Serial.parseInt();
	    int c = Serial.parseInt();
	    int d = Serial.parseInt();
	    Serial.println();
	    Serial.println(a+b+c+d);
	    Serial.println();
	}
}