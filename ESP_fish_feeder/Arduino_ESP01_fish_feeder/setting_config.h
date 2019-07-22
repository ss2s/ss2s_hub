#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// НАСТРОЙКИ СИСТЕМЫ:

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// auth setting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FEEDER_INDEX_NUMBER 1  // номер кормушки

// // введите Auth Token для выбранной платы, у каждой платы свой токен, он отправляется на e-mail из blynk
// char auth[] = "YourAuthToken";  // Auth Token, токен аунтефикации

// // введите имя "SSID" точки доступа и пароль
// char ssid[] = "YourNetworkName";  // имя точки доступа "SSID"
// char pass[] = "YourPassword";  // пароль точки доступа

// char e_mail_addr[] = "YourMail@example.com";  // почта для уведомления пустой бункер

char auth[] = "75a9ab7e2fcb44df8648220eb68a07a2";  // MEGA 1
// char auth[] = "26f4917526a541d7a551fb8aa12c44af";  // MEGA 2

char ssid[] = "AndroidAP8";
char pass[] = "gkyd8206";
// char e_mail_addr[] = "YourMail@example.com";  // почта для уведомления пустой бункер

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// stepper setting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DRIVER_SET 1  // driver setting. 0-step dir type; 1-l298n std type(11-impulse); 2-l298n double coil(22-impulse);
#define STEP_DIVIDER 1  // делитель для степ дир драйвера 1-32. если DRIVER_SET > 0 (l298n) то должен быть 1
#define STEPS_PER_RONUD 200  // шагов на оборот
uint16_t speed_deg_per_sec = 90;  // скорость градусов в секунду (360 = 1 оборот в секунду)
unsigned long step_high_delay = 10000;  // микросекунд будет высокий уровень при шаге (шум, нагрев/мощность)
const bool forward_dir = 1;  // направление вперед 1 или 0. Альтернатива поменять местами провода любой катушки
#define STEPS_WITHOUT_WEIGHT 50  // шагов между измерениями веса
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// other setting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// реконект блинка при отсутствии wi-fi
#define MAX_RECONNECT_ATTEMP 100       // количество попыток перезагрузки, если 100 то бесконечное, если 0 то откл.
#define RECONNECT_TIME 30000           // 30с

#define GENERAL_CONTROL_DAY 1  // упровляющий день контроллера
#define STEPPER_ROTATION_TIME 60000  // общее время работы шагового мотора при выдаче порции корма в миллисекундах
#define AC_DELAY_TIME 5000
#define SERVO_DELAY_TIME 5000
#define SPREADER_RUNING_TIME 60000  // столько работает спредер мс (60000мс = 60с)

#define BUTTON_PRESS_DELAY 5000  // столько мс нужно удерживать кнопку 

#define FEED_UP_TO 5  // покормить до 5 минут часа кормления

#define WEIGHT_DIFFERENCE 5  // разница веса в граммах для заднего хода шагового двигателя
#define FIRST_STEPS_DEF 3    // столько первых проходов функция движения назад будет заблокирована

// настройка весов
float calibration_factor = -225.2784271240;          // калибровка весов! известен из программы калибровки
uint32_t calibration_Weight = 939;         // 1 кг. Калибровочный вес, такой вес ставить на веса при автокалибровке
#define MEASURE_QUANTITY 20                               // количество измерений

uint16_t servo_open_angle = 0;  // угол открытой серво
uint16_t servo_close_angle = 90;  // угол закрытой серво

#define RELE_SIGNAL_TIPE 1  // тип управляющего сигнала реле 0 или 1
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM setup key
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EEPROM_SETUP_KEY 123  // любое число от 1 до 254. измените это число если нужно перепрошить EEPROM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// расписание:
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_GENERAL_CONTROL_DAY 200  // дней в таблице
// настройка времени кормления, часы кормления
uint8_t feeding_time_1 = 5;   // 1 кормление
uint8_t feeding_time_2 = 8;   // 2 кормление
uint8_t feeding_time_3 = 11;  // 3 кормление
uint8_t feeding_time_4 = 14;  // 4 кормление
uint8_t feeding_time_5 = 17;  // 5 кормление
uint8_t feeding_time_6 = 20;  // 6 кормление

