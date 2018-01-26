////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ЗАГОЛВОЧНЫЙ ФАЙЛ С МАКРОСАМИ ДЛЯ Atmega328, Atmega32u4, Atmega2560, Attiny13
//  ДЛЯ БОЛЕЕ УДОБНОГО УПРАВЛЕНИЯ ПОРТАМИ ЧЕРЕЗ РЕГИСРЫ С ПОМОЩЬЮ МАКРОСОВ
//  ФАЙЛ НЕ ЗАНИМАЕТ ПАМЯТЬ УСТРОЙСТВА. ИСПОЛЬЗУЕТСЯ ТОЛЬКО ПРИ КОМПИЛЯЦИИ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ADF.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ВЫБОР КОНТРОЛЛЕРА
#define PLATFORM 328 // 328 - UNO; 324 - Leonardo; 2560 - MEGA2560; 13 - attiny13;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM == 328 // UNO (Atmega328)  (UNO, Nano, proMini)

//  pinMode(pin, INPUT);
#define ADF_PMI0
#define ADF_PMI1
#define ADF_PMI2
#define ADF_PMI3
#define ADF_PMI4
#define ADF_PMI5
#define ADF_PMI6
#define ADF_PMI7
#define ADF_PMI8
#define ADF_PMI9
#define ADF_PMI10
#define ADF_PMI11
#define ADF_PMI12
#define ADF_PMI13
#define ADF_PMI14
#define ADF_PMI15
#define ADF_PMI16
#define ADF_PMI17
#define ADF_PMI18
#define ADF_PMI19
//  pinMode(pin, OUTPUT);
#define ADF_PMO0
#define ADF_PMO1
#define ADF_PMO2
#define ADF_PMO3
#define ADF_PMO4
#define ADF_PMO5
#define ADF_PMO6
#define ADF_PMO7
#define ADF_PMO8
#define ADF_PMO9
#define ADF_PMO10
#define ADF_PMO11
#define ADF_PMO12
#define ADF_PMO13
#define ADF_PMO14
#define ADF_PMO15
#define ADF_PMO16
#define ADF_PMO17
#define ADF_PMO18
#define ADF_PMO19
//  pinMode(pin, INPUT_PULLUP);
#define ADF_PMIP0
#define ADF_PMIP1
#define ADF_PMIP2
#define ADF_PMIP3
#define ADF_PMIP4
#define ADF_PMIP5
#define ADF_PMIP6
#define ADF_PMIP7
#define ADF_PMIP8
#define ADF_PMIP9
#define ADF_PMIP10
#define ADF_PMIP11
#define ADF_PMIP12
#define ADF_PMIP13
#define ADF_PMIP14
#define ADF_PMIP15
#define ADF_PMIP16
#define ADF_PMIP17
#define ADF_PMIP18
#define ADF_PMIP19
//  digitalWrite(pin, HIGH);
#define ADF_DWH0
#define ADF_DWH1
#define ADF_DWH2
#define ADF_DWH3
#define ADF_DWH4
#define ADF_DWH5
#define ADF_DWH6
#define ADF_DWH7
#define ADF_DWH8
#define ADF_DWH9
#define ADF_DWH10
#define ADF_DWH11
#define ADF_DWH12
#define ADF_DWH13
#define ADF_DWH14
#define ADF_DWH15
#define ADF_DWH16
#define ADF_DWH17
#define ADF_DWH18
#define ADF_DWH19
//  digitalWrite(pin, LOW);
#define ADF_DWL0
#define ADF_DWL1
#define ADF_DWL2
#define ADF_DWL3
#define ADF_DWL4
#define ADF_DWL5
#define ADF_DWL6
#define ADF_DWL7
#define ADF_DWL8
#define ADF_DWL9
#define ADF_DWL10
#define ADF_DWL11
#define ADF_DWL12
#define ADF_DWL13
#define ADF_DWL14
#define ADF_DWL15
#define ADF_DWL16
#define ADF_DWL17
#define ADF_DWL18
#define ADF_DWL19
//  digitalRead(pin);
#define ADF_DR0
#define ADF_DR1
#define ADF_DR2
#define ADF_DR3
#define ADF_DR4
#define ADF_DR5
#define ADF_DR6
#define ADF_DR7
#define ADF_DR8
#define ADF_DR9
#define ADF_DR10
#define ADF_DR11
#define ADF_DR12
#define ADF_DR13
#define ADF_DR14
#define ADF_DR15
#define ADF_DR16
#define ADF_DR17
#define ADF_DR18
#define ADF_DR19

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 324 // Leonardo (Atmega32u4)  (Leonardo, proMicro)

