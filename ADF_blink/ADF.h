////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  ВЫБОР КОНТРОЛЛЕРА
#define PLATFORM 328 // 328 - UNO; 324 - Leonardo; 2560 - MEGA2560; 13 - attiny13;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM == 328 // UNO (Atmega328)  (UNO, Nano, proMini)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 324 // Leonardo (Atmega32u4)  (Leonardo, proMicro)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 2560 // MEGA2560 (Atmega2560)  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 13 // attiny13 (Attiny13)

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////