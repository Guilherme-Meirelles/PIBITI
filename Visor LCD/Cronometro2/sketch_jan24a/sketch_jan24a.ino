// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 5, 0, 1, 2, 3);

String palavra = "Hello World";
int min = 0;
int seg = 0;
int a = 0;
int b = sizeof(palavra) - 1;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(8, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();     // Limpa o display
 
 lcd.clear();     // Limpa o display
 lcd.setCursor(7,0);
 lcd.print("Beginning");
 
 
 for(int x=0; x<8; x++) {
  lcd.scrollDisplayLeft();  // Rola o display 16 vezes para a esquerda
  delay(1000);
  
  lcd.setCursor(0,1);
  lcd.print(min/10, DEC);
  lcd.print(min%10,DEC);
  lcd.print(":");
  
  lcd.print(seg/10, DEC);
  lcd.print(seg%10, DEC);
  lcd.setCursor(2,1);
  delay(500);
  lcd.print(" ");
  delay(500);
  seg = (seg + 1) % 60; 
  if (seg == 0){
    min = (min + 1) % 60;

  }}
 }

  
