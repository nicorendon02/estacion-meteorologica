#include <Wire.h>
#include "RTClib.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid     = "tu_red_wifi";
const char* password = "tu_contraseña_wifi";

WiFiUDP ntpUDP;

// Servidor NTP (Pool NTP de América Latina)
const char* ntpServer = "pool.ntp.org";

// Instanciar NTPClient para poder obtener la hora desde el servidor NTP
NTPClient timeClient(ntpUDP, ntpServer, -6 * 60 * 60, 60000);

// Instanciar RTC DS3231
RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);

  // Conectar a Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected!");

  // Iniciar la comunicación con el RTC DS3231
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Error al comunicarse con DS3231");
    while (1);
  }

  // Sincronizar hora desde el servidor NTP
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  // Actualizar el RTC con la hora obtenida
  rtc.adjust(DateTime(timeClient.getEpochTime()));
}

void loop() {
  // Obtener la hora actual desde el RTC DS3231
  DateTime now = rtc.now();

  // Imprimir la hora en la consola
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000);
}
}