//  pinMode(pin, INPUT);
#define ADF_PMI0
#define ADF_PMI1
#define ADF_PMI2
#define ADF_PMI3
#define ADF_PMI4
#define ADF_PMI5
#define ADF_PMI6
#define ADF_PMI7
#define ADF_PMI8
#define ADF_PMI9
#define ADF_PMI10
#define ADF_PMI11
#define ADF_PMI12
#define ADF_PMI13
#define ADF_PMI14
#define ADF_PMI15
#define ADF_PMI16
#define ADF_PMI17
#define ADF_PMI18
#define ADF_PMI19
//  pinMode(pin, OUTPUT);
#define ADF_PMO0
#define ADF_PMO1
#define ADF_PMO2
#define ADF_PMO3
#define ADF_PMO4
#define ADF_PMO5
#define ADF_PMO6
#define ADF_PMO7
#define ADF_PMO8
#define ADF_PMO9
#define ADF_PMO10
#define ADF_PMO11
#define ADF_PMO12
#define ADF_PMO13
#define ADF_PMO14
#define ADF_PMO15
#define ADF_PMO16
#define ADF_PMO17
#define ADF_PMO18
#define ADF_PMO19
//  pinMode(pin, INPUT_PULLUP);
#define ADF_PMIP0
#define ADF_PMIP1
#define ADF_PMIP2
#define ADF_PMIP3
#define ADF_PMIP4
#define ADF_PMIP5
#define ADF_PMIP6
#define ADF_PMIP7
#define ADF_PMIP8
#define ADF_PMIP9
#define ADF_PMIP10
#define ADF_PMIP11
#define ADF_PMIP12
#define ADF_PMIP13
#define ADF_PMIP14
#define ADF_PMIP15
#define ADF_PMIP16
#define ADF_PMIP17
#define ADF_PMIP18
#define ADF_PMIP19
//  digitalWrite(pin, HIGH);
#define ADF_DWH0
#define ADF_DWH1
#define ADF_DWH2
#define ADF_DWH3
#define ADF_DWH4
#define ADF_DWH5
#define ADF_DWH6
#define ADF_DWH7
#define ADF_DWH8
#define ADF_DWH9
#define ADF_DWH10
#define ADF_DWH11
#define ADF_DWH12
#define ADF_DWH13
#define ADF_DWH14
#define ADF_DWH15
#define ADF_DWH16
#define ADF_DWH17
#define ADF_DWH18
#define ADF_DWH19
//  digitalWrite(pin, LOW);
#define ADF_DWL0
#define ADF_DWL1
#define ADF_DWL2
#define ADF_DWL3
#define ADF_DWL4
#define ADF_DWL5
#define ADF_DWL6
#define ADF_DWL7
#define ADF_DWL8
#define ADF_DWL9
#define ADF_DWL10
#define ADF_DWL11
#define ADF_DWL12
#define ADF_DWL13
#define ADF_DWL14
#define ADF_DWL15
#define ADF_DWL16
#define ADF_DWL17
#define ADF_DWL18
#define ADF_DWL19
//  digitalRead(pin);
#define ADF_DR0
#define ADF_DR1
#define ADF_DR2
#define ADF_DR3
#define ADF_DR4
#define ADF_DR5
#define ADF_DR6
#define ADF_DR7
#define ADF_DR8
#define ADF_DR9
#define ADF_DR10
#define ADF_DR11
#define ADF_DR12
#define ADF_DR13
#define ADF_DR14
#define ADF_DR15
#define ADF_DR16
#define ADF_DR17
#define ADF_DR18
#define ADF_DR19

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 2560 // MEGA2560 (Atmega2560)

