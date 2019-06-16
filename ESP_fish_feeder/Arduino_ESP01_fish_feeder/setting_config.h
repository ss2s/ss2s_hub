// НАСТРОЙКИ СИСТЕМЫ:

// введите Auth Token для выбранной платы, у каждой платы свой токен, он отправляется на e-mail из blynk
char auth[] = "YourAuthToken";  // Auth Token, токен аунтефикации

// введите имя "SSID" точки доступа и пароль
char ssid[] = "YourNetworkName";  // имя точки доступа "SSID"
char pass[] = "YourPassword";  // пароль точки доступа

// PINOUT:
#define SOFT_RX_PIN 2
#define SOFT_TX_PIN 3

// БАЗОВАЯ ТАБЛИЦА КОРМЛЕНИЯ
uint16_t basic_feeding_table[200];