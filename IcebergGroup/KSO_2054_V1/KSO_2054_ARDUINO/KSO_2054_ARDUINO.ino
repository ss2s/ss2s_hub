// КОНТРОЛЛЕР СИСТЕМ ОБРАТНОГО ОСМОСА

// ВХОДЫ ЦИФРОВЫЕ НАСТРАИВАЕМЫЕ
#define LOW_PRESSURE_SENSOR_PIN_IN 4        // вход датчика низкого давления (нормально замкнутый, размыкается при низком давлении)
#define HIGH_PRESSURE_SENSOR_PIN_IN 5       // вход датчика высокого давления (нормально разомкнутый, закрытый при высоком давлении)
#define TANK_FILL_SENSOR_PIN_IN 6           // вход датчика заполнения емкости для сбора воды (нормально закрытый, открывается в случае наполнения емкости)
#define EXTERNAL_LOCKOUT_SENSOR_PIN_IN 7    // вход внешнего датчика блокировки (нормально открытый, система начинает работу при его закрытии)
#define CHEMICAL_WASH_SENSOR_PIN_IN 8       // вход датчика химической промывки (нормально замкнутый, размыкается при низком уровне)
// ВЫХОДЫ РЕЛЕ 220V
#define INPUT_GATE_RELE_PIN_OUT 9           // выход для открытия/закрытия входного клапана (нормально открытый, пассивный)
#define HIGH_PRESSURE_PUMP_RELE_PIN_OUT 10  // выход для включения/выключения насоса высокого давления (нормально открытый, пассивный)
#define WASH_GATE_RELE_PIN_OUT 11           // выход для открытия/закрытия клапана промывки (нормально открытый, пассивный)
#define CHEMICAL_WASHING_PUMP 12            // выход для включения/выключения насоса химической промывки (нормально открытый, пассивный)
// ТИП ВХОДА (НОРМАЛЬНО ЗАМКНУТЫЙ, НОРМАЛЬНО РАЗОМКНУТЫЙ)
bool lowPressureSensorTypeIn = 1;
bool highPressureSensorTypeIn = 0;
bool tankFillSensorTypeIn = 1;
bool externalLockoutSensorTypeIn = 0;
bool chemicalWashSensorTypeIn = 1;

void setup() {
}

void loop() {
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