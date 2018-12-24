#include <Wire.h>
#include <Adafruit_ADS1015.h>

float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

void setup(void) {
  Serial.begin(250000);
  Serial.println("Hello!");
  
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1115, 0.1875mV/ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
}

void loop(void) {
  int16_t adc0, adc1, adc2, adc3;

  Serial.print("mil start  -  "); Serial.println(millis());
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  Serial.print("mil stop  -  "); Serial.println(millis());
  Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  mVolt - "); Serial.println(adc0 * multiplier);
  Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  mVolt - "); Serial.println(adc1 * multiplier);
  Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  mVolt - "); Serial.println(adc2 * multiplier);
  Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  mVolt - "); Serial.println(adc3 * multiplier);
  Serial.println(" ");
  
  delay(1000);
}
