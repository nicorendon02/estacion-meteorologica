#include <DHT.h>

#define DHTPIN D4     // define el pin de datos del sensor
#define DHTTYPE DHT11 // define el tipo de sensor que se está utilizando

DHT dht(DHTPIN, DHTTYPE); // inicializa el sensor

void setup() {
  Serial.begin(115200); // inicia la comunicación serial para mostrar los datos en la consola
  dht.begin();          // inicia el sensor
}

void loop() {
  delay(300000); // espera 5 minutos antes de leer los datos del sensor

  float h = dht.readHumidity();    // lee la humedad del sensor
  float t = dht.readTemperature(); // lee la temperatura del sensor

  // muestra los datos en la consola
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println("°C");
}

