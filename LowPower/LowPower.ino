#include <LowPower.h>
int val;
int tempo;
int botao = 2;
int state;
void setup(){
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(2), wakeUp, FALLING);
  Serial.println("Aqui");
  tempo = millis();

}

void loop(){
  delay(50);
  val = digitalRead(botao);
  while (digitalRead(2) == LOW){
    delay(100);
  }
  delay(50);
  if (val == LOW){
    Serial.println("Indo dormir");
    delay(50);
    state = 1;
    
    delay(1000);
    attachInterrupt(digitalPinToInterrupt(botao), wakeUp, LOW);
    while (state){
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
  }
}

void wakeUp(){
  detachInterrupt(digitalPinToInterrupt(botao));
  while (digitalRead(2) == LOW){
    delay(100);
  }
  state = 0;
  
  delay(50);
  Serial.println("Acordei");
  delay(50);
}


