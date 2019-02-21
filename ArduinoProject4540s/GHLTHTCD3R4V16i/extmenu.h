// Объявляем переменные
byte actualIndex = 0;
bool isParamEditMode  = false;            // Флаг режима редактирования параметра
int tmpValue          = 0;                // Временная переменная для хранения изменяемого параметра
bool flagMenuState = 0;                   // флаг состояния меню 1 в меню, 0 нет


// ОПИСАНИЕ МАСИВА С МЕНЮ:

// 0 Идентификационный уникальный индекс ID
// 1 ID родителя
// 2 Является ли пункт изменяемым параметром
// 3 Актуальное значение
// 4 Минимально возможное значение параметра
// 5 Максимально возможное значение параметра

// ПОЗИЦИЯ ID --;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte punktovMenuVsego = 78;

byte masMenu[][6] = {
    {1,0,0,0,0,0},     // МЕНЮ (ВОЙТИ)


    {2,1,0,0,0,0},             // Р4 СВЕТ 

    {3,2,1,1,0,2},                     // ВЫБОР РЕЖИМА - 2 : 1 : 0

    {4,2,0,0,0,0},                     // 2 РЕЖИМ ДОСВЕТКА (НАСТРОЙКА РЕЖИМА 2)

    {5,4,1,5,0,23},                            // 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    {6,4,1,30,0,59},                           // 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    {7,4,1,6,0,23},                            // 1 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 1 h 
    {8,4,1,30,0,59},                           // 1 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 1 m
    {9,4,1,20,0,23},                           // 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    {10,4,1,30,0,59},                          // 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    {11,4,1,22,0,23},                          // 2 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 2 h
    {12,4,1,30,0,59},                          // 2 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 2 m

    {13,2,0,0,0,0},                    // 1 РЕЖИМ СВЕТОЦИКЛ (НАСТРОЙКА РЕЖИМА 1)

    {14,13,1,5,0,23},                          // ВКЛЮЧЕНИЕ В ЧАСОВ                ON h
    {15,13,1,30,0,59},                         // ВКЛЮЧЕНИЕ В МИНУТ                ON m
    {16,13,1,17,0,23},                         // ВЫКЛЮЧЕНИЕ В ЧАСОВ              OFF h
    {17,13,1,30,0,59},                         // ВЫКЛЮЧЕНИЕ В МИНУТ              OFF m



    {18,1,0,0,0,0},            // Р3 ПОЛИВ

    {19,18,1,2,0,3},                   // ВЫБОР РЕЖИМА - 3 : 2 : 1 : 0

    {20,18,0,0,0,0},                   // 3 ОТ ДАТЧИКА ПОЧВЫ (НАСТРОЙКА РЕЖИМА)

    {21,20,1,1,0,2},                           // ТИП ДАТЧИКА                     0 1 2
    {22,20,1,50,0,100},                        // ПОРОГ АНАЛОГОВОГО ДАТЧИКА  если тип 2

    {23,18,0,0,0,0},                   // 2 ПО ВРЕМЕНИ (НАСТРОЙКА РЕЖИМА)

    {24,23,1,5,0,23},                          // 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    {25,23,1,15,0,59},                         // 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    {26,23,1,12,0,23},                         // 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    {27,23,1,15,0,59},                         // 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    {28,23,1,18,0,23},                         // 3 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 3 h
    {29,23,1,15,0,59},                         // 3 ВКЛЮЧЕНИЕ В МИНУТ            ON 3 m
    {30,23,1,0,0,23},                          // ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
    {31,23,1,10,0,59},                         // ВРЕМЯ РАБОТЫ МИНУТ         POWER ON m

    {32,18,0,0,0,0},                   // 1 ЦИКЛИЧЕСКИЙ (НАСТРОЙКА РЕЖИМА)

    {33,32,1,4,0,23},                          // МЕЖДУ ПОЛИВАМИ ЧАСОВ          DELAY h
    {34,32,1,0,0,59},                          // МЕЖДУ ПОЛИВАМИ МИНУТ          DELAY m
    {35,32,1,0,0,23},                          // ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
    {36,32,1,10,0,59},                         // ВРЕМЯ РАБОТЫ МИНУТ         POWER ON m



    {37,1,0,0,0,0},            // Р2 ТЕМПЕРАТУРА

    {38,37,1,1,0,3},                   // ВЫБОР РЕЖИМА - 3 : 2 : 1 : 0

    {39,37,0,0,0,0},                   // 3 НАСТРАИВАЕМЫЙ (НАСТРОЙКА РЕЖИМА)

    {40,39,1,25,0,50},                         // ТЕМПЕРАТУРА ВКЛЮЧЕНИЯ              ON
    {41,39,1,23,0,50},                         // ТЕМПЕРАТУРА ВЫКЛЮЧЕНИЯ            OFF

    {42,37,0,0,0,0},                   // 2 НАГРЕВ (НАСТРОЙКА РЕЖИМА)

    {43,42,1,25,0,50},                         // ЖЕЛАЕМАЯ ТЕМПЕРАТУРА              VAL
    {44,42,1,2,0,10},                          // ГИСТЕРЕЗИС                        GIS

    {45,37,0,0,0,0},                   // 1 ОХЛАЖДЕНИЕ (НАСТРОЙКА РЕЖИМА)

    {46,45,1,20,0,50},                         // ЖЕЛАЕМАЯ ТЕМПЕРАТУРА              VAL
    {47,45,1,1,0,10},                          // ГИСТЕРЕЗИС                        GIS



    {48,1,0,0,0,0},            // Р1 ВЛАЖНОСТЬ

    {49,48,1,2,0,3},                   // ВЫБОР РЕЖИМА - 3 : 2 : 1 : 0

    {50,48,0,0,0,0},                   // 3 НАСТРАИВАЕМЫЙ (НАСТРОЙКА РЕЖИМА)

    {51,50,1,60,20,95},                        // ВЛАЖНОСТЬ ВКЛЮЧЕНИЯ                ON
    {52,50,1,80,20,95},                        // ВЛАЖНОСТЬ ВЫКЛЮЧЕНИЯ              OFF

    {53,48,0,0,0,0},                   // 2 УВЛАЖНЕНИЕ (НАСТРОЙКА РЕЖИМА)

    {54,53,1,80,20,95},                        // ЖЕЛАЕМАЯ ВЛАЖНОСТЬ                VAL
    {55,53,1,5,0,10},                          // ГИСТЕРЕЗИС                        GIS

    {56,48,0,0,0,0},                   // 1 СУШКА (НАСТРОЙКА РЕЖИМА)

    {57,56,1,60,20,95},                        // ЖЕЛАЕМАЯ ВЛАЖНОСТЬ                VAL
    {58,56,1,5,0,10},                          // ГИСТЕРЕЗИС                        GIS



    {59,1,0,0,0,0},            // РУЧНОЕ УПРАВЛЕНИЕ (ВХОД В УПРАВЛЕНИЕ)

    {60,59,0,0,0,0},                   // Р4 СВЕТ                                ON/OFF
    {61,59,0,0,0,0},                   // Р3 ПОЛИВ                               ON/OFF
    {62,59,0,0,0,0},                   // Р2 ТЕМПЕРАТУРА                         ON/OFF
    {63,59,0,0,0,0},                   // Р1 ВЛАЖНОСТЬ                           ON/OFF

    {64,1,1,100,0,100},        // ЯРКОСТЬ ДИСПЛЕЯ

    {65,1,0,0,0,0},            // НАСТРОЙКА ЧАСОВ

    {66,65,1,18,0,99},                  // ГОД
    {67,65,1,3,1,12},                   // МЕСЯЦ
    {68,65,1,22,1,31},                  // ДЕНЬ
    {69,65,1,4,1,7},                    // ДЕНЬ НЕДЕЛИ
    {70,65,1,12,0,23},                  // ЧАСЫ
    {71,65,1,30,0,59},                  // МИНУТЫ
    {72,65,1,0,0,59},                   // СЕКУНДЫ
    {73,65,0,0,0,0},                    // СОХРАНИТЬ                                SAVE

    {74,1,0,0,0,0},             // КАЛИБРОВКА ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ

    {75,74,1,1,0,100},                   // ТЕМПЕРАТУРА
    {76,74,1,27,0,100},                  // ВЛАЖНОСТЬ

    {77,1,1,0,0,1},             // LANGUAGE

    {78,1,0,0,0,0}              // EXIT
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nameIDlcd(byte _ID){

    langFlag = masMenu[76][3];

    switch (_ID) {

        case 0:
        if(langFlag == 0){}  // ENG
        else if(langFlag == 1){}  // РУС
        lcd.print(F("0 menu"));
        break;

        case 1:
        if(langFlag == 0){lcd.print(F("MENU"));}  // ENG
        else if(langFlag == 1){lcd.print(F("MEHY"));}  // РУС
        break;

        case 2:
        if(langFlag == 0){lcd.print(F("R4 LIGHT"));}  // ENG
        else if(langFlag == 1){lcd.print(F("P4 CBET"));}  // РУС
        break;

        case 3:
        if(langFlag == 0){lcd.print(F("mode"));}  // ENG
        else if(langFlag == 1){lcd.print(F("regim"));}  // РУС
        break;

        case 4:
        if(langFlag == 0){lcd.print(F("m2 DOSVET"));}  // ENG
        else if(langFlag == 1){lcd.print(F("m2 DOCBET"));}  // РУС
        break;

        case 5:
        if(langFlag == 0){lcd.print(F("1 on H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 6:
        if(langFlag == 0){lcd.print(F("1 on M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 7:
        if(langFlag == 0){lcd.print(F("1 off H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 8:
        if(langFlag == 0){lcd.print(F("1 off M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 9:
        if(langFlag == 0){lcd.print(F("2 on H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 10:
        if(langFlag == 0){lcd.print(F("2 on M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 11:
        if(langFlag == 0){lcd.print(F("2 off H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 12:
        if(langFlag == 0){lcd.print(F("2 off M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 13:
        if(langFlag == 0){lcd.print(F("m1 LIGHTCYCLE"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 14:
        if(langFlag == 0){lcd.print(F("on Hour"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 15:
        if(langFlag == 0){lcd.print(F("on Minute"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 16:
        if(langFlag == 0){lcd.print(F("off Hour"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 17:
        if(langFlag == 0){lcd.print(F("off Minute"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 18:
        if(langFlag == 0){lcd.print(F("R3 WATER set"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 19:
        if(langFlag == 0){lcd.print(F("R3 mode"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 20:
        if(langFlag == 0){lcd.print(F("m3 SOIL SENSOR"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 21:
        if(langFlag == 0){lcd.print(F("sensor type"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 22:
        if(langFlag == 0){lcd.print(F("thrshld"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 23:
        if(langFlag == 0){lcd.print(F("m2 ON TIME"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 24:
        if(langFlag == 0){lcd.print(F("1 on H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 25:
        if(langFlag == 0){lcd.print(F("1 on M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 26:
        if(langFlag == 0){lcd.print(F("2 on H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 27:
        if(langFlag == 0){lcd.print(F("2 on M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 28:
        if(langFlag == 0){lcd.print(F("3 on H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 29:
        if(langFlag == 0){lcd.print(F("3 on M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 30:
        if(langFlag == 0){lcd.print(F("wrk time H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 31:
        if(langFlag == 0){lcd.print(F("wrk time M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 32:
        if(langFlag == 0){lcd.print(F("m1 CYCLE"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 33:
        if(langFlag == 0){lcd.print(F("btw time H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 34:
        if(langFlag == 0){lcd.print(F("btw time M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 35:
        if(langFlag == 0){lcd.print(F("wrk time H"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 36:
        if(langFlag == 0){lcd.print(F("wrk time M"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 37:
        if(langFlag == 0){lcd.print(F("R2 TEMP set"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 38:
        if(langFlag == 0){lcd.print(F("R2 mode"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 39:
        if(langFlag == 0){lcd.print(F("m3 CUSTOMIZE"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 40:
        if(langFlag == 0){lcd.print(F("on temp"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 41:
        if(langFlag == 0){lcd.print(F("off temp"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 42:
        if(langFlag == 0){lcd.print(F("m2 HEAT"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 43:
        if(langFlag == 0){lcd.print(F("targt temp"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 44:
        if(langFlag == 0){lcd.print(F("hysteresis"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 45:
        if(langFlag == 0){lcd.print(F("m1 COOLING"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 46:
        if(langFlag == 0){lcd.print(F("targt temp"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 47:
        if(langFlag == 0){lcd.print(F("hysteresis"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 48:
        if(langFlag == 0){lcd.print(F("R1 HUMIDTY set"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 49:
        if(langFlag == 0){lcd.print(F("R1 mode"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 50:
        if(langFlag == 0){lcd.print(F("m3 CUSTOMIZE"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 51:
        if(langFlag == 0){lcd.print(F("on hmdt"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 52:
        if(langFlag == 0){lcd.print(F("off hmdt"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 53:
        if(langFlag == 0){lcd.print(F("m2 MOISTURIZNG"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 54:
        if(langFlag == 0){lcd.print(F("targt hmdt"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 55:
        if(langFlag == 0){lcd.print(F("hysteresis"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 56:
        if(langFlag == 0){lcd.print(F("m1 DRYING"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 57:
        if(langFlag == 0){lcd.print(F("targt hmdt"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 58:
        if(langFlag == 0){lcd.print(F("hysteresis"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 59:
        if(langFlag == 0){lcd.print(F("MANUAL CONTROL"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 60:
        if(langFlag == 0){lcd.print(F("r4 lght SW"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 61:
        if(langFlag == 0){lcd.print(F("r3 wtrg SW"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 62:
        if(langFlag == 0){lcd.print(F("r2 temp SW"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 63:
        if(langFlag == 0){lcd.print(F("r1 hmdt SW"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 64:
        if(langFlag == 0){lcd.print(F("SCRN LGHT"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 65:
        if(langFlag == 0){lcd.print(F("CLOCK SET"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 66:
        if(langFlag == 0){lcd.print(F("Year"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 67:
        if(langFlag == 0){lcd.print(F("Month"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 68:
        if(langFlag == 0){lcd.print(F("Day"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 69:
        if(langFlag == 0){lcd.print(F("Day of Week"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 70:
        if(langFlag == 0){lcd.print(F("Hour"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 71:
        if(langFlag == 0){lcd.print(F("Minute"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 72:
        if(langFlag == 0){lcd.print(F("Second"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 73:
        if(langFlag == 0){lcd.print(F("SAVE TIME"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 74:
        if(langFlag == 0){lcd.print(F("CALIBRATION"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 75:
        if(langFlag == 0){lcd.print(F("temp"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 76:
        if(langFlag == 0){lcd.print(F("hmdt"));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 77:
        if(langFlag == 0){lcd.print(F("LANGUAGE"));}  // ENG
        else if(langFlag == 1){lcd.print(F("9l3blk"));}  // РУС
        break;

        case 78:
        if(langFlag == 0){lcd.print(F("EXIT"));}  // ENG
        else if(langFlag == 1){lcd.print(F("BblXOD"));}  // РУС
        break;

        case 79:
        if(langFlag == 0){lcd.print(F(""));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        case 80:
        if(langFlag == 0){lcd.print(F(""));}  // ENG
        else if(langFlag == 1){lcd.print(F(""));}  // РУС
        break;

        default:
        lcd.print(_ID);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nameAndParentNimeIDlcd(byte _ID, byte chFlag = 0){
    lcd.clear(); // очистить дисплей

    nameIDlcd(masMenu[_ID - 1][1]);
    if(chFlag == 0){lcd.setCursor(1, 1);}
    else{lcd.setCursor(0, 1);}
    

    if(chFlag == 1){lcd.print(F(">"));}
    else if(chFlag == 2){lcd.print(F("*"));}
    
    nameIDlcd(_ID);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte getMenuIndexByID(byte id) {        // Функция получения индекса пункта меню по его ID
    for (byte i = 0; i < punktovMenuVsego; i++) {
        if (masMenu[i][0] == id)  return i;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte getNearMenuIndexByID(byte parentid, byte id, byte side) { // Функция получения индекса пункта меню следующего или предыдущего от актуального
    byte prevID    = 0;   // Переменная для хранения индекса предыдущего элемента
    byte nextID    = 0;   // Переменная для хранения индекса следующего элемента
    byte actualID  = 0;   // Переменная для хранения индекса актуального элемента

    byte firstID   = 0;   // Переменная для хранения индекса первого элемента
    byte lastID    = 0;   // Переменная для хранения индекса последнего элемента

    for (byte i = 0; i < punktovMenuVsego; i++) {
        if (masMenu[i][1] == parentid) {  // Перебираем все элементы с одним родителем
            if (firstID == 0) firstID = i;     // Запоминаем первый элемент списка

            if (masMenu[i][0] == id) {
                actualID = i;                     // Запоминаем актальный элемент списка
            }
            else {
                if (actualID == 0) {             // Если встретился элемент до актуального, делаем его предыдущим
                    prevID = i;
                }
                else if (actualID != 0 && nextID == 0) { // Если встретился элемент после актуального, делаем его следующим
                    nextID = i;
                }
            }
            lastID = i;                         // Каждый последующий элемент - последний
        }
    }

    if (nextID == 0) nextID = firstID;     // Если следующего элемента нет - по кругу выдаем первый
    if (prevID == 0) prevID = lastID;      // Если предыдущего элемента нет - по кругу выдаем последний

    //Serial.println("previusindex:" + (String)prevID + "  nextindex:" + (String)nextID);

    if (side == 2) return prevID ;         // В зависимости от направления вращения, выдаем нужный индекс
    else return nextID;
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setActualMenu(byte v, byte h) {
    if (v != 0) {                                           // Двигаемся по вертикали
        if (v == 2) {                                        // Команда ВВЕРХ (отмена)
            if (isParamEditMode) {                              // Если параметр в режиме редактирования, то отменяем изменения
                isParamEditMode = false;
            }
            else{                                              // Если пункт меню не в режиме редактирования, перемещаемся к родителю

                if (masMenu[actualIndex][1] > 0) {            // Если есть куда перемещаться вверх (ParentID>0)
                    actualIndex = getMenuIndexByID(masMenu[actualIndex][1]);
                }
            }
        }
        else{                                                // Если команда ВНИЗ - входа/редактирования

            if (masMenu[actualIndex][2] && !isParamEditMode) { // Если не в режиме редактирования, то ...
                isParamEditMode = true;                           // Переходим в режим редактирования параметра
                tmpValue = masMenu[actualIndex][3];              // Временной переменной присваиваем актуальное значение параметра
            }
            else if (masMenu[actualIndex][2] && isParamEditMode) { // Если в режиме редактирования
                masMenu[actualIndex][3] = tmpValue;              // Сохраняем заданное значение
                isParamEditMode = false;                          // И выходим из режима редактирования
            }
            else {

                bool nochild = true;                              // Флаг, есть ли дочерние элементы
                for (byte i = 0; i < punktovMenuVsego; i++) {
                    if (masMenu[i][1] == masMenu[actualIndex][0]) {
                        actualIndex = i;                              // Если есть, делаем первый попавшийся актуальным элементом
                        nochild = false;                              // Потомки есть
                        break;                                        // Выходим из for
                    }
                }

                if (nochild) {                     // Если же потомков нет, воспринимаем как команду

                    // lcd
                    nameAndParentNimeIDlcd(masMenu[actualIndex][0], 2);
                    lcd.print(F(" OK"));











                    // COMAND:

                    // И здесь обрабатываем команду по своему усмотрению:
                    if(actualIndex == 78-1){  // EXIT
                        actualIndex = 0;
                    }












                    delay(500);
                }
            }
        }
    }

    if (h != 0) {                                           // Если горизонтальная навигация
        if (isParamEditMode) {                                // В режиме редактирования параметра
            // tmpValue += h;                                      // Изменяем его значение и ...
            if(h == 1){tmpValue ++;}  // Изменяем его значение и ...
            else if(h == 2){tmpValue --;}  // Изменяем его значение и ...
            // ... контроллируем, чтобы оно осталось в заданном диапазоне
            if (tmpValue > masMenu[actualIndex][5]) {
                tmpValue = masMenu[actualIndex][4];
            }

            if (tmpValue < masMenu[actualIndex][4]) {
                tmpValue = masMenu[actualIndex][5];
            }

















            if(actualIndex == 77-1){  //----------------------->>
                langFlag = tmpValue;
            }


























        }
        else {     // Если режим редактирования не активен, навигация среди потомков одного родителя
            actualIndex = getNearMenuIndexByID(masMenu[actualIndex][1], masMenu[actualIndex][0], h);
        }
    }





















    if(actualIndex == 0){
        lcd.clear();
        lcd.setCursor(1,1);
        lcd.print(F("*EXIT OK"));
        delay(500);
        lcd.clear();
        flagMenuState = 0;
        return;
    }




















    // Отображаем информацию в lcd
    if (isParamEditMode) {  // если в режиме редактирования
        lcd.print(F(">"));
        nameAndParentNimeIDlcd(masMenu[actualIndex][0], 1);
        lcd.print(F(": "));
        lcd.print(tmpValue);
    }
    else {
        if (masMenu[actualIndex][2]) {
            nameAndParentNimeIDlcd(masMenu[actualIndex][0]);
            lcd.print(F(": "));
            lcd.print(masMenu[actualIndex][3]);
        }
        else {
            nameAndParentNimeIDlcd(masMenu[actualIndex][0]);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void extMenuSetup(){
    actualIndex = getMenuIndexByID(0);    // Main - актуальный элемент меню
    setActualMenu(0, 0);                  // Выводим в Serial актуальный элемент меню
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void extMenuLoop(){
    bool menuRefresh = 1;
    if(flagMenuState && actualIndex == 0){
        actualIndex = 1;
    }
    while(flagMenuState){
        byte vmenu = 0;  // Переменная хранящая действие по вертикали 1 - вход в меню, 2 - выход из меню
        byte hmenu = 0;             // Переменная хранящая действие по горизонтали 1 - вправо, 2 - влево

        if (millis() - timeButtonDown > timeButtonPressed && isButtonDown) { // Время длительного удержания наступило
            flagButtonLong = true;
        }

        if (flagCW) {               // Шаг вращения по часовой стрелке
            hmenu = 1;
            flagCW = false;           // Действие обработано - сбрасываем флаг
        }

        if (flagCCW) {              // Шаг вращения против часовой стрелки
            hmenu = 2;
            flagCCW = false;          // Действие обработано - сбрасываем флаг
        }

        if (flagButton) {           // Кнопка нажата
            vmenu = 1;                // По нажатию кнопки - переходим на уровень вниз
            flagButton = false;       // Действие обработано - сбрасываем флаг
        }

        if (flagButtonLong && isButtonDown) {   // Кнопка удерживается
            if (!digitalRead(pinButton) && millis() - timeButtonDown > timeButtonPressed) { // Защита от ложного срабатывания
            // ...
                vmenu = 2;             // По удержанию кнопки - возвращаемся на уровень вверх
            }
        
            //================================= Настраиваем реакцию на долгое удержание кнопки =========
            // Для того, чтобы изменить это поведение, измените настройки в блоке ENKODER V
            #if LONG_PRESS_CONTINUES == 0
            // при долгом удержании кнопки, событие long_press повторяется каждые 1,5 секунды
            timeButtonDown = millis(); // Сбрасываем таймер
            longPressReleased = true;  // Флаг удерж. чтоб отсечь обычное нажатие при отпускании кнопки
            #else
            // при долгом удержании кнопки, событие long_press повторялось 1 раз
            isButtonDown = false;                   // Программно "отжимаем" кнопку
            #endif
            //==========================================================================================

            flagButtonLong = false;               // Действие обработано - сбрасываем флаг
        }
        if (vmenu != 0 || hmenu != 0 || menuRefresh) setActualMenu(vmenu, hmenu); // Если было действие - реагируем на него

        menuRefresh = 0;




        if(actualIndex == 63 && isParamEditMode){
            screenBrightnesF(tmpValue);
        }
        else{
            screenBrightnesF(masMenu[63][3]);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////