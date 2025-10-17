#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>
#include <avr/sleep.h>
 // Altere para o pino desejado


IRrecvPCI myReceiver(2);
uint8_t pinBot = 3;
unsigned int tempo;

uint8_t cont = 0;
const uint16_t sentinela = 1023;
uint8_t len = EEPROM.read(sentinela);
uint8_t max_command = 1023 /(len *2);
uint16_t* tempBuffer;

void saveCommandToEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.put(address, tempBuffer[i]);
    address += sizeof(uint16_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.get(address, tempBuffer[i]);
    address += sizeof(uint16_t);
  }
}

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (len != recvGlobal.recvLength - 1){
        cont = 0;
        Serial.println(F("Reconfigurando"));

        len = recvGlobal.recvLength - 1;
        max_command = 1023 / (len*(sizeof(uint16_t)));
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

  }

}

void send_r(uint8_t cmdIndex) {
  loadCommandFromEEPROM(cmdIndex, len);
  IRsendRaw mySender;
  mySender.send(tempBuffer, len, 36);
  Serial.println(F("Signal sent"));
  tempo = millis();
}

void setup() {
  Serial.begin(9600);                      
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
  pinMode(pinBot, INPUT_PULLUP);

  tempBuffer = (uint16_t *)malloc(len * sizeof(uint16_t));
  tempo = millis();
}
bool a;
void loop() {
  
  if (Serial.available()) {
    char serial = Serial.read();
    
    if (serial == 'r') {
      cont = 0;
      myReceiver.enableIRIn();
      Serial.println(F("Ready to receive"));
      a = true;
      while(a) {
        receiver();
        tempo = millis();
        if (Serial.read() == 's'){
          myReceiver.disableIRIn();
          a = false;
          Serial.println(F("Not Receiving"));
        }
      }
    }
    else if (serial >= '0' && serial <= '9') {
      
      if (max_command < serial - '0'){
        Serial.println(F("Comando inválido, máximo comando é"));
        Serial.print(max_command);
      }
      else {
      send_r(serial - '0');
      }
    }
  }
    if (digitalRead(pinBot) == LOW ){
    while (digitalRead(pinBot) == LOW){
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
  attachInterrupt(digitalPinToInterrupt(pinBot), acorda, LOW);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void acorda() {
  
 
  detachInterrupt( digitalPinToInterrupt(pinBot));
  sleep_disable();
  tempo = millis();
  while (digitalRead(pinBot) == LOW){
    delay(50);
    }
    delay(50);
  Serial.println(F("Interrupção ativada, acordando arduino"));


}