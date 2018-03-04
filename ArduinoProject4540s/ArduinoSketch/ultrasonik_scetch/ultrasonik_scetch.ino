const int trigPin = 12;
const int echoPin = 11;
long duration, mm, cm, ss; 


void setup() {
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.begin(9600);
}

void loop() { 
digitalWrite(trigPin, LOW); 
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10); 
digitalWrite(trigPin, LOW);

duration = pulseIn(echoPin, HIGH); 
mm = duration * 1.7 * 0.1 +10;     //mm
cm = duration * 1.7 * 0.01 +1;     //cm

ss = mm - (cm * 10);

Serial.print(cm);
Serial.print(" cm ");
Serial.print(ss);
Serial.println(" mm");

/*
Serial.print(cm);
Serial.println(" cm");
*/
/*
Serial.print(mm);
Serial.println(" mm");
*/
delay (1000);
} 
