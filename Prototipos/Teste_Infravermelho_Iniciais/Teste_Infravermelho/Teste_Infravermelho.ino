#include <IRremote.h>


#define RECV_PIN 2

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{

  Serial.begin(9600); // Establish serial communication
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Foi:");
}

void loop() {
  if (irrecv.decode(results)) {
      Serial.println(results.value, HEX);
      Serial.println(results.decode_type);
      Serial.println(results.bits);
      irrecv.resume(); // Receive the next value
      delay(1000);
      
  }
}
