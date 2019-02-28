#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX
unsigned long timing; // Переменная для хранения точки отсчета

void setup()
{ Serial.begin(9600);
   mySerial.begin(9600);
   pinMode(2, OUTPUT); // ЭТО РЕЛЕ
   digitalWrite(2, HIGH); //  ПОДТЯЖКА ПОРТА
   mySerial.println("ATE0");  //ОТКЛ ЭХО
   delay(300);
   mySerial.println("AT+IPR=9600;"); 
   delay(300); 
  }
    
void loop()
{ 
 { digitalWrite(13, HIGH);// ИНДИКАЦИЯ СВЯЗИ СИМ800 И МИКРОК.
  mySerial.println("AT"); // РЕГИСТАЦИЯ  СЕТИ
 if (millis() - timing > 500){ // Вместо 10000 подставьте нужное вам значение паузы 
 if ( mySerial.find("OK")) // ОТВЕТ  ОК
      { Serial.println("SOEDINENIE OK");
        digitalWrite(13, LOW); }//  МИГАЕМ ВСЕ ОК
 else{ Serial.println("SOEDINENIE NO");
       digitalWrite(13, HIGH); // ЕСЛИ НЕ НАШЛИ ГОРИМ
        }
  timing = millis(); 
 
 }

      /////////////ИЩЕМ ЗВОНОК/////////////
  
  if (mySerial.find("CLIP:")) // если нашли RING
  {
    Serial.println("RING!");
    delay(200);
    mySerial.println("AT+CLIP=1"); // ВКЛ ОПРЕДЕЛИТЕЛЬ НОМЕРА
    delay(300);
    while (1) // в цикле
    { 
      if (mySerial.find("79000000000")) { // ищим номер телефона, если нашли
        { digitalWrite(2, !digitalRead(2)); // инвертируем A1
          delay(200);
          Serial.println("INVERSIA RELE");
          ////////////////////////////////////////
          delay(200);
           if( digitalRead(2)==LOW)//ЕСЛИ РЕЛЕ ВКЛ
           { mySerial.println("AT+CHUP;"); // сбрасываем вызов
            delay(200); 
            Serial.println("VKL RELE");
             delay(1500); 
            mySerial.print("AT+CREC=4,\"C:\\User\\"), mySerial.print(1), mySerial.println(".amr\",0,100");
              }
            else 
           { 
            Serial.println("OTKL  RELE");
            delay(6000);
             mySerial.println("AT+CHUP;"); // сбрасываем вызов
             delay(2000);
             
           mySerial.print("AT+CREC=4,\"C:\\User\\"), mySerial.print(5), mySerial.println(".amr\",0,100");
           }
           if (mySerial.find("CLIP:"))    
               break; 
        }
           
        }  
      
      else { // иначе
        mySerial.println("AT"); // спрашиваем состояние модема
        delay(200);
        if (mySerial.find("OK"))
          break;   // и если он в "готовности", выходим из цикла
      }            // если звонок в процессе, возвращает +CPAS: 3
    }              // и крутимся дальше

    mySerial.println("AT+CHUP;"); // сбрасываем вызов
    delay(200);

  }
}
}     
