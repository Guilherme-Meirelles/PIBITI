#include <IRremote.hpp>

#define IR_SEND_PIN 3  // Pino digital conectado ao LED IR (deve ser PWM no Uno/Nano)
int endereco = 7;
int comando = 11;
void setup() {
  Serial.begin(9600);
  Serial.println("Transmissor IR Pronto");
  
  IrSender.begin(IR_SEND_PIN);  // Inicializa o emissor IR
}

void loop() {
  // Exemplo 1: Envia comando POWER (0x45) para um dispositivo com endereço 0xFF00
  sendNEC(endereco, comando);  // Endereço 0xFF00 + Comando POWER (0x45)
  delay(5000);            // Espera 5 segundos

  // Exemplo 2: Envia comando VOLUME+ (0x16)
  
}

// Função para enviar comandos no protocolo NEC
void sendNEC(int address, int command) {
  Serial.print("Enviando NEC -> Endereço: 0x");
  Serial.print(address, HEX);
  Serial.print(", Comando: 0x");
  Serial.println(command, HEX);
  
  IrSender.sendSamsung(address, command, 0);  // 0 = sem repetições
}
