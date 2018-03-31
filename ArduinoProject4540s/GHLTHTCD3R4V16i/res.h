// Начало функции обработки кириллических символов при выводе на монитор серийного порта
String utf8rus(String source){     // Функция для конвертации русских символов из двубайтовой кодировки в однобайтовую

    int i,k;
    String target;
    unsigned char n;
    char m[2] = { '0', '\0' };
    k = source.length(); i = 0;
    while (i < k) {
    n = source[i]; i++;
 
    if (n >= 0xBF){
        switch (n) {
            case 0xD0: {
                n = source[i]; i++;
                if (n == 0x81) { n = 0xA8; break; }
                if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
                break;
            }
            case 0xD1: {
                n = source[i]; i++;
                if (n == 0x91) { 
                    n = 0xB8; break; 
                }
                    if (n >= 0x80 && n <= 0x8F) {
                        n = n + 0x70;
                    }
                break;
            }
        }
    }
    m[0] = n; target = target + String(m); 
    }
    return target;
}
// Конец функции обработки кириллических симоволов

int ascii_cod(char x)
{
int a;
a = x;
return a;
}

int ascii_cod2(char x)
{
(int)x;
return x;
}
