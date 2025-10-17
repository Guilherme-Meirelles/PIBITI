#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>  //We need the base code
#include <IRLib_HashRaw.h>  //Only use raw sender

IRrecvPCI myReceiver(2);  //pin number for the receiver
IRsendRaw mySender;

int *command = (int*) malloc(100 * sizeof(int));
int len;
char s;
void setup() {
  Serial.begin(9600);                      
  myReceiver.enableIRIn();  // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  myReceiver.setFrameTimeout(100000);
}

void loop() {
  //Continue looping until you get a complete signal received
  if (Serial.available()){
    s = Serial.read();
  }
if (s == 'a'){
  if (myReceiver.getResults()) {
    len = recvGlobal.recvLength - 1;
    Serial.println(F("Signal Received"));
    command = (int*) realloc(command,len * sizeof(int));
    for (bufIndex_t i = 1; i < recvGlobal.recvLength; i++) {
      command[i-1] = recvGlobal.recvBuffer[i];
      Serial.print(recvGlobal.recvBuffer[i],DEC);
      Serial.print(F(" "));
    }
    delay(1000);
    myReceiver.enableIRIn();
  }
}
else if (s == 'b') {
    mySender.send(command, len, 36);
    Serial.println();
    Serial.println(F("Signal sent"));
    delay(5000);
    myReceiver.enableIRIn();
  }
}
