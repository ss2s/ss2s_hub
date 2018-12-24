
void setup() {
	Serial.begin(250000);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
}

unsigned long delTimeH = 50;
unsigned long delTimeL = 1;

void loop() {
	if (Serial.available() > 0) {
		char sr = Serial.read();
		if(sr == 'h'){
			delTimeH = Serial.parseInt();
		}else if(sr == 'l'){
			delTimeL = Serial.parseInt();
		}
	}
	digitalWrite(8, HIGH);
	delay(delTimeH);
	digitalWrite(8, LOW);
	delay(delTimeL);
	digitalWrite(9, HIGH);
	delay(delTimeH);
	digitalWrite(9, LOW);
	delay(delTimeL);
	digitalWrite(10, HIGH);
	delay(delTimeH);
	digitalWrite(10, LOW);
	delay(delTimeL);
	digitalWrite(11, HIGH);
	delay(delTimeH);
	digitalWrite(11, LOW);
	delay(delTimeL);
	digitalWrite(12, HIGH);
	delay(delTimeH);
	digitalWrite(12, LOW);
	delay(delTimeL);
	digitalWrite(13, HIGH);
	delay(delTimeH);
	digitalWrite(13, LOW);
	delay(delTimeL);
	// digitalWrite(13, HIGH);
	// delay(delTimeH);
	// digitalWrite(13, LOW);
	// delay(delTimeL);
	digitalWrite(12, HIGH);
	delay(delTimeH);
	digitalWrite(12, LOW);
	delay(delTimeL);
	digitalWrite(11, HIGH);
	delay(delTimeH);
	digitalWrite(11, LOW);
	delay(delTimeL);
	digitalWrite(10, HIGH);
	delay(delTimeH);
	digitalWrite(10, LOW);
	delay(delTimeL);
	digitalWrite(9, HIGH);
	delay(delTimeH);
	digitalWrite(9, LOW);
	delay(delTimeL);
	// digitalWrite(8, HIGH);
	// delay(delTimeH);
	// digitalWrite(8, LOW);
	// delay(delTimeL);
}