#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>

IRrecvPCI myReceiver(2);

const uint8_t MAX_COMMANDS PROGMEM = 6;
const uint8_t MAX_PULSES PROGMEM = 75;
uint8_t cont = 0;
uint16_t sentinela = 1023;
uint8_t len = EEPROM.read(sentinela);
uint16_t tempBuffer[MAX_PULSES];

void saveCommandToEEPROM(uint8_t cmdIndex, const uint16_t* cmdData, uint8_t cmdLength) {
  uint16_t address = cmdIndex * MAX_PULSES * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.put(address, cmdData[i]);
    address += sizeof(uint16_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex, uint16_t* cmdData, uint8_t cmdLength) {
  uint16_t address = cmdIndex * MAX_PULSES * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.get(address, cmdData[i]);
    address += sizeof(uint16_t);
  }
}

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
    len = recvGlobal.recvLength - 1;
    if (len > MAX_PULSES) len = MAX_PULSES;
    
    for (uint8_t i = 1; i <= len; i++) {
      tempBuffer[i-1] = recvGlobal.recvBuffer[i];
    }
    
    if (cont < MAX_COMMANDS) {
      saveCommandToEEPROM(cont, tempBuffer, len);
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
  
  loadCommandFromEEPROM(cmdIndex, tempBuffer, len);
  IRsendRaw mySender;
  mySender.send(tempBuffer, len, 36);
  Serial.println(F("Signal sent"));
  myReceiver.enableIRIn();
}

void setup() {
  Serial.begin(9600);                      
  myReceiver.enableIRIn();
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
}
bool a = true;
void loop() {
  
  if (Serial.available()) {
    char serial = Serial.read();
    
    if (serial == 'r') {
      cont = 0;
      Serial.println(F("Ready to receive"));
      while(a) {
        receiver();
        if (Serial.read() == 's'){
          a = false;
        }
      }
    }
    else if (serial >= '0' && serial <= '5') {
      send_r(serial - '0');
    }
  }
}