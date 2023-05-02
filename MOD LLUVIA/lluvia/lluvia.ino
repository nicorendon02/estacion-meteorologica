int pinSensor=A0;
void setup()
{
  Serial.begin(9600);
}
void loop()
{
  int valor = analogRead(pinSensor);
  if (valor > 950)
    Serial.println("No hay lluvia");
  else if (valor <= 950 && valor >600 )
  {
    Serial.println("Llueve");
  }
  else if (valor <= 600 && valor > 300)
  {
    Serial.println("Aguacero");
  }
  else
  {
    Serial.println("Se inundó esto!");
  }
  delay(1); 
}
