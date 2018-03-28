// Объявляем переменные
int actualIndex = 0;
bool isParamEditMode  = false;                            // Флаг режима редактирования параметра
int tmpValue          = 0;                                // Временная переменная для хранения изменяемого параметра

struct menu                             // Структура описывающая меню
{
  int id;                               // Идентификационный уникальный индекс ID
  int parentid;                         // ID родителя
  bool isParam;                         // Является ли пункт изменяемым параметром
  String _name;                         // Название
  int value;                            // Актуальное значение
  int _min;                             // Минимально возможное значение параметра
  int _max;                             // Максимально возможное значение параметра
};

int menuArraySize = 22;                 // Задаем размер массива
menu menus[] = {                        // Задаем пункты меню
  {0, -1, false,     "MENU", 0, 0, 0},
  {1, 0, false,          "CBET", 0, 0, 0},
  {2, 1, false,              ":dosvetka", 0, 0, 0},
  {3, 2, true,                    "::1 on chas", 5, 1, 24},
  {4, 2, true,                    "::1 on minut", 30, 0,59 },
  {5, 2, true,                    "::1 off chas", 6, 1, 24},
  {6, 2, true,                    "::1 off minut", 30, 0,59 },
  {7, 2, true,                    "::2 on chas", 20, 1, 24},
  {8, 2, true,                    "::2 on minut", 30, 0,59 },
  {9, 2, true,                    "::2 off chas", 22, 1, 24},
  {10, 2, true,                   "::2 off minut", 30, 0,59 },
  {11, 2, false,                  "::SAVE", 0, 0,0 },
  {12, 1, false,              ":svetocicl", 0, 0, 0},
  {13, 12, true,                  "::on chas", 5, 1, 24},
  {14, 12, true,                  "::on minut", 30, 0,59 },
  {15, 12, true,                  "::off chas", 17, 1, 24},
  {16, 12, true,                  "::off minut", 30, 0,59 },
  {17, 12, false,                 "::SAVE", 0, 0,0 },
  {18, 1, false,              ":viklucheno", 0, 0, 0},
  {19, 1, false,         "POLIV", 0, 0, 0},
  {20, 19, false,             ":ot datchika", 0, 0, 0},
  {21, 20, true,                  "::tip datchika", 1, 0, 2},
  {22, 20, true,                  "::porog analoga", 50, 0, 100},
  {23, 20, false,                 "::SAVE", 0, 0, 0},
  {24, 19, false,             ":po vremeni", 0, 0, 0},
  {25, 24, true,                  "::1 on chas", 5, 1, 24},
  {26, 24, true,                  "::1 on minut", 30, 0,59 },
  {27, 24, true,                  "::2 on chas", 5, 1, 24},
  {28, 24, true,                  "::2 on minut", 30, 0,59 },
  {29, 24, true,                  "::3 on chas", 5, 1, 24},
  {30, 24, true,                  "::3 on minut", 30, 0,59 },
  {31, 24, true,                  "::vrem rabot chas", 5, 1, 24},
  {32, 24, true,                  ":vrem rabot minu", 30, 0,59 },
  {33, 24, false,                 "::SAVE", 0, 0, 0},
  {34, 19, false,             ":ciklicheskiy", 0, 0, 0},
  {35, 19, false,             ":viklucheno", 0, 0, 0},
  {36, 1, false,         "EXIT", 0, 0, 0}
};

int getMenuIndexByID(int id) {        // Функция получения индекса пункта меню по его ID
  for (int i = 0; i < menuArraySize; i++) {
    if (menus[i].id == id)  return i;
  }
  return -1;
}


int getNearMenuIndexByID(int parentid, int id, int side) { // Функция получения индекса пункта меню следующего или предыдущего от актуального
  int prevID    = -1;   // Переменная для хранения индекса предыдущего элемента
  int nextID    = -1;   // Переменная для хранения индекса следующего элемента
  int actualID  = -1;   // Переменная для хранения индекса актуального элемента

  int firstID   = -1;   // Переменная для хранения индекса первого элемента
  int lastID    = -1;   // Переменная для хранения индекса последнего элемента

  for (int i = 0; i < menuArraySize; i++) {
    if (menus[i].parentid == parentid) {  // Перебираем все элементы с одним родителем
      if (firstID == -1) firstID = i;     // Запоминаем первый элемент списка

      if (menus[i].id == id) {
        actualID = i;                     // Запоминаем актальный элемент списка
      }
      else {
        if (actualID == -1) {             // Если встретился элемент до актуального, делаем его предыдущим
          prevID = i;
        }
        else if (actualID != -1 && nextID == -1) { // Если встретился элемент после актуального, делаем его следующим
          nextID = i;
        }
      }
      lastID = i;                         // Каждый последующий элемент - последний
    }
  }

  if (nextID == -1) nextID = firstID;     // Если следующего элемента нет - по кругу выдаем первый
  if (prevID == -1) prevID = lastID;      // Если предыдущего элемента нет - по кругу выдаем последний

  //Serial.println("previusindex:" + (String)prevID + "  nextindex:" + (String)nextID);

  if (side == -1) return prevID ;         // В зависимости от направления вращения, выдаем нужный индекс
  else return nextID;
  return -1;
}