//  pinMode(pin, INPUT);
#define ADF_PMI0
#define ADF_PMI1
#define ADF_PMI2
#define ADF_PMI3
#define ADF_PMI4
#define ADF_PMI5
#define ADF_PMI6
#define ADF_PMI7
#define ADF_PMI8
#define ADF_PMI9
#define ADF_PMI10
#define ADF_PMI11
#define ADF_PMI12
#define ADF_PMI13
#define ADF_PMI14
#define ADF_PMI15
#define ADF_PMI16
#define ADF_PMI17
#define ADF_PMI18
#define ADF_PMI19
#define ADF_PMI20
#define ADF_PMI21
#define ADF_PMI22
#define ADF_PMI23
#define ADF_PMI24
#define ADF_PMI25
#define ADF_PMI26
#define ADF_PMI27
#define ADF_PMI28
#define ADF_PMI29
#define ADF_PMI30
#define ADF_PMI31
#define ADF_PMI32
#define ADF_PMI33
#define ADF_PMI34
#define ADF_PMI35
#define ADF_PMI36
#define ADF_PMI37
#define ADF_PMI38
#define ADF_PMI39
#define ADF_PMI40
#define ADF_PMI41
#define ADF_PMI42
#define ADF_PMI43
#define ADF_PMI44
#define ADF_PMI45
#define ADF_PMI46
#define ADF_PMI47
#define ADF_PMI48
#define ADF_PMI49
#define ADF_PMI50
#define ADF_PMI51
#define ADF_PMI52
#define ADF_PMI53
//  pinMode(pin, OUTPUT);
#define ADF_PMO0
#define ADF_PMO1
#define ADF_PMO2
#define ADF_PMO3
#define ADF_PMO4
#define ADF_PMO5
#define ADF_PMO6
#define ADF_PMO7
#define ADF_PMO8
#define ADF_PMO9
#define ADF_PMO10
#define ADF_PMO11
#define ADF_PMO12
#define ADF_PMO13
#define ADF_PMO14
#define ADF_PMO15
#define ADF_PMO16
#define ADF_PMO17
#define ADF_PMO18
#define ADF_PMO19
#define ADF_PMO20
#define ADF_PMO21
#define ADF_PMO22
#define ADF_PMO23
#define ADF_PMO24
#define ADF_PMO25
#define ADF_PMO26
#define ADF_PMO27
#define ADF_PMO28
#define ADF_PMO29
#define ADF_PMO30
#define ADF_PMO31
#define ADF_PMO32
#define ADF_PMO33
#define ADF_PMO34
#define ADF_PMO35
#define ADF_PMO36
#define ADF_PMO37
#define ADF_PMO38
#define ADF_PMO39
#define ADF_PMO40
#define ADF_PMO41
#define ADF_PMO42
#define ADF_PMO43
#define ADF_PMO44
#define ADF_PMO45
#define ADF_PMO46
#define ADF_PMO47
#define ADF_PMO48
#define ADF_PMO49
#define ADF_PMO50
#define ADF_PMO51
#define ADF_PMO52
#define ADF_PMO53
//  pinMode(pin, INPUT_PULLUP);
#define ADF_PMIP0
#define ADF_PMIP1
#define ADF_PMIP2
#define ADF_PMIP3
#define ADF_PMIP4
#define ADF_PMIP5
#define ADF_PMIP6
#define ADF_PMIP7
#define ADF_PMIP8
#define ADF_PMIP9
#define ADF_PMIP10
#define ADF_PMIP11
#define ADF_PMIP12
#define ADF_PMIP13
#define ADF_PMIP14
#define ADF_PMIP15
#define ADF_PMIP16
#define ADF_PMIP17
#define ADF_PMIP18
#define ADF_PMIP19
#define ADF_PMIP20
#define ADF_PMIP21
#define ADF_PMIP22
#define ADF_PMIP23
#define ADF_PMIP24
#define ADF_PMIP25
#define ADF_PMIP26
#define ADF_PMIP27
#define ADF_PMIP28
#define ADF_PMIP29
#define ADF_PMIP30
#define ADF_PMIP31
#define ADF_PMIP32
#define ADF_PMIP33
#define ADF_PMIP34
#define ADF_PMIP35
#define ADF_PMIP36
#define ADF_PMIP37
#define ADF_PMIP38
#define ADF_PMIP39
#define ADF_PMIP40
#define ADF_PMIP41
#define ADF_PMIP42
#define ADF_PMIP43
#define ADF_PMIP44
#define ADF_PMIP45
#define ADF_PMIP46
#define ADF_PMIP47
#define ADF_PMIP48
#define ADF_PMIP49
#define ADF_PMIP50
#define ADF_PMIP51
#define ADF_PMIP52
#define ADF_PMIP53
//  digitalWrite(pin, HIGH);
#define ADF_DWH0
#define ADF_DWH1
#define ADF_DWH2
#define ADF_DWH3
#define ADF_DWH4
#define ADF_DWH5
#define ADF_DWH6
#define ADF_DWH7
#define ADF_DWH8
#define ADF_DWH9
#define ADF_DWH10
#define ADF_DWH11
#define ADF_DWH12
#define ADF_DWH13
#define ADF_DWH14
#define ADF_DWH15
#define ADF_DWH16
#define ADF_DWH17
#define ADF_DWH18
#define ADF_DWH19
#define ADF_DWH20
#define ADF_DWH21
#define ADF_DWH22
#define ADF_DWH23
#define ADF_DWH24
#define ADF_DWH25
#define ADF_DWH26
#define ADF_DWH27
#define ADF_DWH28
#define ADF_DWH29
#define ADF_DWH30
#define ADF_DWH31
#define ADF_DWH32
#define ADF_DWH33
#define ADF_DWH34
#define ADF_DWH35
#define ADF_DWH36
#define ADF_DWH37
#define ADF_DWH38
#define ADF_DWH39
#define ADF_DWH40
#define ADF_DWH41
#define ADF_DWH42
#define ADF_DWH43
#define ADF_DWH44
#define ADF_DWH45
#define ADF_DWH46
#define ADF_DWH47
#define ADF_DWH48
#define ADF_DWH49
#define ADF_DWH50
#define ADF_DWH51
#define ADF_DWH52
#define ADF_DWH53
//  digitalWrite(pin, LOW);
#define ADF_DWL0
#define ADF_DWL1
#define ADF_DWL2
#define ADF_DWL3
#define ADF_DWL4
#define ADF_DWL5
#define ADF_DWL6
#define ADF_DWL7
#define ADF_DWL8
#define ADF_DWL9
#define ADF_DWL10
#define ADF_DWL11
#define ADF_DWL12
#define ADF_DWL13
#define ADF_DWL14
#define ADF_DWL15
#define ADF_DWL16
#define ADF_DWL17
#define ADF_DWL18
#define ADF_DWL19
#define ADF_DWL20
#define ADF_DWL21
#define ADF_DWL22
#define ADF_DWL23
#define ADF_DWL24
#define ADF_DWL25
#define ADF_DWL26
#define ADF_DWL27
#define ADF_DWL28
#define ADF_DWL29
#define ADF_DWL30
#define ADF_DWL31
#define ADF_DWL32
#define ADF_DWL33
#define ADF_DWL34
#define ADF_DWL35
#define ADF_DWL36
#define ADF_DWL37
#define ADF_DWL38
#define ADF_DWL39
#define ADF_DWL40
#define ADF_DWL41
#define ADF_DWL42
#define ADF_DWL43
#define ADF_DWL44
#define ADF_DWL45
#define ADF_DWL46
#define ADF_DWL47
#define ADF_DWL48
#define ADF_DWL49
#define ADF_DWL50
#define ADF_DWL51
#define ADF_DWL52
#define ADF_DWL53
//  digitalRead(pin);
#define ADF_DR0
#define ADF_DR1
#define ADF_DR2
#define ADF_DR3
#define ADF_DR4
#define ADF_DR5
#define ADF_DR6
#define ADF_DR7
#define ADF_DR8
#define ADF_DR9
#define ADF_DR10
#define ADF_DR11
#define ADF_DR12
#define ADF_DR13
#define ADF_DR14
#define ADF_DR15
#define ADF_DR16
#define ADF_DR17
#define ADF_DR18
#define ADF_DR19
#define ADF_DR20
#define ADF_DR21
#define ADF_DR22
#define ADF_DR23
#define ADF_DR24
#define ADF_DR25
#define ADF_DR26
#define ADF_DR27
#define ADF_DR28
#define ADF_DR29
#define ADF_DR30
#define ADF_DR31
#define ADF_DR32
#define ADF_DR33
#define ADF_DR34
#define ADF_DR35
#define ADF_DR36
#define ADF_DR37
#define ADF_DR38
#define ADF_DR39
#define ADF_DR40
#define ADF_DR41
#define ADF_DR42
#define ADF_DR43
#define ADF_DR44
#define ADF_DR45
#define ADF_DR46
#define ADF_DR47
#define ADF_DR48
#define ADF_DR49
#define ADF_DR50
#define ADF_DR51
#define ADF_DR52
#define ADF_DR53

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif PLATFORM == 13 // attiny13 (Attiny13)

//  pinMode(pin, INPUT);
#define ADF_PMI0
#define ADF_PMI1
#define ADF_PMI2
#define ADF_PMI3
#define ADF_PMI4
#define ADF_PMI5
//  pinMode(pin, OUTPUT);
#define ADF_PMO0
#define ADF_PMO1
#define ADF_PMO2
#define ADF_PMO3
#define ADF_PMO4
#define ADF_PMO5
//  pinMode(pin, INPUT_PULLUP);
#define ADF_PMIP0
#define ADF_PMIP1
#define ADF_PMIP2
#define ADF_PMIP3
#define ADF_PMIP4
#define ADF_PMIP5
//  digitalWrite(pin, HIGH);
#define ADF_DWH0
#define ADF_DWH1
#define ADF_DWH2
#define ADF_DWH3
#define ADF_DWH4
#define ADF_DWH5
//  digitalWrite(pin, LOW);
#define ADF_DWL0
#define ADF_DWL1
#define ADF_DWL2
#define ADF_DWL3
#define ADF_DWL4
#define ADF_DWL5
//  digitalRead(pin);
#define ADF_DR0
#define ADF_DR1
#define ADF_DR2
#define ADF_DR3
#define ADF_DR4
#define ADF_DR5

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////