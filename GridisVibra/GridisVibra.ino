//#include <MemoryFree.h>
#include <SoftwareSerial.h>

#define swPin 2
uint32_t SWCount=0UL;
bool SWState;
long SWTime; 
#define LedPin A5
String currStr = "";
String SMSto;
byte state=0; //state of module 0 - idle, 1 - dialing, 3 - alerting, 4 - active call , 5 - error
const char* initString[]= {"ATZ", "ATE0", "AT+CLIP=1", "AT+CMGF=1", "AT+CNMI=3,2", "AT+CLCC=1", "AT+CPMS=\"ME\",\"ME\",\"ME\""};
volatile long dialtime;
long battest;
int batVoltage;
int batPercent;

void pulseCounter ()
{
  SWCount++;
}
//===============================================================================

void sendBatVoltage()
{
    Serial.println("AT+CMGS=\"+"+SMSto+"\"");
    delay(1000);
    Serial.print("Battery voltage is: "+String(batVoltage)+" mV. Charged at: "+String(batPercent)+"%");
    Serial.write((char)26);
}


SoftwareSerial debug(11, 12);


void setup() {

  debug.begin(9600);
  pinMode (LedPin, OUTPUT);
  digitalWrite(LedPin, HIGH);
  pinMode (swPin, INPUT);
  digitalWrite(swPin, HIGH);
  Serial.begin(9600);
  Serial.setTimeout(100);
  delay(15000);
  while (Serial.available() > 0 ) Serial.read();

  do {
    Serial.println("AT");
    currStr = Serial.readStringUntil('\n');
    debug.println("< "+currStr);
  } while (!currStr.startsWith("OK"));
  
  while (Serial.available() > 0 ) Serial.read();
  
  for (byte i=0; i<7; i++ ) {
  Serial.println(initString[i]);
  debug.println("> "+String(initString[i]));
  do {
    currStr = Serial.readStringUntil('\n');
    debug.println("< "+currStr);
  } while (!currStr.startsWith("OK"));
  };
 attachInterrupt(0, pulseCounter, CHANGE);
 SWState=false;
 digitalWrite(LedPin, LOW);
 }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void loop() {

  
   
 if (SWTime < millis() && SWCount < 200 )  {SWTime=millis()+1000; SWCount=0;} 
 
 if (SWCount > 200 && state==0 && dialtime < millis() ) {Serial.println(F("ATD>\"GRIDIS\";"));  debug.println(F("> ATD>\"GRIDIS\";")); state=1; SWCount=0;} 
  
 if (battest < millis() && state==0 ) {battest=millis()+10000; Serial.println(F("AT+CBC\nAT+CSQ"));  debug.println(F("> AT+CBC"));} 
//===================================================

if (Serial.available() > 0) 
    { 
     currStr = Serial.readStringUntil('\n');  

     debug.println("< "+currStr);                      //печатаем в монитор строку
 
      if (currStr.startsWith("+CBC:")) 
                                {currStr=currStr.substring(8);
                                 batPercent=currStr.toInt();
                                 currStr=currStr.substring((currStr.length()-5));
                                 batVoltage=currStr.toInt();
                                 if (batVoltage < 3600) digitalWrite(LedPin, HIGH); else digitalWrite(LedPin, LOW);
                                 };
       if (currStr.startsWith("+CLCC:") && currStr.substring(11, 12)=="4" && currStr.indexOf("GRIDIS") > 0)
            {
            SMSto=currStr.substring(19, 31);
            while (Serial.available() > 0 ) Serial.read(); // очистим буфер порта
            Serial.println(F("ATH"));
            debug.println(F("> ATH"));                     // печатаем в монитор 
            while (Serial.available() > 0 ) Serial.read(); // очистим буфер порта
            sendBatVoltage();
            delay(500);
            digitalWrite(LedPin, HIGH);
            }
       if (currStr.startsWith("+CLCC:") && currStr.substring(11, 12)=="0") {Serial.println(F("ATH")); debug.println(F("> ATH")); state=0; dialtime=millis ()+15000; SWCount=0;}
       if (currStr.startsWith("+CLCC:") && currStr.substring(11, 12)=="3") {state=3;}
       if (currStr.startsWith("+CLCC:") && currStr.substring(11, 12)=="6" && state==1) {dialtime=millis () + 2000; state=0; SWCount=0;}
       if (currStr.startsWith("+CLCC:") && currStr.substring(11, 12)=="6" && state==3) {dialtime=millis () + 15000; state=0; SWCount=0;}
  
      // if (SWCount < 200 && state!=0 ) state=0;
  
   }
 }






