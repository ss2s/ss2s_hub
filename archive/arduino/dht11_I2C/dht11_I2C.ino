#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include "Arduino.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
dht11 sensor;

#define DHT11PIN 5

byte gradus[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  lcd.createChar(1, gradus);
  lcd.begin(16, 2);
  lcd.clear();
  delay(500);  
}

void loop() {
  lcd.setCursor(0, 0);
  switch (sensor.read(DHT11PIN)) {
    case DHTLIB_OK:
                lcd.clear();
                lcd.print("Read OK");                
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                lcd.clear();
                lcd.print("Checksum error");                
                return;
    case DHTLIB_ERROR_TIMEOUT: 
                lcd.clear();
                lcd.print("Time out error");                 
                return;
    default: 
                lcd.clear();
                lcd.print("Unknown error");                 
                return;
  }
  
  lcd.setCursor(0, 1);
  lcd.print(sensor.temperature);
  lcd.write(1);
  lcd.print("C");  
  lcd.print("  ");
  lcd.print(sensor.humidity);
  lcd.print("%"); 
  
  delay(500);   
}

