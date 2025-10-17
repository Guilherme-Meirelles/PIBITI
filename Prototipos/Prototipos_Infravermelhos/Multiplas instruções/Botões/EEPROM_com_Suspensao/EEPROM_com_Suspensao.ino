#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>
#include <avr/sleep.h>

 // Altere para o pino desejado


IRrecvPCI myReceiver(2);
uint8_t suspensao = 3;
uint8_t recepcao = 4;
uint8_t val;
uint8_t val_antigo;
uint8_t estado = 0;
uint8_t ligar_desligar = A2;
uint8_t flag = 1;
uint8_t descer = A1;
uint8_t subir = A0;
uint8_t flagv = 2;
unsigned long tempo;

uint8_t cont = 0;
const uint16_t sentinela = 1023;
uint8_t len = EEPROM.read(sentinela);
uint8_t max_command = 1023 / len;
uint16_t* tempBuffer;

void saveCommandToEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint8_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.update(address, (uint8_t)(tempBuffer[i] / 50));
    address += sizeof(uint8_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint8_t);
  uint16_t variavel_auxiliar;
  for(uint8_t i = 0; i < cmdLength; i++) {
    variavel_auxiliar = EEPROM.read(address);
    tempBuffer[i] = variavel_auxiliar * 50;
    address += sizeof(uint8_t);
  }
}

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (len != recvGlobal.recvLength - 1){
        cont = 0;
        Serial.println(F("Reconfigurando"));

        len = recvGlobal.recvLength - 1;
        max_command = 1023 / len;
        tempBuffer = (uint16_t *)realloc(tempBuffer, sizeof(len * sizeof(uint16_t)));
        EEPROM.update(sentinela, len);
      }
    

    
    for (uint8_t i = 1; i <= len; i++) {
      tempBuffer[i-1] = recvGlobal.recvBuffer[i];
    }
    
    if (cont < max_command) {
      saveCommandToEEPROM(cont, len);
      cont++;
      Serial.println(F("Command saved to EEPROM"));
      EEPROM.update(sentinela, len);
    } else {
      Serial.println(F("Max commands reached"));
    }
    }
    myReceiver.enableIRIn();
    tempo = millis();
  }

}

void send_r(uint8_t cmdIndex) {
  loadCommandFromEEPROM(cmdIndex, len);
  IRsendRaw mySender;
  mySender.send(tempBuffer, len, 36);
  Serial.print(F("Signal sent, command: "));
  Serial.println(cmdIndex);
  tempo = millis();
  delay(2000);
}

void setup() {
  Serial.begin(9600);                      
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
  pinMode(suspensao, INPUT_PULLUP);
  pinMode(recepcao,INPUT);
  pinMode(ligar_desligar, INPUT);
  tempBuffer = (uint16_t *)malloc(len * sizeof(uint16_t));
  tempo = millis();
}

void loop() {

  val = digitalRead(recepcao);
  delay(50);
  if ((val == HIGH) && (val_antigo == LOW)){
    estado = 1 - estado;
    delay(10);
    if (estado == 0){
      Serial.println(F("Not Receiving"));
      myReceiver.disableIRIn();
    }
    else {
      myReceiver.enableIRIn();
      Serial.println(F("Ready to receive"));
      cont = 0;
    }
  }
  val_antigo = val;

  if (estado == 1){
    receiver();
  }
  if (estado == 0){
    delay(50);
    if (digitalRead(ligar_desligar) == HIGH){
      delay(10);
      flag = 1 - flag;
      send_r(flag);
    }
    else if (digitalRead(descer) == HIGH){
      delay(10);
      if (flagv > 2){
        flagv --;
      }
      send_r(flagv);
    }
    else if (digitalRead(subir) == HIGH){
      delay(10);
      if (flagv < max_command - 1){
        flagv ++;
      }
      send_r(flagv);
     }
  }

    if (digitalRead(suspensao) == LOW ){
      while (digitalRead(suspensao) == LOW){
    }
    delay(50);
    durmaBem();
  }
  /*
  if (millis() - tempo > 10000){
    durmaBem();
  }
*/
}


void durmaBem() {

  
  Serial.println(F("Indo mimir"));
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(suspensao), acorda, LOW);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void acorda() {
  
 
  detachInterrupt(digitalPinToInterrupt(suspensao));
  sleep_disable();
  tempo = millis();
  while (digitalRead(suspensao) == LOW){
    delay(50);
    }
    delay(50);
  Serial.println(F("Interrupção ativada, acordando arduino"));
}