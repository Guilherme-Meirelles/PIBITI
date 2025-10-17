#include "IRLibAll.h" 
IRrecv myReceiver(2); //create receiver and pass pin number 
IRdecode myDecoder;   //create decoder 
void setup() { 
Serial.begin(9600); 
delay(2000); while (!Serial); //delay for Leonardo 
myReceiver.enableIRIn(); // Start the receiver 
Serial.println(F("Ready to receive IR signals")); 
} 
void loop() { 
//Continue looping until you get a complete signal received 
if (myReceiver.getResults()) { 
myDecoder.decode();           
//Decode it 
myDecoder.dumpResults(false);  //the results of a Now print results. 
myReceiver.enableIRIn();      
delay(1000);
//Restart receiver 
} 
} 
