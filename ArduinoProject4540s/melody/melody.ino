// МЕЛОДИЯ – массив нот и массив длительностей
char melody[]={'G','G','G','E','H',
'G','E','H','G','*',
'd','d','d','e','H',
'T','E','H','F',
'g','G','G','g','t','e',
's','s','s','*','U','r','c','B',
'H','A','H','*','E','T','E','F',
'H','G','H','d',
'g','G','G','g','t','f',
's','s','s','*','U','r','c','B',
'H','A','H','*','E','T','E','H',
'G','E','H','G',
'%','%'};
int bb[]={8,8,8,6,2,
8,6,2,8,8,
8,8,8,6,2,
8,6,2,16,
8,6,2,8,6,2,
2,2,4,4,2,8,6,2,
2,2,4,4,2,8,6,2,
8,6,2,16,
8,6,2,8,6,2,
2,2,4,4,2,8,6,2,
2,2,4,4,2,8,6,2,
8,6,2,16,
64,64};
// подключить динамик к pin 8
int speakerPin = 8;
// темп воспроизведения, ноты, длительности
int tempo,notes,beats;
// процедура проигрыша ноты
void playNote(char note, int duration)
{
// массив для наименований нот в пределах двух октав
char names[]={'c','r','d','s','e','f','t','g','u','a','b',
'h','C','R','D','S','E','F','T','G','U','A','B', 'H','F'};
// массив частот нот
int tones[]={261,277,293,311,329,349,370,392,415,440,466,
494, 523,554,587,622,659,698,740,784,830,880,932,988};
// проиграть тон, соответствующий ноте
for (int i = 0; i < sizeof(tones); i++)
{
if (names[i] == note)
{
tone(speakerPin,tones[i],duration);
}
}
}
void setup()
{
pinMode(speakerPin, OUTPUT);
tempo=50; // скорость воспроизведения мелодии
}
void loop()
{
for(int i=0;i<sizeof(melody);i++)
{
notes=melody[i];
beats=bb[i];
if (notes == '*')
tone(speakerPin,0, beats*tempo); // пауза
else
playNote(notes, beats*tempo);
// пауза между нотами
delay(beats*tempo+tempo);
}
}
