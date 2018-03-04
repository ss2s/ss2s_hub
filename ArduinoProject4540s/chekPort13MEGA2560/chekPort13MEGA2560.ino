void setup() 
{
  Serial.begin(9600);
  //pinMode(13, OUTPUT);
  Serial.println(DDRB, BIN);
  Serial.println(PORTB, BIN);
}
void loop(){}