// БАЗОВАЯ ТАБЛИЦА КОРМЛЕНИЯ
////////////////////////////////////////////////////////////
uint16_t basic_feeding_table[MAX_GENERAL_CONTROL_DAY+1] = {
/*0*/      0,        /*zero day, stop feedeng*/

/*День   вес г*/

/*1*/    1410,
/*2*/    1494,
/*3*/    1584,
/*4*/    1668,
/*5*/    1842,
/*6*/    1920,
/*7*/    2088,
/*8*/    2160,
/*9*/    2190,
/*10*/   2376,
/*11*/   2568,
/*12*/   2760,
/*13*/   2946,
/*14*/   3138,
/*15*/   3324,
/*16*/   3234,
/*17*/   3480,
/*18*/   3720,
/*19*/   3966,
/*20*/   4212,
/*21*/   4458,
/*22*/   4704,
/*23*/   4548,
/*24*/   4860,
/*25*/   5166,
/*26*/   5472,
/*27*/   5778,
/*28*/   6090,
/*29*/   6396,
/*30*/   6078,
/*31*/   6438,
/*32*/   6798,
/*33*/   7158,
/*34*/   7518,
/*35*/   7884,
/*36*/   8244,
/*37*/   7608,
/*38*/   7998,
/*39*/   8388,
/*40*/   8778,
/*41*/   9174,
/*42*/   9564,
/*43*/   9954,
/*44*/   9012,
/*45*/   9390,
/*46*/   9768,
/*47*/   10146,
/*48*/   10524,
/*49*/   10902,
/*50*/   11280,
/*51*/   10110,
/*52*/   10470,
/*53*/   10824,
/*54*/   11178,
/*55*/   11532,
/*56*/   11892,
/*57*/   12246,
/*58*/   11082,
/*59*/   11412,
/*60*/   11742,
/*61*/   12072,
/*62*/   12408,
/*63*/   12738,
/*64*/   13068,
/*65*/   12060,
/*66*/   12366,
/*67*/   12672,
/*68*/   12978,
/*69*/   13284,
/*70*/   13596,
/*71*/   13902,
/*72*/   12936,
/*73*/   13230,
/*74*/   13524,
/*75*/   13818,
/*76*/   14118,
/*77*/   14412,
/*78*/   14706,
/*79*/   13860,
/*80*/   14142,
/*81*/   14424,
/*82*/   14706,
/*83*/   14982,
/*84*/   15264,
/*85*/   15546,
/*86*/   15138,
/*87*/   15420,
/*88*/   15708,
/*89*/   15996,
/*90*/   16278,
/*91*/   16566,
/*92*/   16854,
/*93*/   16356,
/*94*/   16644,
/*95*/   16932,
/*96*/   17220,
/*97*/   17514,
/*98*/   17802,
/*99*/   18090,
/*100*/  17502,
/*101*/  17796,
/*102*/  18090,
/*103*/  18384,
/*104*/  18678,
/*105*/  18972,
/*106*/  19266,
/*107*/  18714,
/*108*/  19008,
/*109*/  19302,
/*110*/  19596,
/*111*/  19890,
/*112*/  20184,
/*113*/  20478,
/*114*/  19824,
/*115*/  20124,
/*116*/  20418,
/*117*/  20712,
/*118*/  21006,
/*119*/  21300,
/*120*/  21600,
/*121*/  20844,
/*122*/  21138,
/*123*/  21432,
/*124*/  21720,
/*125*/  22014,
/*126*/  22308,
/*127*/  22596,
/*128*/  21732,
/*129*/  22020,
/*130*/  22308,
/*131*/  22590,
/*132*/  22878,
/*133*/  23160,
/*134*/  23448,
/*135*/  22470,
/*136*/  22746,
/*137*/  23022,
/*138*/  23304,
/*139*/  23580,
/*140*/  23856,
/*141*/  24132,
/*142*/  23496,
/*143*/  23766,
/*144*/  24042,
/*145*/  24312,
/*146*/  24582,
/*147*/  24858,
/*148*/  25128,
/*149*/  24162,
/*150*/  24432,
/*151*/  24702,
/*152*/  24966,
/*153*/  25236,
/*154*/  25506,
/*155*/  25770,
/*156*/  24702,
/*157*/  24954,
/*158*/  25212,
/*159*/  25470,
/*160*/  25722,
/*161*/  25980,
/*162*/  26238,
/*163*/  25638,
/*164*/  25890,
/*165*/  26142,
/*166*/  26400,
/*167*/  26652,
/*168*/  26904,
/*169*/  27162,
/*170*/  26496,
/*171*/  26748,
/*172*/  26994,
/*173*/  27246,
/*174*/  27492,
/*175*/  27744,
/*176*/  27996,
/*177*/  27594,
/*178*/  27834,
/*179*/  28080,
/*180*/  28326,
/*181*/  28566,
/*182*/  28812,
/*183*/  29058,
/*184*/  29298,
/*185*/  29544,
/*186*/  29790,
/*187*/  30030,
/*188*/  30276,
/*189*/  30522,
/*190*/  30762,
/*191*/  31008,
/*192*/  31254,
/*193*/  31494,
/*194*/  31740,
/*195*/  31986,
/*196*/  32226,
/*197*/  32472,
/*198*/  32718,
/*199*/  32958,
/*200*/  33204
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////