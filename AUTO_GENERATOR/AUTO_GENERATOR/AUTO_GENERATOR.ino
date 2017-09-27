// АВТОЗАПУСК ГЕНЕРАТОРА

// ***НАСТРОЙКА*** //

// РАСПИНОВКА
#define IN_CETb_PIN 2              // ВХОД 5в СЕТЬ
#define IN_GENA_PIN 3              // ВЧОД 5в ГЕНЕРАТОР
#define OUT_IGNITION_PIN 4         // РЕЛЕ ЗАЖИГАНИЯ
#define OUT_PODSOS_ON_PIN 5        // РЕЛЕ ВКЛЮЧЕНИЯ ПОДСОСА
#define OUT_STARTER_PIN 6          // РЕЛЕ СТАРТЕРА
#define OUT_PODSOS_OFF_PIN 7       // РЕЛЕ ВЫКЛЮЧЕНИЯ ПОДСОСА
#define OUT_KONTAKTOR_PIN 8        // РЕЛЕ КОНТАКТОРА (СИЛОВОЕ)
#define IN_ERROR_PIN 9             // ВХОД 5в ОШИБКА ПРИ ЗАМЫКАНИИ НА ЛИНИИ
#define IN_STOP_PIN 10             // ВХОД 5в СТОП
#define LED_PIN 13                 // ДИОД НА ПЛАТЕ АРДУИНО "L"

// ВРЕМЕННЫЕ ЗАДЕРЖКИ В мс
#define PODSOS_TIMEOUT 800         // ВРЕМЯ РАБОТЫ ПРИВОДА ПОДСОСА 0.8 с
#define STARTER_TIMEOUT 2000       // ВРЕМЯ РАБОТЫ СТАРТЕРА 2 с
#define PRE_KONT_TIMEOUT 15000     // ЗАДЕРЖКА ПЕРЕД ВКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ЗАКРЫТИЯ ПОДСОСА 15 с
#define KONT_OFF_TIMEOUT 10000     // ЗАДЕРЖКА ПЕРЕД ВЫКЛЮЧЕНИЕМ КОНТАКТОРА, ПОСЛЕ ВОССТАНОВЛЕНИЯ СЕТИ 10 с
#define OFF_IGNITION_TIMEOUT 30000 // ЗАДЕРЖКА ВЫКЛЮЧЕНИЯ ЗАЖИГАНИЯ 30 с

// ДРУГИЕ НАСТРОЙКИ
#define ZAPUSK_COUNTER_VAL 5       // КОЛИЧЕСТВО ПОПЫТОК ЗАПУСКА ГЕНЕРАТОРА

// ***КОНЕЦ НАСТРОЙКИ*** //

// НАСТРОЙКА РЕЛЕ (не трогать)
// если реле управляется 1
#define HHH 1
#define LLL 0
// если реле управляется 0
//#define HHH 0
//#define LLL 1

void setup() {

	pinMode(IN_CETb_PIN, INPUT);
	pinMode(IN_GENA_PIN, INPUT);
	pinMode(OUT_IGNITION_PIN, OUTPUT);
	pinMode(OUT_PODSOS_ON_PIN, OUTPUT);
	pinMode(OUT_STARTER_PIN, OUTPUT);
	pinMode(OUT_PODSOS_OFF_PIN, OUTPUT);
	pinMode(OUT_KONTAKTOR_PIN, OUTPUT);
	pinMode(IN_ERROR_PIN, INPUT);
	pinMode(IN_STOP_PIN, INPUT);

	digitalWrite(OUT_IGNITION_PIN, LLL);
	digitalWrite(OUT_PODSOS_ON_PIN, LLL);
	digitalWrite(OUT_STARTER_PIN, LLL);
	digitalWrite(OUT_PODSOS_OFF_PIN, LLL);
	digitalWrite(OUT_KONTAKTOR_PIN, LLL);

	Serial.begin(9600);
}

void loop() {
}
