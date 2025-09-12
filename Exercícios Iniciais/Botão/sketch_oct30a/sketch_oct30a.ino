int ledPin = 13;
int buttonPin = 7;
bool ledState = false;
bool buttonState = false;
bool lastButtonState = false;

void setup(){
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop(){
  buttonState = digitalRead(buttonPin);
  delay(50);
  if (!buttonState){
    ledState = !ledState;
    delay(50);
  }
  if (ledState){
    digitalWrite(ledPin, HIGH);
  }
  else{
    digitalWrite(ledPin, LOW);
  }
}