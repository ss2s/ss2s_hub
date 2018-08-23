#define LOG_OUT 1 // use the log output function
#define FHT_N 32  // set to 256 point fht
#include "FHT.h"

#include "FastLED.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ

#define BRIGHTNESS 250           // яркость (0 - 255)

#define MIC_PIN 0               // вход микрофона
#define DATA_PIN 6              // выход на светодиоды

#define DEL_FOR_LOOP 30*VOLNA   // задержка между обновлениями (плавность анимации)
#define VOLNA 1                 // эфект волны 0; без ефекта 1

#define PREAMP_EQ 1             // предуселитель для еквалайзера 
byte equalizer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; // еквалайзер

#define IN_VOLTS 1              // вольт в аудиовыходе (только 1 или 5) эта настройка влияет на чувствительность
                                // если 1 то Arduino будет оцифровывать сигнал от 0 до 1 вольт, если 5 то от 0 до 5
// КОНЕЦ НАСТРОЕК
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// матрица
#define HEIGHT 9 // высота матрицы (количество диодов)
#define WIDTH 16  // ширина матрицы (количество диодов)
#define NUM_LEDS WIDTH * HEIGHT
CRGB leds[NUM_LEDS];

int maxValForLoop = 0;
int curentValForLoop = 0;
int semplForFilter[WIDTH];
byte curentVerticalPos[WIDTH];

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void prevSpectrAnalizer(){  // оцифровка звука
  for (int i = 0 ; i < FHT_N ; i++) {
    int sample = analogRead(MIC_PIN);
    fht_input[i] = sample; // put real data into bins
  }
  fht_window();  // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run();     // process the data in the fht
  fht_mag_log(); // take the output of the fht
}

void preampFilter(){  // считывание в масив и эквалайзер

  maxValForLoop = 0;

  for (int curBin = 0; curBin < FHT_N / 2; ++curBin){

    semplForFilter[curBin] = fht_log_out[curBin] * equalizer[curBin] * PREAMP_EQ;

    curentValForLoop = semplForFilter[curBin];
    if(maxValForLoop < curentValForLoop){
      maxValForLoop = curentValForLoop;
    }
  }
}

void remapFilter(){
  if(maxValForLoop > 0){

    for (int curBin = 0; curBin < FHT_N / 2; ++curBin){
      semplForFilter[curBin] = constrain(semplForFilter[curBin], 0, maxValForLoop);
      semplForFilter[curBin] = map(semplForFilter[curBin], 0, maxValForLoop, 0, HEIGHT
        );
    }
  }
}

void curentVertikalPosFilter(){
  if(maxValForLoop > 0){
    for(int i=0;i<WIDTH;i++){
      if((curentVerticalPos[i] < semplForFilter[i])&&(curentVerticalPos[i] < HEIGHT)){
        curentVerticalPos[i] ++;
      }
      else if((curentVerticalPos[i] > semplForFilter[i])&&(curentVerticalPos[i] > 0)){
        curentVerticalPos[i] --;
      }
    }
  }else{
    for(int i=0;i<WIDTH;i++){
      if(curentVerticalPos[i] > 0){
        curentVerticalPos[i] --;
      }
    }
  }  
}

void drawMatrix(){

  int ggi = 0;
  int ggiPlus = ggi;
  int sumGgiPlus = 0;
  int sumGgiPlusCur = 0;

  for(int gi=0;gi<WIDTH;gi++){
    for(int li=1;li<=HEIGHT;li++){
      sumGgiPlus = ggiPlus + curentVerticalPos[gi];
      sumGgiPlusCur = ggiPlus + li;
      if(sumGgiPlusCur <= sumGgiPlus){
        leds[ggi] = CRGB::Blue;
      }else{
        leds[ggi] = CRGB::Black;
      }
      ggi++;
    }
    if(VOLNA == 0){
      FastLED.show();
    }
    ggiPlus = ggi;
  }
  if(VOLNA > 0){
    FastLED.show();
  }
}

void setup() {
  Serial.begin(9600);

  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  if(IN_VOLTS == 1){
    analogReference(INTERNAL);
  }

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  prevSpectrAnalizer();
  preampFilter();
  remapFilter();
  curentVertikalPosFilter();
  drawMatrix();
  delay(DEL_FOR_LOOP);
}
