byte s=0;
void setup() {
pinMode(s, OUTPUT);

}

void loop() {
for (byte pos=0;pos<180;pos++)
{
pulseOut(s,pos);
delay(20);
}
}

void pulseOut( byte pin, byte p){
digitalWrite(pin,HIGH);
delayMicroseconds(300+p*(2500/180));
digitalWrite(pin,LOW);
//Serial.println(p*(1000/180));
}
