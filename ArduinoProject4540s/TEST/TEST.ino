
void setup(){
	Serial.begin(250000);
}

void loop(){
	if (Serial.available() > 0) {
	    Serial.println(Serial.parseInt());
	}
}