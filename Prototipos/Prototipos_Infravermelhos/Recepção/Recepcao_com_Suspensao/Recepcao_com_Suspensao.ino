#include <IRLibRecvPCI.h> 
// INCLUSÃO DE BIBLIOTECAS
#include <avr/sleep.h>

// DEFINIÇÕES DE PINOS
#define pinBot 3
IRrecvPCI myReceiver(2);//pin number for the receiver
unsigned long cont;

void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));
  pinMode(pinBot, INPUT_PULLUP);
  myReceiver.setFrameTimeout(100000);
  cont = millis();
}

void loop() {
  //Continue looping until you get a complete signal received
  uint8_t a = digitalRead(pinBot);
  if (myReceiver.getResults()) { 
    if (recvGlobal.recvLength  > 20){
    Serial.println(F("Do a cut-and-paste of the following lines into the "));
    Serial.println(F("designated location in rawSend.ino"));
    Serial.print(F("\n#define RAW_DATA_LEN "));
    Serial.println(recvGlobal.recvLength,DEC);
    Serial.print(F("uint16_t rawData[RAW_DATA_LEN]={\n\t"));
    for(bufIndex_t i=1;i<recvGlobal.recvLength;i++) {
      Serial.print(recvGlobal.recvBuffer[i],DEC);
      Serial.print(F(", "));
      if( (i % 8)==0) Serial.print(F("\n\t"));
    }
    Serial.println(F("1000};"));//Add arbitrary trailing space
    
    cont = millis();   //Restart receiver
  }
  myReceiver.enableIRIn();
  }
  if (a == LOW ){
    while (digitalRead(pinBot) == LOW){
    }
    durmaBem();
  }
  if (millis() - cont > 10000){
    durmaBem();
  }
  
}


void durmaBem() {
  myReceiver.disableIRIn();
  
  Serial.println("Indo mimir");
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(pinBot), acorda, LOW);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}

void acorda() {
  
 
  detachInterrupt( digitalPinToInterrupt(pinBot));
  sleep_disable();
  cont = millis();
  while (digitalRead(pinBot) == LOW){
    delay(50);
    }
  Serial.println("Interrupção ativada, acordando arduino");
  myReceiver.enableIRIn();

}