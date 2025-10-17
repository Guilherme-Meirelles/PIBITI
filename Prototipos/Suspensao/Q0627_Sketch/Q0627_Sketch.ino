/*
     CÓDIGO:  Q0627
     AUTOR:   BrincandoComIdeias
     LINK:    https://www.youtube.com/brincandocomideias ; https://cursodearduino.net/ ; https://cursoderobotica.net
     COMPRE:  https://www.arducore.com.br/
     SKETCH:  Fazer o Arduino Dormir
     DATA:    12/02/2020
*/

// INCLUSÃO DE BIBLIOTECAS
#include <avr/sleep.h>

// DEFINIÇÕES DE PINOS
#define pinBot 2
#define pinLed 7

// DECLARAÇÃO DE FUNÇÕES
void durmaBem();

// DECLARAÇÃO DE VARIÁVEIS
bool estadoBUILTIN = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando setup()");

  pinMode(pinLed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinBot, INPUT_PULLUP);

  digitalWrite(pinLed, LOW);

  Serial.println("Fim setup()");
}

void loop() {
  Serial.println("Ligando o LED");
  digitalWrite(pinLed, HIGH);
  delay(5000);
  Serial.println("Indo Dormir");
  digitalWrite(pinLed, LOW);

  durmaBem();
  Serial.println("Arduino Acordado!");

  estadoBUILTIN = !estadoBUILTIN;
  digitalWrite(LED_BUILTIN, estadoBUILTIN);

  delay(2000);
}

// IMPLEMENTO DE FUNÇÕES

void durmaBem() {

  attachInterrupt( digitalPinToInterrupt(pinBot), acorda, LOW );

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  delay(1000);

  sleep_cpu();
}

void acorda() {
  Serial.println("Interrupção ativada, acordando arduino");
  sleep_disable();
  detachInterrupt( digitalPinToInterrupt(pinBot) );
}
