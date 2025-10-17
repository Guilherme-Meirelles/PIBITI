//pinos usados
#define LedInterrupt 4
#define Led 5
#define InterruptPin 3

//estado atual do led, o atributo volatile pois ele vai ser usado na função ISR
volatile bool estado = false;

void setup() {
  //pinos do led como saida
  pinMode(LedInterrupt, OUTPUT);
  pinMode(Led, OUTPUT);
  
  //pino da interrupção como INPUT
  pinMode(InterruptPin, INPUT);
  
  //configura interrupção por qualquer mudança no pino 3
  attachInterrupt(digitalPinToInterrupt(InterruptPin), inverte_led, CHANGE);

}

void loop() {
  //pisca o led
  digitalWrite(Led, HIGH);
  delay(1000);
  digitalWrite(Led, LOW);
  delay(1000);
}

//ISR que inverte o valor de LedInterrupt
void inverte_led(){
  if(estado){
    digitalWrite(LedInterrupt, HIGH);
    estado = false;
  }else{
    digitalWrite(LedInterrupt, LOW);
    estado = true;
  }
}