#include <IRremote.h>

const int receiverPin = 11;
const int led = 10;
IRrecv irReceiver(receiverPin);

int comando = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irReceiver.enableIRIn();
  pinMode(led, INPUT);
  Serial.println("Iniciou: ");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irReceiver.decode()) {
    comando = irReceiver.decodedIRData.command;
    Serial.println(comando);
    irReceiver.resume();
    
    switch(comando){
      case 88 :
        digitalWrite(led, HIGH);
        break;
      case 89: 
        digitalWrite(led, LOW);
      default:
        break;
    }
  }

}
