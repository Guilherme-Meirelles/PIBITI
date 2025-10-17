char c;
char old_c;
char aux;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
    c = Serial.read();

    if (c == '\n'|| c == '\r'){
      aux = old_c;
    }
    old_c = c;
  }
  Serial.println(aux);
  delay(1000);
}
