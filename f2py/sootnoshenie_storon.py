# _*_ coding utf-8 _*_

print()
print()
print("введите соотношение сторон")
print()
horizontalVal_1 = input("ширина: ")
print()
verticalVal_1 = input("высота: ")
delitel = float(horizontalVal_1) / float(verticalVal_1)
print()
print("= ", delitel)
print()
print( horizontalVal_1,'/', verticalVal_1)
print()
print("изображение которое нужно обрезать")
print()
horizontalVal_2 = input("ширина: ")
verticalVal_2 = float(horizontalVal_2) / delitel
print()
print("высота = " + str(verticalVal_2))
print()

if verticalVal_2 > int(verticalVal_2):
    print( horizontalVal_2,'/', verticalVal_2)
    print()
    print('>')
    horizontalVal_2 = int( horizontalVal_2 )
    while verticalVal_2 > int(verticalVal_2):
        horizontalVal_2 -= 1
        verticalVal_2 = horizontalVal_2 / delitel
        print( horizontalVal_2,'/', verticalVal_2)

    print('>')
    print()
    print( horizontalVal_2,'/', int(verticalVal_2))
else:
    print( horizontalVal_2,'/', int(verticalVal_2))

print()
print()
print()
input("press Enter to exit")