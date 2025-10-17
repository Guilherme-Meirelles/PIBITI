const int sensorPin = A0;  // Pino onde o fototransistor está conectado

void setup() {
  Serial.begin(9600);  // Inicia a comunicação serial
}

void loop() {
  int sensorValue = analogRead(sensorPin);  // Lê o valor do fototransistor
  Serial.println(sensorValue);             // Exibe o valor no monitor serial
  delay(100);                              // Aguarda 100 ms
}
