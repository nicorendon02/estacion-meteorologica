// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.
int pinSensor=A0;
char lluvia;

// include reloj
#include <NTPClient.h>
#include <WiFiUdp.h>

//include presion Atm
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// include osquitar
#include <ESP8266WiFi.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"

// include temp y humedad
#include <DHT.h>

#define DHTPIN D4     // define el pin de datos del sensor
#define DHTTYPE DHT11 // define el tipo de sensor que se está utilizando

DHT dht(DHTPIN, DHTTYPE); // inicializa el sensor

// Crea una instancia del sensor BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// configuracion reloj
WiFiUDP ntpUDP;

// Servidor NTP (Pool NTP de América Latina)
const char* ntpServer = "pool.ntp.org";

// Instanciar NTPClient para poder obtener la hora desde el servidor NTP
NTPClient timeClient(ntpUDP, ntpServer, -6 * 60 * 60, 60000);

// Instanciar RTC DS3231
RTC_DS3231 rtc;

#define WIFI_SSID "NICO"
#define WIFI_PASSWORD "nico1228"

const char* SCOPE_ID = "0ne009DBDFB";
const char* DEVICE_ID = "1iaapabmtpi";
const char* DEVICE_KEY = "teW5yzBRRiyWqVcsTkMGQPp/MLhevr9z3z4ujRihAvk=";

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // ConnectionStatus
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }

  // payload buffer doesn't have a null ending.
  // add null ending in another buffer before print
  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] event was received. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Command name was => %s\r\n", callbackInfo->tag);
  }
}

void setup() {
  
  Serial.begin(9600);

  // setup temp y humedad
  dht.begin();          // inicia el sensor

  //setup reloj
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

  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  if (context != NULL) {
    lastTick = 0;  // set timer in the past to enable first telemetry a.s.a.p
  }

  // Inicializa el sensor BMP180
  if (!bmp.begin()) {
    Serial.print("No se pudo encontrar un sensor BMP180 válido, verifica las conexiones.");
    while (1);
  }
}

void loop() {
  // loop temp y humedad
  float h = dht.readHumidity();    // lee la humedad del sensor
  float t = dht.readTemperature(); // lee la temperatura del sensor

  // loop reloj
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

  // Sensor Presion ATM
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

  //luvia
  int valor = analogRead(pinSensor);
  if (valor > 950)
    lluvia='No hay lluvia';
  else if (valor <= 950 && valor >600 )
  {
    lluvia='Llueve';
  }
  else if (valor <= 600 && valor > 300)
  {
    lluvia='Aguacero';
  }
  else
  {
    lluvia='Se inundó esto!';
  }
  delay(1); 

  // codigo osquitar
  if (isConnected) {
    unsigned long ms = millis();
    if (ms - lastTick > 300000) {  // send telemetry every 5 minutes
      char msg[64] = { 0 };
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // send telemetry
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Temperatura\": %d, \"Humedad\": %d, \"PresionATM\": %d, \"Lluvia\": %d, \"fecha\": %d, \"Hora\": %d, \"id_usu\": %d}",
               t, h, event.pressure, lluvia, now, now.hour(), "tu cedula");
        errorCode = iotc_send_telemetry(context, msg, pos);
      
      } else {  // send property
                // pos = snprintf(msg, sizeof(msg) - 1, "{\"PresionATM\":%d}",
        //               1 + (rand() % 5));
        //errorCode = iotc_send_property(context, msg, pos);
      }
      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Sending message has failed with error code %d", errorCode);
      }
    }

    iotc_do_work(context);  // do background work for iotc
  } else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);
  }
}
