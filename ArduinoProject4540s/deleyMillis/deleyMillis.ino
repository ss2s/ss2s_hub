unsigned long startMill = 1UL;
unsigned long currentMill = 1UL;
unsigned long delPeriod = 5000UL;
bool ledState = 0;
bool ciklState = 0;

void setup(){
  pinMode(13, OUTPUT);
}

void loop(){
  ciklState = 1;
  startMill = millis();
  while(ciklState){
    if ((currentMill = millis()) - startMill >= delPeriod){
      ledState = !ledState;
      digitalWrite(13, ledState);
      ciklState = 0;
      //break;
    }
  }
}
