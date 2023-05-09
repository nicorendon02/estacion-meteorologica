#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

void setup() {
  // Inicializa el puerto serie
  Serial.begin(115200);

  // Inicializa WifiManager
  WiFiManager wifiManager;

  // Restablece las configuraciones guardadas, útil para pruebas y desarrollo
  // Descomenta la siguiente línea si deseas restablecer la configuración de Wi-Fi en cada inicio
  // wifiManager.resetSettings();

  // Establece el nombre de la red y la contraseña del punto de acceso para la configuración
  wifiManager.autoConnect("AP_Esp8266", "passwordAP");

  // Si llegamos hasta aquí, el ESP8266 se ha conectado exitosamente a la red Wi-Fi
  Serial.println("Conectado al Wi-Fi.");
}

void loop() {
  // El código principal de tu programa va aquí
}