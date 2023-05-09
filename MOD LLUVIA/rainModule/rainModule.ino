#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define rainAnalog A0
uint8_t rainDigital = 5; 
//const int rainDigital = D1
//#define static const uint8_t D1 = rainDigital;
//static const uint8_t rainDigital D1

// for ESP8266 Microcontroller
//#define rainAnalog A0
//#define rainDigital D1

void setup() {
  Serial.begin(9600);
  pinMode(rainDigital,INPUT);
}
void loop() {
  int rainAnalogVal = analogRead(rainAnalog);
  int rainDigitalVal = digitalRead(rainDigital);

  Serial.print(rainAnalogVal);
  Serial.print("t");
  Serial.println(rainDigitalVal);
  delay(200);
}