void setActualMenu(int v, int h) {
  if (v != 0) {                                           // Двигаемся по вертикали
    if (v == -1) {                                        // Команда ВВЕРХ (отмена)
      if (isParamEditMode) {                              // Если параметр в режиме редактирования, то отменяем изменения
        isParamEditMode = false;
      }
      else {                                              // Если пункт меню не в режиме редактирования, перемещаемся к родителю
        if (menus[actualIndex].parentid > 0) {            // Если есть куда перемещаться вверх (ParentID>0)
          actualIndex = getMenuIndexByID(menus[actualIndex].parentid);
        }
      }
    }
    else {                                                // Если команда ВНИЗ - входа/редактирования
      if (menus[actualIndex].isParam && !isParamEditMode) { // Если не в режиме редактирования, то ...
        isParamEditMode = true;                           // Переходим в режим редактирования параметра
        tmpValue = menus[actualIndex].value;              // Временной переменной присваиваем актуальное значение параметра
      }
      else if (menus[actualIndex].isParam && isParamEditMode) { // Если в режиме редактирования
        menus[actualIndex].value = tmpValue;              // Сохраняем заданное значение
        isParamEditMode = false;                          // И выходим из режима редактирования

      }
      else {
        bool nochild = true;                              // Флаг, есть ли дочерние элементы
        for (int i = 0; i < menuArraySize; i++) {
          if (menus[i].parentid == menus[actualIndex].id) {
            actualIndex = i;                              // Если есть, делаем первый попавшийся актуальным элементом
            nochild = false;                              // Потомки есть
            break;                                        // Выходим из for
          }
        }
        if (nochild) {                                    // Если же потомков нет, воспринимаем как команду
          Serial.println("Executing command...");         // И здесь обрабатываем по своему усмотрению
        }
      }
    }
  }

  if (h != 0) {                                           // Если горизонтальная навигация
    if (isParamEditMode) {                                // В режиме редактирования параметра
      tmpValue += h;                                      // Изменяем его значение и ...
      // ... контроллируем, чтобы оно осталось в заданном диапазоне
      if (tmpValue > menus[actualIndex]._max) tmpValue = menus[actualIndex]._max;
      if (tmpValue < menus[actualIndex]._min) tmpValue = menus[actualIndex]._min;
    }
    else {                                                // Если режим редактирования не активен, навигация среди потомков одного родителя
      actualIndex = getNearMenuIndexByID(menus[actualIndex].parentid, menus[actualIndex].id, h);
    }
  }

  // Отображаем информацию в Serial
  if (isParamEditMode) {
    Serial.println(" > " + (String)menus[actualIndex]._name + ": " +
                   (String)tmpValue +
                   "  min:" + (String)menus[actualIndex]._min +
                   ", max:" + (String)menus[actualIndex]._max);
  }
  else {
    if (menus[actualIndex].isParam) {
      Serial.println((String)menus[actualIndex]._name + ": " + (String)menus[actualIndex].value);
    }
    else {
      Serial.println((String)menus[actualIndex]._name);
    }
  }
}





void extMenuSetup()
{
  actualIndex = getMenuIndexByID(0);    // Main - актуальный элемент меню

  setActualMenu(0, 0);                  // Выводим в Serial актуальный элемент меню
}

void extMenuLoop()
{
  int vmenu = 0;                        // Переменная хранящая действие по вертикали 1 - вход в меню, -1 - выход из меню
  int hmenu = 0;                        // Переменная хранящая действие по горизонтали 1 - вправо, -1 - влево

  if (millis() - timeButtonDown > timeButtonPressed && isButtonDown) { // Время длительного удержания наступило
    flagButtonLong = true;
  }

  if (flagCW) {               // Шаг вращения по часовой стрелке
    hmenu = 1;
    //Serial.println("right");
    flagCW = false;           // Действие обработано - сбрасываем флаг
  }
  if (flagCCW) {              // Шаг вращения против часовой стрелки
    hmenu = -1;
    //Serial.println("left");
    flagCCW = false;          // Действие обработано - сбрасываем флаг
  }
  if (flagButton) {           // Кнопка нажата
    vmenu = 1;                // По нажатию кнопки - переходим на уровень вниз
    //Serial.println("button");
    flagButton = false;       // Действие обработано - сбрасываем флаг
  }
  if (flagButtonLong && isButtonDown) {   // Кнопка удерживается
    if (!digitalRead(pinButton) && millis() - timeButtonDown > timeButtonPressed) { // Защита от ложного срабатывания
      // ...
      vmenu = -1;             // По удержанию кнопки - возвращаемся на уровень вверх
      //Serial.println("long_press");
    }
    
    //=========================================== Настраиваем реакцию на долгое удержание кнопки ===============================================
    // Чтобы событие long_press во время удержания срботало только один раз, необходимо раскомментировать блок и закомментировать следующий
    //isButtonDown = false;                 // Программно "отжимаем" кнопку

    // Эти две строки отвечают за то, чтобы при долгом удержании кнопки, событие long_press повторялось каждые 1,5 секунды
    // Для того, чтобы изменить это поведение нужно закомментировать две эти строки и раскомментировать строку из предыдущего блока
    timeButtonDown = millis();            // Сбрасываем таймер
    longPressReleased = true;             // Флаг срабатывания долгого удержания, чтобы отсечь генерацию обычного нажатия при отпускании кнопки
    //==========================================================================================================================================

    flagButtonLong = false;               // Действие обработано - сбрасываем флаг
  }
  if (vmenu != 0 || hmenu != 0) setActualMenu(vmenu, hmenu); // Если было действие - реагируем на него

}
