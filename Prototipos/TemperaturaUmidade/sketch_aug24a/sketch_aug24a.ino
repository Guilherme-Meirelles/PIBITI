#include <dht11.h>

int pinoDHT11 = 5;

dht11 DHT11;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  DHT11.read(pinoDHT11);
  Serial.println("Humidade: ");
  Serial.print(DHT11.humidity);
  Serial.println("%");

  Serial.println("Temperatura: ");
  Serial.print(DHT11.temperature);
  Serial.println("C");
  Serial.println("___________");
  delay(2000);
}
