// ProMini
// NRF scanner with bluetoth wi-fi and other RF detected

#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"

#include "U8glib.h"
// U8GLIB_SSD1309_128X64 u8g(13, 11, 10, 9, 8);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

#include "SoftSPI.h"
const uint8_t SOFT_SPI_MISO_PIN = 16;
const uint8_t SOFT_SPI_MOSI_PIN = 15;
const uint8_t SOFT_SPI_SCK_PIN  = 14;
const uint8_t SPI_MODE = 0;

SoftSPI<SOFT_SPI_MISO_PIN, SOFT_SPI_MOSI_PIN, SOFT_SPI_SCK_PIN, SPI_MODE> spi;
U8GLIB_SSD1309_128X64 u8g(14, 15, 8, 7, 6);	// SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RF24 radio(9,10);  // nrf init CE:9, CSN:10, MOSI:11, MISO:12, SCK:13  for UNO
// RF24 radio(48,53);  // nrf init CE, CSN  nrf init:  MISO:50, MOSI:51, SCK:52  for MEGA
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t num_channels = 126;    // количество каналов
const int num_reps = 16;            // циклов прослушивания за один проход 1 - 100
const uint32_t listenTimeMcs = 64;  // слушать микро секунд
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t values[num_channels];        // масив со значенимями
// переменные хранящие состаяние рамки обнаружения заданного сигнала
boolean btChek = 0;
boolean wfChek = 0;
boolean rfChek = 0;
boolean rf24Chek = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioNrfSetup(){
	radio.begin();
	radio.setAutoAck(false);
	// radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

  // Get into standby mode
	radio.startListening();
	radio.stopListening();

	Serial.println("");

	// Print out header, high then low digit
	int i = 0;
	while ( i < num_channels ){
		Serial.print(i>>4,"%x");
		++i;
	}
	Serial.println();
	i = 0;
	while ( i < num_channels ){
		Serial.print(i&0xf,"%x");
		++i;
	}
	Serial.println(); 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void radioNrfLoop(){
	// Clear measurement values
	memset(values,0,sizeof(values));

	// Scan all channels num_reps times
	int rep_counter = num_reps;
	while (rep_counter--){
    	int i = num_channels;
    	while (i--){
    		// Select this channel
    		radio.setChannel(i);

    		// Listen for a little
    		radio.startListening();
    		delayMicroseconds(listenTimeMcs);
    		radio.stopListening();

    		// Did we get a carrier?
    		if ( radio.testCarrier() ){
				++values[i];
    		}
		}
	}

	// Print out channel measurements, clamped to a single hex digit

	btChek = 0;
	wfChek = 0;
	rfChek = 0;
	rf24Chek = 0;

	for(int i=0; i<num_channels; i++){
	    Serial.print(min(0xf,values[i]),"%x");
	    if(values[i] > 0){
	    	if(i >= 49 && i <= 71){wfChek = 1;}
	    	if((i>=2 && i<=14) || (i>=28 && i<=47) || (i>=72 && i<=80)){btChek = 1;}
	    	if(!(i >= 49 && i <= 71) && !((i>=2 && i<=14) || (i>=28 && i<=47) || (i>=72 && i<=80))){rfChek = 1;}
	    	if(1){rf24Chek = 1;}  // all rf signal
	    	// if(i >= 95 && i <= 97){rfChek = 1;}  // rf test
	    }
	}
	Serial.println();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drawSETUP(void) {
	// u8g.setFont(u8g_font_6x10);
	u8g.setFont(u8g_font_10x20r);

	u8g.drawStr( 14, 31, "RF SCANNER");
	u8g.drawStr( 29, 50, "2.4 GHz");

	u8g.drawRFrame(0,12,128,44,15);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void signalBarDraw(byte ys){
	u8g.setFont(u8g_font_micro);
	u8g.drawLine(1,ys,126,ys);
	String sstr;
	char schr[8];
	byte yu = ys+1;
	byte yd;
	for(byte i=1; i<127; i += 9){
		yd = yu + 1;
		// if(i == 64){yd += 1;}
	    u8g.drawLine(i,yu,i,yd);

	    sstr = String(i-1);
		for(int i=0; i<7; i++){schr[i]=sstr[i];}
		schr[7]=0;
		if(i == 1){u8g.drawStr270( i+4, yu+13, schr);}
		else{u8g.drawStr270( i+3, yu+13, schr);}
	    // u8g.drawStr270( i+3, yu+13, schr);
	}
	u8g.drawLine(126,yu,126,yd);
	u8g.drawStr270( 126+2, yu+13, "125");

	for(int i=0; i<num_channels; i++){
		if(values[i] > 0){u8g.drawLine(i+1,ys-1,i+1,ys-min(35,values[i]*5));}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void draw(void) {
	// u8g.drawRFrame(0,0,128,64,10);// u8g.drawRBox(5,40,10,10,2);// u8g.drawLine(15,60,35,60);// u8g.drawPixel(14,59);
	// u8g.drawDisc(30,45,5);// u8g.drawCircle(30,45,8);// u8g.setFont(u8g_font_unifont);// u8g.setFont(u8g_font_5x8);
	// u8g.setFont(u8g_font_osb21);// u8g.setFont(u8g_font_micro);// u8g.setFont(u8g_font_6x10);

	String sstr;
	char schr[8];
	// sstr = String(rxStrctVal.val_O2);
	// for(int i=0; i<7; i++){schr[i]=sstr[i];}
	// schr[7]=0;
	// u8g.drawStr( 25, 41, schr);

	// u8g.setFont(u8g_font_micro);
	// u8g.setFont(u8g_font_6x10r);
	// u8g.setFont(u8g_font_unifont_8_9);

	// btChek = 0;
	// wfChek = 0;
	// rfChek = 0;
	// rf24Chek = 0;

	u8g.setFont(u8g_font_5x8r);
	u8g.drawStr( 3, 9, "blTooth"); if(btChek){u8g.drawRFrame(0,0,40,12,4);}
	u8g.drawStr( 46, 9, "Wi"); u8g.drawLine(56,6,58,6); u8g.drawStr( 60, 9, "Fi"); if(wfChek){u8g.drawRFrame(43,0,29,12,4);}
	u8g.drawStr( 78, 9, "RF"); if(rfChek){u8g.drawRFrame(75,0,15,12,4);}
	u8g.drawStr( 96, 9, "2.4GHz"); if(rf24Chek){u8g.drawRFrame(93,0,35,12,4);}

	signalBarDraw(49);

	if(btChek == 0 && wfChek == 0 && rfChek == 0 && rf24Chek == 0){
		u8g.setFont(u8g_font_10x20r);
		u8g.drawStr( 0, 35, "RF");
		u8g.drawStr( 24, 35, "NO");
		u8g.drawStr( 49, 35, "DETECTED");
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displaySetup(void) {
  // picture loop
  u8g.firstPage();  
  do {
    drawSETUP();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayLoop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  // delay(50);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup(){
	Serial.begin(250000);
	delay(10);
	displaySetup();
	radioNrfSetup();
	delay(1000);
}



void loop(){
	radioNrfLoop();
	displayLoop();
}