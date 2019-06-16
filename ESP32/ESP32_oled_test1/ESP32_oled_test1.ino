// D:/Program Files (x86)/Arduino
// C:/Users/SS2S/Documents/Arduino
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// Initialize the OLED display using brzo_i2c
// 5 -> SDA
// 4 -> SCL
// 0x3c -> ADDR
// SSD1306Wire  display(i2c_oled_ADDR, SDA_pin, SCL_pin);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 5, 4);

// Adapted from Adafruit_SSD1306

// display.getWidth();
// display.getHeight();

// display.clear();
// display.display();

// display.drawLine(0, 0, 10, 10);
// display.display();

// display.drawRect(0, 0, 0, 0);
// display.display();

// uint8_t color = 1;
// display.setColor(BLACK);
// display.setColor(WHITE);
// display.fillRect(1, 1, display.getWidth() - 1, display.getHeight() - 1);
// display.display();

// display.drawCircle(display.getWidth()/2, display.getHeight()/2, 10);  // r = 10
// display.display();

// display.println("ss2s");

void printBuffer(void) {
  // Initialize the log buffer
  // allocate memory to store 8 lines of text and 30 chars per line.
  display.setLogBuffer(5, 30);

  // Some test data
    const char* test[] = {
        " ",
        "PRESS BOOT",
        " " ,
        "ESP32 RUN TIME TESTER",
        " ",
        " ",
        "5",
        "4",
        "3",
        "2",
        "1",
        "START"
    };

  for (uint8_t i = 0; i < 12; i++) {
    display.clear();
    // Print to the screen
    display.println(test[i]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    delay(500);
  }
}

// Print progTime:
uint32_t progTime_h = 0;
uint32_t progTime_m = 0;
uint32_t progTime_m_res = 0;
uint32_t progTime_s = 0;
uint32_t progTime_s_res = 0;
uint32_t progTime_s_res_prev = 60;
int16_t text1hPos = 4;
String timeString = "";
void printProgTime()
{
    do{
        progTime_s_res = millis()/1000 - 7;
        progTime_m_res = progTime_s_res / 60;
        progTime_h = progTime_m_res / 60;
        progTime_m = progTime_m_res - (progTime_h *60);
        progTime_s = progTime_s_res - (progTime_m_res * 60);
    }while(progTime_s_res == progTime_s_res_prev);

    progTime_s_res_prev = progTime_s_res;

    // display.setFont(ArialMT_Plain_16); // make the font smaller
    // display.drawString(text1hPos,  4, "h  " + String(progTime_h));
    // display.drawString(text1hPos, 24, "m  " + String(progTime_m));
    // display.drawString(text1hPos, 44, "s  " + String(progTime_s));
    // display.display();

    if(progTime_h < 10){
        timeString = "0" + String(progTime_h)+ ":";
    }else{
        timeString = String(progTime_h)+ ":";
    }
    if(progTime_m < 10){
        timeString += "0" + String(progTime_m)+ ":";
    }else{
        timeString += String(progTime_m)+ ":";
    }
    if(progTime_s < 10){
        timeString += "0" + String(progTime_s);
    }else{
        timeString += String(progTime_s);
    }
    display.setFont(ArialMT_Plain_24);  // 10, 16, 24
    // display.drawString(20, text1hPos / 2, String(progTime_h) + ":" + String(progTime_m) + ":" + String(progTime_s));
    display.drawString(18, text1hPos / 2, timeString);
    display.display();

    // delay(1000);
    display.clear();
    // display.display();
    text1hPos ++;
    if(text1hPos > 63){
        text1hPos = 4;
    }
}

void setup() {

display.init();
// display.flipScreenVertically();
display.setContrast(1);  // яркость
printBuffer();
delay(1000);
display.clear();
display.display();
}

void loop(){

    printProgTime();

    // delay(1000);
    // display.clear();
    // display.println("ss2s");
    // display.drawLogBuffer(0, 0);
    // display.display();

    // delay(1000);
    // display.clear();
    // display.println("SS2S");
    // display.drawLogBuffer(0, 0);
    // display.display();
}