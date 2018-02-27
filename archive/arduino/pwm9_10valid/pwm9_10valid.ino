
int pwmL = 10;
int pwmR = 9;


void setup() {

pinMode(pwmL,OUTPUT);
pinMode(pwmR,OUTPUT);

analogWrite(pwmL,100);
analogWrite(pwmR,200);
}

void loop() {
  
}
