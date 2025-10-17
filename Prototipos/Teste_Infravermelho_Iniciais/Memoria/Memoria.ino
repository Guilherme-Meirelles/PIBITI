#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>  //We need the base code
#include <IRLib_HashRaw.h>  //Only use raw sender


IRrecvPCI myReceiver(2);  //pin number for the receiver
IRsendRaw mySender;
uint16_t command[10][75];
uint8_t cont = 0;
const uint8_t num_command PROGMEM = 10;
uint8_t len;
void receiver(){
  if (myReceiver.getResults()) {
    len = recvGlobal.recvLength - 1;
    Serial.println(F("Signal Received"));
    if (cont < num_command || len > 10){
    for (uint8_t i = 1; i < recvGlobal.recvLength; i++) {
      command[cont][i-1] = recvGlobal.recvBuffer[i];
    }
    delay(1000);
    Serial.println(F("Vetor carregado"));
    cont++;
    
  }
  else {
    Serial.println(F("Máximo de comandos armazenados, esvazie"));
  }
  myReceiver.enableIRIn();
  }
}

void send_r(){
   
    mySender.send(command[0] , len, 36);
    Serial.println(F("Signal sent"));
    delay(1000);
    myReceiver.enableIRIn();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);                      
  myReceiver.enableIRIn();  // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
}
char serial;

char l;
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
      
      serial = Serial.read();
      if (serial == 'r'){
        l = serial;
        cont = 0;
        Serial.println(F("Preparado para receber"));
      }
      if (serial == 's'){
          Serial.println(F("Envio preparado"));
          l = 's';
        }
      }
      if (l == 'r'){
        receiver();  
      }
      send_r();
      /*
      else if (l == 's'){
        if(isDigit(serial)) { // Verifica se é um dígito numérico
          int i = serial - '0';
          send_r(command[i]);    // Use i como necessário
      } 
        else {
          Serial.println(F("Digite um digito valido"));
    // Tratar caso não seja um dígito
}
}
*/
      
      

}
