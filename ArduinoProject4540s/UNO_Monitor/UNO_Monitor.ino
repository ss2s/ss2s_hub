// Uno Monitor Via Bluetooth
// By keuwlsoft:  www.keuwl.com  26th Sept 2015
// cc Attribution-ShareAlike

// This sketch monitors the Analogue and Digital Inputs and sends
// the results via Bluetooth to keuwlsofts 'Bluetooth Electronics' app.
// Digital Pins 0 and 1 for serial commincation, Pins 2-13 Monitored.
// Analogue pins 0-5 monitored on graph in app.

String colors[]={"R0G0B0","R0G150B0"}; //App Light colours for LOW and HIGH
String receive_chars="abcdefghijkl"; //so app knows which data goes where
int interval=100; //Gives the serial link and app a chance to process data
float voltage; //Assumes 0-1023 range over 5V

void setup() {
 //Initiate Digital pins as Inputs
  for(int i=2;i<=13;i++) pinMode(i, INPUT); 
 
 //Initiate Serial for Bluetooth Communication
  Serial.begin(9600); 
}

void loop() {
  
  //Read Digital Pins and Send results over Bluetooth
    for(int i=2;i<=13;i++){
      Serial.print("*"+String(receive_chars.charAt(i-2))+colors[digitalRead(i)]+"*");
    }

  //Graph - Read Analogue Pins and Send Results over Bluetooth
    Serial.print("*G"); //using 'G' as receive char for graph in app
    for(int i=0;i<=5;i++){
      voltage=analogRead(i)*0.0048828;
      Serial.print(String(voltage)+",");
    }
    Serial.print("*");
    
  //Pause before taking next measurement
    delay(interval); 
}
