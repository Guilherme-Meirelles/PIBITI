#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>  //We need the base code
#include <IRLib_HashRaw.h>  //Only use raw sender


IRrecvPCI myReceiver(2);  //pin number for the receiver


uint8_t cont = 0;
const uint8_t num_command PROGMEM = 5;
uint8_t len;
uint16_t command[6][75];
//uint16_t one_command[75];

 

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

void send_r(const uint16_t one_command[]){
    IRsendRaw mySender;
    mySender.send(one_command , len, 36);
    Serial.println(F("Signal sent"));
    delay(1000);
    myReceiver.enableIRIn();
}
  


   
void setup(){
  Serial.begin(9600);                      
  myReceiver.enableIRIn();  // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
}
char serial;

char l;

void loop(){
  
    
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
      
          if (serial == '0'){
             send_r(command[0]);
          }
          else if(serial == '1'){
            send_r(command[1]);
          }
          else if(serial == '2'){
            send_r(command[2]);
          }
          else if(serial == '3'){
            send_r(command[3]);
          }
          else if(serial == '4'){
            send_r(command[4]);
          }
          else if(serial == '5'){
            send_r(command[5]);
          }
          /*
          else if(serial == '5'){
            send_r(5);
          }
          
          else if(serial == '6'){
            send_r(6);
          }
          else if(serial == '7'){
            send_r(7);
          }
          else if(serial == '8'){
            send_r(8);
          }
          else if(l == '9'){
            send_r(9);
          }
*/
  } 
        
        
      
    






