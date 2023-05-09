#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// Crea una instancia del sensor BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  // Inicializa el puerto serie
  Serial.begin(9600);
  Serial.println("Inicializando el sensor BMP180...");

  // Inicializa el sensor BMP180
  if (!bmp.begin()) {
    Serial.print("No se pudo encontrar un sensor BMP180 válido, verifica las conexiones.");
    while (1);
  }
}

void loop() {
  // Estructura para almacenar los datos del sensor
  sensors_event_t event;

  // Lee la presión atmosférica
  bmp.getEvent(&event);

  // Muestra la presión atmosférica en el puerto serie
  if (event.pressure) {
    Serial.print("Presión: ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
  } else {
    Serial.println("Error al leer la presión");
  }

  // Espera 2 segundos antes de leer los datos nuevamente
  delay(2000);
}