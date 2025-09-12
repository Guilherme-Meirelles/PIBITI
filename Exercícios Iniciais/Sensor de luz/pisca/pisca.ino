const int LED = 13;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  val =  10 * analogRead(0);
  digitalWrite(LED, HIGH);
  delay(val);
  digitalWrite(LED, LOW);
  delay(val);

}
