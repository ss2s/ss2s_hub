// Объявляем переменные
byte actualIndex = 0;
bool isParamEditMode  = false;            // Флаг режима редактирования параметра
int tmpValue          = 0;                // Временная переменная для хранения изменяемого параметра


// ОПИСАНИЕ МАСИВА С МЕНЮ:

// 0 Идентификационный уникальный индекс ID
// 1 ID родителя
// 2 Является ли пункт изменяемым параметром
// 3 Актуальное значение
// 4 Минимально возможное значение параметра
// 5 Максимально возможное значение параметра

// ПОЗИЦИЯ ID --;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte punktovMenuVsego = 77;

byte masMenu[][6] = {
    {1,0,0,0,0,0},     // МЕНЮ (ВОЙТИ)


    {2,1,0,0,0,0},             // Р4 СВЕТ 

    {3,2,1,1,0,2},                     // ВЫБОР РЕЖИМА - 2 : 1 : 0

    {4,2,0,0,0,0},                     // 2 РЕЖИМ ДОСВЕТКА (НАСТРОЙКА РЕЖИМА 2)

    {5,4,1,5,1,24},                            // 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    {6,4,1,30,0,59},                           // 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    {7,4,1,6,1,24},                            // 1 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 1 h 
    {8,4,1,30,0,59},                           // 1 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 1 m
    {9,4,1,20,1,24},                           // 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    {10,4,1,30,0,59},                          // 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    {11,4,1,22,1,24},                          // 2 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 2 h
    {12,4,1,30,0,59},                          // 2 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 2 m

    {13,2,0,0,0,0},                    // 1 РЕЖИМ СВЕТОЦИКЛ (НАСТРОЙКА РЕЖИМА 1)

    {14,13,1,5,1,24},                          // ВКЛЮЧЕНИЕ В ЧАСОВ                ON h
    {15,13,1,30,0,59},                         // ВКЛЮЧЕНИЕ В МИНУТ                ON m
    {16,13,1,17,1,24},                         // ВЫКЛЮЧЕНИЕ В ЧАСОВ              OFF h
    {17,13,1,30,0,59},                         // ВЫКЛЮЧЕНИЕ В МИНУТ              OFF m



    {18,1,0,0,0,0},            // Р3 ПОЛИВ

    {19,18,1,2,0,3},                   // ВЫБОР РЕЖИМА - 3 : 2 : 1 : 0

    {20,18,0,0,0,0},                   // 3 ОТ ДАТЧИКА ПОЧВЫ (НАСТРОЙКА РЕЖИМА)

    {21,20,1,1,0,2},                           // ТИП ДАТЧИКА                     0 1 2
    {22,20,1,50,0,100},                        // ПОРОГ АНАЛОГОВОГО ДАТЧИКА  если тип 2

    {23,18,0,0,0,0},                   // 2 ПО ВРЕМЕНИ (НАСТРОЙКА РЕЖИМА)

    {24,23,1,5,1,24},                          // 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    {25,23,1,15,0,59},                         // 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    {26,23,1,12,1,24},                         // 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    {27,23,1,15,0,59},                         // 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    {28,23,1,18,1,24},                         // 3 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 3 h
    {29,23,1,15,0,59},                         // 3 ВКЛЮЧЕНИЕ В МИНУТ            ON 3 m
    {30,23,1,0,0,24},                          // ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
    {31,23,1,10,0,59},                         // ВРЕМЯ РАБОТЫ МИНУТ         POWER ON m

    {32,18,0,0,0,0},                   // 1 ЦИКЛИЧЕСКИЙ (НАСТРОЙКА РЕЖИМА)

    {33,32,1,4,0,24},                          // МЕЖДУ ПОЛИВАМИ ЧАСОВ          DELAY h
    {34,32,1,0,0,59},                          // МЕЖДУ ПОЛИВАМИ МИНУТ          DELAY m
    {35,32,1,0,0,24},                          // ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
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

    {64,1,1,50,0,100},        // ЯРКОСТЬ ДИСПЛЕЯ

    {65,1,0,0,0,0},            // НАСТРОЙКА ЧАСОВ

    {66,65,1,18,0,99},                  // ГОД
    {67,65,1,3,1,12},                   // МЕСЯЦ
    {68,65,1,22,1,31},                  // ДЕНЬ
    {69,65,1,4,1,7},                    // ДЕНЬ НЕДЕЛИ
    {70,65,1,12,1,23},                  // ЧАСЫ
    {71,65,1,30,0,59},                  // МИНУТЫ
    {72,65,1,0,0,59},                   // СЕКУНДЫ
    {73,65,0,0,0,0},                    // СОХРАНИТЬ                                SAVE

    {74,1,0,0,0,0},             // КАЛИБРОВКА ТЕМПЕРАТУРЫ И ВЛАЖНОСТИ

    {75,74,1,1,0,100},                   // ТЕМПЕРАТУРА
    {76,74,1,27,0,100},                  // ВЛАЖНОСТЬ

    {77,1,0,0,0,0}             // EXIT
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nameIDserial(byte _ID){
    Serial.print(_ID);
    Serial.print(". ");
    switch (_ID) {

        case 1:
        Serial.print(F("MENU"));
        break;

        case 2:
        Serial.print(F("R4 LIGHT"));
        break;

        default:
        Serial.print(_ID);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void nameIDlcd(byte _ID){
    switch (_ID) {

        case 0:
        lcd.print(F("0 menu"));
        break;

        case 1:
        lcd.print(F("MENU"));
        break;

        case 2:
        lcd.print(F("R4 LIGHT set"));
        break;

        case 3:
        lcd.print(F("R4 mode"));
        break;

        case 4:
        lcd.print(F("m2 DOSVET"));
        break;

        case 5:
        lcd.print(F("1 on H"));
        break;

        case 6:
        lcd.print(F("1 on M"));
        break;

        case 7:
        lcd.print(F("1 off H"));
        break;

        case 8:
        lcd.print(F("1 off M"));
        break;

        case 9:
        lcd.print(F("2 on H"));
        break;

        case 10:
        lcd.print(F("2 on M"));
        break;

        case 11:
        lcd.print(F("2 off H"));
        break;

        case 12:
        lcd.print(F("2 off M"));
        break;

        case 13:
        lcd.print(F("m1 LIGHTCYCLE"));
        break;

        case 14:
        lcd.print(F("on Hour"));
        break;

        case 15:
        lcd.print(F("on Minute"));
        break;

        case 16:
        lcd.print(F("off Hour"));
        break;

        case 17:
        lcd.print(F("off Minute"));
        break;

        case 18:
        lcd.print(F("R3 WATER set"));
        break;

        case 19:
        lcd.print(F("R3 mode"));
        break;

        case 20:
        lcd.print(F("m3 SOIL SENSOR"));
        break;

        case 21:
        lcd.print(F("sensor type"));
        break;

        case 22:
        lcd.print(F("thrshld"));
        break;

        case 23:
        lcd.print(F("m2 ON TIME"));
        break;

        case 24:
        lcd.print(F("1 on H"));
        break;

        case 25:
        lcd.print(F("1 on M"));
        break;

        case 26:
        lcd.print(F("2 on H"));
        break;

        case 27:
        lcd.print(F("2 on M"));
        break;

        case 28:
        lcd.print(F("3 on H"));
        break;

        case 29:
        lcd.print(F("3 on M"));
        break;

        case 30:
        lcd.print(F("wrk time H"));
        break;

        case 31:
        lcd.print(F("wrk time M"));
        break;

        case 32:
        lcd.print(F("m1 CYCLE"));
        break;

        case 33:
        lcd.print(F("btw time H"));
        break;

        case 34:
        lcd.print(F("btw time M"));
        break;

        case 35:
        lcd.print(F("wrk time H"));
        break;

        case 36:
        lcd.print(F("wrk time M"));
        break;

        case 37:
        lcd.print(F("R2 TEMP set"));
        break;

        case 38:
        lcd.print(F("R2 mode"));
        break;

        case 39:
        lcd.print(F("m3 CUSTOMIZE"));
        break;

        case 40:
        lcd.print(F("on temp"));
        break;

        case 41:
        lcd.print(F("off temp"));
        break;

        case 42:
        lcd.print(F("m2 HEAT"));
        break;

        case 43:
        lcd.print(F("targt temp"));
        break;

        case 44:
        lcd.print(F("hysteresis"));
        break;

        case 45:
        lcd.print(F("m1 COOLING"));
        break;

        case 46:
        lcd.print(F("targt temp"));
        break;

        case 47:
        lcd.print(F("hysteresis"));
        break;

        case 48:
        lcd.print(F("R1 HUMIDTY set"));
        break;

        case 49:
        lcd.print(F("R1 mode"));
        break;

        case 50:
        lcd.print(F("m3 CUSTOMIZE"));
        break;

        case 51:
        lcd.print(F("on hmdt"));
        break;

        case 52:
        lcd.print(F("off hmdt"));
        break;

        case 53:
        lcd.print(F("m2 MOISTURIZNG"));
        break;

        case 54:
        lcd.print(F("targt hmdt"));
        break;

        case 55:
        lcd.print(F("hysteresis"));
        break;

        case 56:
        lcd.print(F("m1 DRYING"));
        break;

        case 57:
        lcd.print(F("targt hmdt"));
        break;

        case 58:
        lcd.print(F("hysteresis"));
        break;

        case 59:
        lcd.print(F("MANUAL CONTROL"));
        break;

        case 60:
        lcd.print(F("r4 lght SW"));
        break;

        case 61:
        lcd.print(F("r3 wtrg SW"));
        break;

        case 62:
        lcd.print(F("r2 temp SW"));
        break;

        case 63:
        lcd.print(F("r1 hmdt SW"));
        break;

        case 64:
        lcd.print(F("SCRN LGHT"));
        break;

        case 65:
        lcd.print(F("CLOCK SET"));
        break;

        case 66:
        lcd.print(F("Year"));
        break;

        case 67:
        lcd.print(F("Month"));
        break;

        case 68:
        lcd.print(F("Day"));
        break;

        case 69:
        lcd.print(F("Day of Week"));
        break;

        case 70:
        lcd.print(F("Hour"));
        break;

        case 71:
        lcd.print(F("Minute"));
        break;

        case 72:
        lcd.print(F("Second"));
        break;

        case 73:
        lcd.print(F("SAVE TIME"));
        break;

        case 74:
        lcd.print(F("CALIBRATION"));
        break;

        case 75:
        lcd.print(F("temp"));
        break;

        case 76:
        lcd.print(F("hmdt"));
        break;

        case 77:
        lcd.print(F("EXIT"));
        break;

        case 78:
        lcd.print(F(""));
        break;

        case 79:
        lcd.print(F(""));
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
                    // serial
                    Serial.print(F("Executing command... "));
                    nameIDserial(masMenu[actualIndex][0]);
                    Serial.println(F(" OK"));
                    // lcd
                    nameAndParentNimeIDlcd(masMenu[actualIndex][0], 2);
                    lcd.print(F(" OK"));

                    // И здесь обрабатываем команду по своему усмотрению:

                    delay(1000);
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
                tmpValue = masMenu[actualIndex][5];
            }

            if (tmpValue < masMenu[actualIndex][4]) {
                tmpValue = masMenu[actualIndex][4];
            }
        }
        else {     // Если режим редактирования не активен, навигация среди потомков одного родителя
            actualIndex = getNearMenuIndexByID(masMenu[actualIndex][1], masMenu[actualIndex][0], h);
        }
    }

  // Отображаем информацию в Serial
    if (isParamEditMode) {  // если в режиме редактирования
        Serial.print(F(" > "));
        nameIDserial(masMenu[actualIndex][0]);
        Serial.print(F(": "));
        Serial.print(tmpValue);
        Serial.print(F("  "));
        Serial.print(masMenu[actualIndex][4]);
        Serial.print(F("-"));
        Serial.println(masMenu[actualIndex][5]);
    }
    else {
        if (masMenu[actualIndex][2]) {
            nameIDserial(masMenu[actualIndex][0]);
            Serial.print(F(": "));
            Serial.println(masMenu[actualIndex][3]);
        }
        else {
            nameIDserial(masMenu[actualIndex][0]);
            Serial.println();
        }
    }

    if(actualIndex == 0){
        lcd.clear();
        lcd.print(F("EXIT OK"));
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
    byte vmenu = 0;  // Переменная хранящая действие по вертикали 1 - вход в меню, 2 - выход из меню
    byte hmenu = 0;             // Переменная хранящая действие по горизонтали 1 - вправо, 2 - влево

    if (millis() - timeButtonDown > timeButtonPressed && isButtonDown) { // Время длительного удержания наступило
        flagButtonLong = true;
    }

    if (flagCW) {               // Шаг вращения по часовой стрелке
        hmenu = 1;
        // lcd.clear(); // очистить дисплей
        // lcd.println("right");
        //Serial.println("right");
        flagCW = false;           // Действие обработано - сбрасываем флаг
    }

    if (flagCCW) {              // Шаг вращения против часовой стрелки
        hmenu = 2;
        // lcd.clear(); // очистить дисплей
        // lcd.println("left");
        //Serial.println("left");
        flagCCW = false;          // Действие обработано - сбрасываем флаг
    }

    if (flagButton) {           // Кнопка нажата
        vmenu = 1;                // По нажатию кнопки - переходим на уровень вниз
        // lcd.clear(); // очистить дисплей
        // lcd.println("button");
        //Serial.println("button");
        flagButton = false;       // Действие обработано - сбрасываем флаг
    }

    if (flagButtonLong && isButtonDown) {   // Кнопка удерживается
        if (!digitalRead(pinButton) && millis() - timeButtonDown > timeButtonPressed) { // Защита от ложного срабатывания
        // ...
            vmenu = 2;             // По удержанию кнопки - возвращаемся на уровень вверх
            // lcd.clear(); // очистить дисплей
            // lcd.println("long_press");
            //Serial.println("long_press");
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
    if (vmenu != 0 || hmenu != 0) setActualMenu(vmenu, hmenu); // Если было действие - реагируем на него
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////