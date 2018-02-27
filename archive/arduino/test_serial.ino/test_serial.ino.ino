
char bd; // bluetooth data данные блютуз. переменная для считывания первого управляющего байта
int x_pad,y_pad;

void setup() {

  Serial.begin(9600);
  
}

void loop() {
  
  if (Serial.available()) {
    
    bd = Serial.read();

    if (bd == 'X'){
      
      x_pad = Serial.parseInt();

      Serial.print("*D");
      Serial.println(x_pad);
      
    }
    
    //Serial.print(bd);
    //Serial.print(" ");
   
  }
    
}
