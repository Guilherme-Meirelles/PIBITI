#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <LowPower.h>
LiquidCrystal lcd(6, 7, 10, 11, 12, 13);

char* comando[]= {"Recepcao", "Emissao", "Ativada", "Desativ.", "Controle", "Hiberna.", "Acordan.", 
  "Sinal", "Captado", "Enviado", "Aparelho", "Desliga.", "Ligando", "Temp.", "Modo", "Ar", "Tv"};

void mover_lcd(int posicao, char* palavra){
  
 
  lcd.print(palavra);
  Serial.println("Entrei aqui");
  Serial.println(strlen(palavra));
  uint8_t tamanho = strlen(palavra);
  for (size_t i = 0; i < tamanho - 7; i++){
    Serial.println("Entrou aqui");
    lcd.clear();
    
    lcd.print(palavra + i);
    delay(1000);
    
  }
    }
    


void mostrar_lcd(int palavra1, int palavra2){
  lcd.clear();
  char* a = comando[palavra1];
  char* b = comando[palavra2];
  /*
   uint8_t tamanho_a = strlen(a);
   uint8_t tamanho_b = strlen(b);
   //Serial.println(strlen(a));
  if (tamanho_a > 8){
   
     for (size_t i = 0; i < tamanho_a - 7; i++){
      lcd.clear();
      Serial.println("Entrou aqui");
       lcd.setCursor(0, 0);
      lcd.print(a + i);
      lcd.setCursor(0, 1);
      lcd.print(b);
    
      delay(1500);
      
    
  }

    
  }
  else if (tamanho_b > 8){
    for (size_t i = 0; i < tamanho_b - 7; i++){
      lcd.clear();
      Serial.println("Entrou aqui");
      lcd.setCursor(0, 0);
      lcd.print(a);
      lcd.setCursor(0, 1);
      lcd.print(b + i);
    
      delay(1000);
      
    
  }

  }
  else {
    */
    lcd.setCursor(0, 0);
    lcd.print(a);
    lcd.setCursor(0, 1);
    lcd.print(b);
  //}
  
}


uint8_t suspensao = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(suspensao, INPUT_PULLUP);
  lcd.begin(8,2);
  Serial.begin(9600);
}
 int j;
void acorda(){
  detachInterrupt(digitalPinToInterrupt(suspensao));
  j = 0;
  Serial.println("Acordou");
  delay(30000);
}

void loop() {
  
  delay(50);
  if (digitalRead(suspensao) == LOW){
    
    while (digitalRead(suspensao) == LOW){
      delay(50);
    }
  Serial.println("Dorme");
  delay(50);
  attachInterrupt(digitalPinToInterrupt(suspensao), acorda, LOW);
  int i = 0;
  j = 1;
  lcd.noDisplay();
  while (i < 60 && j == 1){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    Serial.println("Ainda estou aqui");
    delay(100);
    i ++;
  }
  
  delay(1000);
  }
  /*
  mostrar_lcd(0,1);
  
  delay(3000);
  mostrar_lcd(1,2);
 
  delay(3000);
  mostrar_lcd(0,3);
  //Serial.println(strlen(b));
  delay(3000);
  lcd.clear();
  // put your main code here, to run repeatedly:
  */
  /*
  mostrar_lcd(0, 2);
  delay(5000);
  lcd.clear();
  mostrar_lcd(0, 3);
  delay(5000);
  lcd.clear();
  mostrar_lcd(4, 5);
  delay(5000);
  lcd.clear();
  mostrar_lcd(4, 6);
  delay(5000);
  lcd.clear();
  */
  /*
  a = comando[0];
  b = comando[1];
  lcd.setCursor(0, 0);
  lcd.print(a);
  lcd.setCursor(0, 1);
  lcd.print(b);
  delay(3000);
  lcd.clear();
  a= comando[1];
  b = comando[3];
  lcd.setCursor(0, 0);
  lcd.print(a);
  lcd.setCursor(0, 1);
  lcd.print(b);

  //mostrar_lcd(1,2);
  delay(3000);
  lcd.clear();
  */
}
