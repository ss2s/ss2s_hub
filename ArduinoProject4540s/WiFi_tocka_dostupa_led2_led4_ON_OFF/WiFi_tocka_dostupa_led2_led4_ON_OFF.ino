// WiFi точка доступа

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char *ssi = "ESPHA";
const char *password = "12344321";

ESP8266WebServer server(80);
const int led2 = 2; 
const int led4 = 4; 

void led2_on() { 
digitalWrite(led2, 0); 
server.send(200, "text/html","gpio2 ON"); // в браузере пишет gpio2 ON и подает 0 на 2 pin
} 
void led2_off() { 
digitalWrite(led2, 1); 
server.send(200, "text/html","gpio2 OFF"); // в браузере пишет gpio2 OFF и подает 1 на 2 pin
} 


void led4_on() { 
digitalWrite(led4, 0); 
server.send(200, "text/html","gpio4 ON"); // в браузере пишет gpio2 ON и подает 0 на 2 pin
} 
void led4_off() { 
digitalWrite(led4, 1); 
server.send(200, "text/html","gpio4 OFF"); // в браузере пишет gpio2 OFF и подает 1 на 2 pin
} 



void handleRoot() {
	server.send(200, "text/html", "<h1>Soedinenie ustanovleno</h1>"); // текст
}



void setup() {
	delay(1000);
        pinMode(led2, OUTPUT); // устанавливает 2 pin на выход
        digitalWrite(led2, 1); // устанавливает на 2 pinе низкий уровень, диод не горит! 

         pinMode(led4, OUTPUT); // устанавливает 4 pin на выход
        digitalWrite(led4, 1); // устанавливает на 4 pinе низкий уровень, диод не горит! 

        
	Serial.begin(115200);
	Serial.println();
	Serial.print("Konfiguracia tocki dostupa..."); // отправка текста в сериал
	WiFi.softAP(ssi, password); // создание точки доступа

	IPAddress myIP = WiFi.softAPIP(); // присваивание значения для переменной myIP
	Serial.print("AP IP address: "); // отправка текста в сериал
	Serial.println(myIP); // отправка в сериал IP адреса
	server.on("/", handleRoot); // инициализация
        server.on("/gpio2on", led2_on); // отправка запроса на адрес http://192.168.4.1/gpio2on для значения led2_on
        server.on("/gpio2off", led2_off); // отправка запроса на адрес http://192.168.4.1/gpio2off для значения led2_off

        server.on("/gpio4on", led4_on); // отправка запроса на адрес http://192.168.4.1/gpio4on для значения led4_on
        server.on("/gpio4off", led4_off); // отправка запроса на адрес http://192.168.4.1/gpio4off для значения led4_off

        
	server.begin(); // включение сервера
	Serial.println("HTTP server started"); // отправка текста в сериал
}

void loop() {
	server.handleClient(); // получение пакетов от сервера
}
