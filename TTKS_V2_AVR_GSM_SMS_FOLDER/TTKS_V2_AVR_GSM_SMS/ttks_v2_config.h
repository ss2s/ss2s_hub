///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TTKS_V2_AVR_GSM_SMS
// Контроллер удаленного управления генератором с помощью СМС сообщений
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// программа разрабатывалась в Arduino IDE 1.8.1 для компиляции и прошивки используйте эту версию
// скачать можно здесь:
// без установки    https://www.arduino.cc/en/Main/Donate
// с установкой     https://www.arduino.cc/en/Main/Donate
// все версии       https://www.arduino.cc/en/Main/OldSoftwareReleases#previous
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// НАСТРОЙКИ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// РАСПИНОВКА:
// здесь настраиваются выводы микроконтроллера

	#define RESET_MODEM_PIN 8                  // RESET sim800 optional

	#define STARTER_RELE_PIN A1                // РЕЛЕ СТАРТЕРА 30А ИЛИ МОСФЕТ 30А

	#define AIR_DAMPER_RELE_PIN A2             // РЕЛЕ ВОЗДУШНОЙ ЗАСЛОНКИ
	#define STOP_GENERATOR_RELE_PIN A3         // РЕЛЕ ОСТАНОВКИ ГЕНЕРАТОРА

	#define VOLTAGE_VCC_SENSOR_PIN 4           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ VCC
	#define VOLTAGE_OUT_SENSOR_PIN 5           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ OUT
	#define VOLTAGE_GND_SENSOR_PIN 6           // ИНДИКАТОР НАПРЯЖЕНИЯ НА ГЕНЕРАТОРЕ GND

	#define SOFT_RX_PIN 10                     // SoftSerial RX
	#define SOFT_TX_PIN 11                     // SoftSerial TX
	#define GND_PIN 12                         // GND

	#define BATTERY_12V_VOLTAGE_SENSOR_PIN A0  // НАПРЯЖЕНИЯ НА АККУМУЛЯТОРЕ ГЕНЕРАТОРА

// НАСТРОЙКИ ПРОГРАММЫ:

	// КНОПКИ ТЕЛЕФОНА
	#define BUTTON_GENERATOR_START "1"
	#define BUTTON_STOP_GENERATOR "2"
	#define BUTTON_GENERATOR_STATUS "3"
	#define BUTTON_SETTING "#"   // to future...
	#define BUTTON_REMINDER "0"  // reminder

	// номера телефонов с которых будет принимать звонок
	String whiteListPhones = "+380xxxxxxxxx, +380xxxxxxxxx, +380xxxxxxxxx"; // Белый список телефонов
	String masterPhones = "+380xxxxxxxxx";     // мастер телефон

	// настройки запуска
	uint32_t timeOut_starter_ON = 3000;    // столько работает стартер 2s (1-5s)
	uint8_t  number_launch_attempt = 5;    // количество попыток запуска, при неудачном старте. MAX255

	uint32_t timeOut_stop_rele_ON = 5000;  // столько работает реле глущения генератора 2s (1-5s)
	uint8_t  number_stop_attempt = 3;      // количество попыток остановки, при неудачном старте. MAX255

	uint32_t voice_timeout = 5000;         // задержка для голосового ответа >5s

	// задержки
	#define timeOut_operation 50           // other operation timeout 0.05s
	#define timeOut_operation_2 1000       // other operation timeout 2 = 0.5s // задержка после отключения стартера
	#define timeOut_prev_starter_DEL 500   // задержка перед запуском стартера, после открытия заслонки 0.5s
	#define timeOut_airDamper_OFF 2000     // задержка перед закрытием воздушной заслонки 2s
	#define timeOut_next_start 10000       // задержка между попытками запуска, при неудачном старте


	// НАСТРОЙКИ ФУНКЦИЙ  // эта функция еще не активна
	#define SET_STARTER_TIMER_FUNC 0       // ЕСЛИ 0 ТО СТАРТЕР БУДЕТ УЧИТЫВАТЬ ТОЛЬКО ТАЙМАУТ СТАРТЕРА
								           // ЕСЛИ 1 ТО СТАРТЕР БУДЕТ УЧИТЫВАТЬ НАПРЯЖЕНИЕ НА ГЕНЕРАТОРЕ И ТАЙМАУТ СТАРТЕРА
	// типы активации реле
	// реле бывает 2 типов активации 1 или 0. здесь можно настроить тип активации реле
	#define Y_220 0                        // out sensor 220 yes
	#define N_220 1                        // out sensor 220 no
	#define RELE_HIGH 0                    // 10A rele high
	#define RELE_LOW 1                     // 10A rele low
	#define STARTER_RELE_HIGH 1            // 30A starter rele high
	#define STARTER_RELE_LOW 0             // 30A starter rele low

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// КОНЕЦ ВСЕХ НАСТРОЕК
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////