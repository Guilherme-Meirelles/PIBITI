
#define PinInterrupt 3
#define Led 4

volatile bool estado = false;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(Led, OUTPUT);
  
  pinMode(PinInterrupt, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PinInterrupt), inverte_led, RISING);

  

  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void inverte_led(){

  static unsigned long debounce = millis();

if (millis() - debounce > 1000){
  if (estado){
    digitalWrite(Led, HIGH);
    estado = false;
    printf("Ligou");
  }
  else {
    digitalWrite(Led, LOW);
    estado = true;
    printf("Desligou");
  }
}
}