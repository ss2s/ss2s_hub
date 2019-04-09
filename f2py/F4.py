# coding: utf8
# f4
import codecs
#import qt
symb = ['►','☼','◙','↑','↓','→','←']
beep = "♫•" # alt+7
bel = "\a"

masMenu = [
# 0 Идентификационный уникальный индекс ID
# 1 ID родителя
# 2 Является ли пункт изменяемым параметром
# 3 Актуальное значение
# 4 Минимально возможное значение параметра
# 5 Максимально возможное значение параметра
    [1,0,0,0,0,0],     # МЕНЮ (ВОЙТИ)

    [2,1,0,0,0,0],             # Р4 СВЕТ (ВЫБОР РЕЖИМА)

    [3,2,0,0,0,0],                     # 2 РЕЖИМ ДОСВЕТКА (НАСТРОЙКА РЕЖИМА 2 1 0)

    [4,3,1,5,1,24],                            # 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    [5,3,1,30,0,59],                           # 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    [6,3,1,6,1,24],                            # 1 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 1 h 
    [7,3,1,30,0,59],                           # 1 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 1 m
    [8,3,1,20,1,24],                           # 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    [9,3,1,30,0,59],                           # 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    [10,3,1,22,1,24],                          # 2 ВЫКЛЮЧЕНИЕ В ЧАСОВ          OFF 2 h
    [11,3,1,30,0,59],                          # 2 ВЫКЛЮЧЕНИЕ В МИНУТ          OFF 2 m
    [12,3,0,0,0,0],                            # СОХРАНИТЬ                        SAVE

    [13,2,0,0,0,0],                    # 1 РЕЖИМ СВЕТОЦИКЛ (НАСТРОЙКА РЕЖИМА)

    [14,13,1,5,1,24],                          # ВКЛЮЧЕНИЕ В ЧАСОВ                ON h
    [15,13,1,30,0,59],                         # ВКЛЮЧЕНИЕ В МИНУТ                ON m
    [16,13,1,17,1,24],                         # ВЫКЛЮЧЕНИЕ В ЧАСОВ              OFF h
    [17,13,1,30,0,59],                         # ВЫКЛЮЧЕНИЕ В МИНУТ              OFF m
    [18,13,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [19,2,0,0,0,0],                    # 0 ОТКЛЮЧЕНО, РУЧНОЕ УПРАВЛЕНИЕ            OFF

    [20,1,0,0,0,0],            # Р3 ПОЛИВ (ВЫБОР РЕЖИМА 3 2 1 0)

    [21,20,0,0,0,0],                   # 3 ОТ ДАТЧИКА ПОЧВЫ (НАСТРОЙКА РЕЖИМА)

    [22,21,1,1,0,2],                           # ТИП ДАТЧИКА                     0 1 2
    [23,21,1,50,0,100],                        # ПОРОГ АНАЛОГОВОГО ДАТЧИКА  если тип 2
    [24,21,0,0,0,0],                           # СОХРАНИТЬ

    [25,20,0,0,0,0],                   # 2 ПО ВРЕМЕНИ (НАСТРОЙКА РЕЖИМА)

    [26,25,1,5,1,24],                          # 1 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 1 h
    [27,25,1,15,0,59],                         # 1 ВКЛЮЧЕНИЕ В МИНУТ            ON 1 m
    [28,25,1,12,1,24],                         # 2 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 2 h
    [29,25,1,15,0,59],                         # 2 ВКЛЮЧЕНИЕ В МИНУТ            ON 2 m
    [30,25,1,18,1,24],                         # 3 ВКЛЮЧЕНИЕ В ЧАСОВ            ON 3 h
    [31,25,1,15,0,59],                         # 3 ВКЛЮЧЕНИЕ В МИНУТ            ON 3 m
    [32,25,1,0,0,24],                          # ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
    [33,25,1,10,0,59],                         # ВРЕМЯ РАБОТЫ МИНУТ         POWER ON m
    [34,25,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [35,20,0,0,0,0],                   # 1 ЦИКЛИЧЕСКИЙ (НАСТРОЙКА РЕЖИМА)

    [36,35,1,4,0,24],                          # МЕЖДУ ПОЛИВАМИ ЧАСОВ          DELAY h
    [37,35,1,0,0,59],                          # МЕЖДУ ПОЛИВАМИ МИНУТ          DELAY m
    [38,35,1,0,0,24],                          # ВРЕМЯ РАБОТЫ ЧАСОВ         POWER ON h
    [39,35,1,10,0,59],                         # ВРЕМЯ РАБОТЫ МИНУТ         POWER ON m
    [40,35,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [41,20,0,0,0,0],                   # 0 ОТКЛЮЧЕНО, РУЧНОЕ УПРАВЛЕНИЕ            OFF

    [42,1,0,0,0,0],            # Р2 ТЕМПЕРАТУРА (ВЫБОР РЕЖИМА 3 2 1 0)

    [43,42,0,0,0,0],                   # 3 НАСТРАИВАЕМЫЙ (НАСТРОЙКА РЕЖИМА)

    [44,43,1,25,0,50],                         # ТЕМПЕРАТУРА ВКЛЮЧЕНИЯ              ON
    [45,43,1,23,0,50],                         # ТЕМПЕРАТУРА ВЫКЛЮЧЕНИЯ            OFF
    [46,43,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [47,42,0,0,0,0],                   # 2 НАГРЕВ (НАСТРОЙКА РЕЖИМА)

    [48,47,1,25,0,50],                         # ЖЕЛАЕМАЯ ТЕМПЕРАТУРА              VAL
    [49,47,1,2,0,10],                          # ГИСТЕРЕЗИС                        GIS
    [50,47,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [51,42,0,0,0,0],                   # 1 ОХЛАЖДЕНИЕ (НАСТРОЙКА РЕЖИМА)

    [52,51,1,20,0,50],                         # ЖЕЛАЕМАЯ ТЕМПЕРАТУРА              VAL
    [53,51,1,1,0,10],                          # ГИСТЕРЕЗИС                        GIS
    [54,51,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [55,42,0,0,0,0],                   # 0 ОТКЛЮЧЕНО, РУЧНОЕ УПРАВЛЕНИЕ            OFF


    [56,1,0,0,0,0],            # Р1 ВЛАЖНОСТЬ (ВЫБОР РЕЖИМА 3 2 1 0)

    [57,56,0,0,0,0],                   # 3 НАСТРАИВАЕМЫЙ (НАСТРОЙКА РЕЖИМА)

    [58,57,1,60,20,95],                        # ВЛАЖНОСТЬ ВКЛЮЧЕНИЯ                ON
    [59,57,1,80,20,95],                        # ВЛАЖНОСТЬ ВЫКЛЮЧЕНИЯ              OFF
    [60,57,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [61,56,0,0,0,0],                   # 2 УВЛАЖНЕНИЕ (НАСТРОЙКА РЕЖИМА)

    [62,61,1,80,20,95],                        # ЖЕЛАЕМАЯ ВЛАЖНОСТЬ                VAL
    [63,61,1,5,0,10],                          # ГИСТЕРЕЗИС                        GIS
    [64,61,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [65,56,0,0,0,0],                   # 1 СУШКА (НАСТРОЙКА РЕЖИМА)

    [66,65,1,60,20,95],                        # ЖЕЛАЕМАЯ ВЛАЖНОСТЬ                VAL
    [67,65,1,5,0,10],                          # ГИСТЕРЕЗИС                        GIS
    [68,65,0,0,0,0],                           # СОХРАНИТЬ                        SAVE

    [69,56,0,0,0,0],                   # 0 ОТКЛЮЧЕНО, РУЧНОЕ УПРАВЛЕНИЕ            OFF

    [70,1,0,0,0,0],            # РУЧНОЕ УПРАВЛЕНИЕ (ВХОД В УПРАВЛЕНИЕ)

    [71,70,0,0,0,0],                   # Р4 СВЕТ                                ON/OFF
    [72,70,0,0,0,0],                   # Р3 ПОЛИВ                               ON/OFF
    [73,70,0,0,0,0],                   # Р2 ТЕМПЕРАТУРА                         ON/OFF
    [74,70,0,0,0,0],                   # Р1 ВЛАЖНОСТЬ                           ON/OFF

    [75,1,1,200,0,255],        # ЯРКОСТЬ ДИСПЛЕЯ

    [76,1,0,0,0,0],            # НАСТРОЙКА ЧАСОВ

    [77,76,1,18,0,99],                  # ГОД
    [78,76,1,3,1,12],                   # МЕСЯЦ
    [79,76,1,22,1,31],                  # ДЕНЬ
    [80,76,1,4,1,7],                    # ДЕНЬ НЕДЕЛИ
    [81,76,1,12,1,23],                  # ЧАСЫ
    [82,76,1,30,0,59],                  # МИНУТЫ
    [83,76,1,0,0,59],                   # СЕКУНДЫ
    [84,76,0,0,0,0],                    # СОХРАНИТЬ                                SAVE

    [85,1,0,0,0,0]             # EXIT
]

names0 = ["Й", "Ц", "У", "К", "Е", "Н", "Г", "Ш", "З", "Х", "Ы", "В", "А", "П", "Р", "О", "Л", "Д", "Ж", "Я", "Ч", "С", "М", "И", "Т", "Ь", "Б", "Ю"]

namesMenu0 = ["МЕНЮ", "СВЕТ", "ПОЛИВ", "ТЕМПЕРАТУРА", "ВЛАЖНОСТЬ", "РУЧНОЕ УПРАВЛЕНИЕ", "ЯРКОСТЬ ДИСПЛЕЯ", "НАСТРОЙКА ЧАСОВ"]
namesMenu00 = ["РЕЖИМ ", "ДОСВЕТКА", "СВЕТОЦИКЛ", "ВКЛЮЧЕНИЕ В ", "ВЫКЛЮЧЕНИЕ В ", "ЧАСОВ", "МИНУТ"]
namesMenu01 = ["ОТ ДАТЧИКА", "ПО ВРЕМЕНИ", "ЦИКЛИЧЕСКИЙ", "ТИП ДАТЧИКА", "ПОРОГ АНАЛОГА", "ВРЕМЯ РАБОТЫ НАСОСА", "МЕЖДУ ПОЛИВАМИ"]
namesMenu02 = ["НАСТРАИВАЕМЫЙ", "НАГРЕВ", "ОХЛАЖДЕНИЕ", "ЖЕЛАЕМАЯ", "ГИСТЕРЕЗИС", "ВКЛЮЧЕНИЯ", "ВЫКЛЮЧЕНИЯ"]
namesMenu03 = ["УВЛАЖНЕНИЕ", "СУШКА"]
namesMenu04 = ["0 ", "1 ", "2 ", "3 ", "4", "РУЧНОЕ УПРАВЛЕНИЕ"]
namesMenu05 = ["ЯРКОСТЬ"]
namesMenu06 = ["ГОД", "МЕСЯЦ", "ДЕНЬ", "ДЕНЬ НЕДЕЛИ", "ЧАСЫ", "МИНУТЫ", "СЕКУНДЫ",]
namesMenu07 = ["СОХРАНИТЬ:", "ОКЛ. РУЧНОЕ УПРАВЛЕНИЕ", "EXIT", "    "]

def indexToLetterName(index):
	pass

def indexToName(index):
	name = ""

	if index == 0:
		return
	elif index==1:  # МЕНЮ (ВОЙТИ)
		name = namesMenu0[0]
	elif index==2:
		name = namesMenu07[3]+namesMenu0[1]
	elif index==3:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu00[0]+namesMenu00[1]
	elif index==4:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[1]+namesMenu00[3]+namesMenu00[5]
	elif index==5:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[1]+namesMenu00[3]+namesMenu00[6]
	elif index==6:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[1]+namesMenu00[4]+namesMenu00[5]
	elif index==7:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[1]+namesMenu00[4]+namesMenu00[6]
	elif index==8:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[2]+namesMenu00[3]+namesMenu00[5]
	elif index==9:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[2]+namesMenu00[3]+namesMenu00[6]
	elif index==10:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[2]+namesMenu00[4]+namesMenu00[5]
	elif index==11:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu04[2]+namesMenu00[4]+namesMenu00[6]
	elif index==12:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==13:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu00[0]+namesMenu00[2]
	elif index==14:
		name = ""
	elif index==15:
		name = ""
	elif index==16:
		name = ""
	elif index==17:
		name = ""
	elif index==18:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==19:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[1]
	elif index==20:
		name = namesMenu07[3]+namesMenu0[2]
	elif index==21:
		name = ""
	elif index==22:
		name = ""
	elif index==23:
		name = ""
	elif index==24:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==25:
		name = ""
	elif index==26:
		name = ""
	elif index==27:
		name = ""
	elif index==28:
		name = ""
	elif index==29:
		name = ""
	elif index==30:
		name = ""
	elif index==31:
		name = ""
	elif index==32:
		name = ""
	elif index==33:
		name = ""
	elif index==34:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==35:
		name = ""
	elif index==36:
		name = ""
	elif index==37:
		name = ""
	elif index==38:
		name = ""
	elif index==39:
		name = ""
	elif index==40:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==41:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[1]
	elif index==42:
		name = namesMenu07[3]+namesMenu0[3]
	elif index==43:
		name = ""
	elif index==44:
		name = ""
	elif index==45:
		name = ""
	elif index==46:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==47:
		name = ""
	elif index==48:
		name = ""
	elif index==49:
		name = ""
	elif index==50:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==51:
		name = ""
	elif index==52:
		name = ""
	elif index==53:
		name = ""
	elif index==54:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==55:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[1]
	elif index==56:
		name = namesMenu07[3]+namesMenu0[4]
	elif index==57:
		name = ""
	elif index==58:
		name = ""
	elif index==59:
		name = ""
	elif index==60:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==61:
		name = ""
	elif index==62:
		name = ""
	elif index==63:
		name = ""
	elif index==64:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==65:
		name = ""
	elif index==66:
		name = ""
	elif index==67:
		name = ""
	elif index==68:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==69:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[1]
	elif index==70:
		name = namesMenu07[3]+namesMenu0[5]
	elif index==71:
		name = ""
	elif index==72:
		name = ""
	elif index==73:
		name = ""
	elif index==74:
		name = ""
	elif index==75:
		name = namesMenu07[3]+namesMenu0[6]
	elif index==76:
		name = namesMenu07[3]+namesMenu0[7]
	elif index==77:
		name = ""
	elif index==78:
		name = ""
	elif index==79:
		name = ""
	elif index==80:
		name = ""
	elif index==81:
		name = ""
	elif index==82:
		name = ""
	elif index==83:
		name = ""
	elif index==84:
		name = namesMenu07[3]+namesMenu07[3]+namesMenu07[0]
	elif index==85:
		name = namesMenu07[3]+namesMenu07[2]

	print(name)
	return name

print()

for i in range(86):
	indexToName(i)

for i in range(86):
	indexToLetterName(i)

print()
input("press Enter or Escape to exit:")