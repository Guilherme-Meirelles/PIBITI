int pino = 3;
String valor;
int tamanho;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(pino, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
  
    valor = Serial.readStringUntil('\n');
  
  Serial.println(valor);
  if (valor == "Ligar"){
    digitalWrite(pino, HIGH);
  }
  if (valor == "Desligar"){
    digitalWrite(pino, LOW);
  }
}
  }