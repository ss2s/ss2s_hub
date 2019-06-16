// КОРМУШКА ДЛЯ РЫБ НА ESP32 v1.0

#define BLYNK_PRINT Serial  // Comment this out to disable prints and save space

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

void setup()
{
	// Debug console
	Serial.begin(9600);

	Blynk.begin(auth, ssid, pass);
}

void loop()
{
	Blynk.run();
}
