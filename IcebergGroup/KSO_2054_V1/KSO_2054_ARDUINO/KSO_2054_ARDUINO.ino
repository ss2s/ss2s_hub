// КОНТРОЛЛЕР СИСТЕМ ОБРАТНОГО ОСМОСА

// rele type
#define RELE_CONTROL_SET 0
#if RELE_CONTROL_SET == 0
#define DWH 0
#define DWL 1
#else
#define DWH 1
#define DWL 0
#endif

// ВХОДЫ ЦИФРОВЫЕ НАСТРАИВАЕМЫЕ 1:0 настройка ниже ↓
byte in_port_1 = 4;    // вход датчика низкого давления (нормально замкнутый, размыкается при низком давлении)
byte in_port_2 = 5;    // вход датчика высокого давления (нормально разомкнутый, закрытый при высоком давлении)
byte in_port_3 = 6;    // вход датчика заполнения емкости для сбора воды (нормально закрытый, открывается в случае наполнения емкости)
byte in_port_4 = 7;    // вход внешнего датчика блокировки (нормально открытый, система начинает работу при его закрытии)
byte in_port_5 = 8;    // вход датчика химической промывки (нормально замкнутый, размыкается при низком уровне)

// ВЫХОДЫ РЕЛЕ
byte out_port_1 = 9;   // выход для открытия/закрытия входного клапана (нормально открытый, пассивный)
byte out_port_2 = 10;  // выход для включения/выключения насоса высокого давления (нормально открытый, пассивный)
byte out_port_3 = 11;  // выход для открытия/закрытия клапана промывки (нормально открытый, пассивный)
byte out_port_4 = 12;  // выход для включения/выключения насоса химической промывки (нормально открытый, пассивный)

byte out_port_5 = 13;  // химпромывка 1
byte out_port_6 = 14;  // химпромывка 2
byte out_port_7 = 15;  // химпромывка 3
byte out_port_8 = 16;  // химпромывка 4

byte out_port_9 = 17;  // бузер (пищалка)
byte out_port_10 = 18;  // доп выход

// НАСТРОЙКА ТИПА ВХОДА (НОРМАЛЬНО ЗАМКНУТЫЙ 1, НОРМАЛЬНО РАЗОМКНУТЫЙ 0)
bool lowPressureSensorTypeIn = 1;
bool highPressureSensorTypeIn = 0;
bool tankFillSensorTypeIn = 1;
bool externalLockoutSensorTypeIn = 0;
bool chemicalWashSensorTypeIn = 1;

void setup(){

    pinMode(out_port_1,OUTPUT);
    pinMode(out_port_2,OUTPUT);
    pinMode(out_port_3,OUTPUT);
    pinMode(out_port_4,OUTPUT);

    pinMode(out_port_5,OUTPUT);
    pinMode(out_port_6,OUTPUT);
    pinMode(out_port_7,OUTPUT);
    pinMode(out_port_8,OUTPUT);

    pinMode(out_port_9,OUTPUT);  // бузер

    digitalWrite(out_port_1, DWL);
    digitalWrite(out_port_2, DWL);
    digitalWrite(out_port_3, DWL);
    digitalWrite(out_port_4, DWL);

    digitalWrite(out_port_5, DWL);
    digitalWrite(out_port_6, DWL);
    digitalWrite(out_port_7, DWL);
    digitalWrite(out_port_8, DWL);
    
    digitalWrite(out_port_8, LOW);  // бузер

    pinMode(in_port_1,INPUT);
    pinMode(in_port_2,INPUT);
    pinMode(in_port_3,INPUT);
    pinMode(in_port_4,INPUT);
    pinMode(in_port_5,INPUT);

    Serial.begin(115200);
    Serial1.begin(115200);
}

void loop(){
}

/*void sendToLCD(uint8_t type, String index, String cmd)
{
     if (type == 1 ){
        Nextion.print(index);
        Nextion.print(".txt=");
        Nextion.print("\"");
        Nextion.print(cmd);
        Nextion.print("\"");
    }
    else if (type == 2){
        Nextion.print(index);
        Nextion.print(".val=");
        Nextion.print(cmd);
    }
    else if (type == 3){
        Nextion.print(index);
        Nextion.print(".picc="); 
        Nextion.print(cmd);
    }
    else if (type ==4 ){
        Nextion.print("page ");
        Nextion.print(cmd);
    }

    Nextion.write(0xff);
    Nextion.write(0xff);
    Nextion.write(0xff);
}*/