// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 5, 0, 1, 2, 3);

char palavra[] = {"Bem vindo ao RoboDin! "};
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
  lcd.setCursor(0,0);
  lcd.print(palavra[a % b]);
  lcd.print(palavra[(a + 1) % b]);
  lcd.print(palavra[(a + 2) % b]);
  lcd.print(palavra[(a + 3) % b]);
  lcd.print(palavra[(a + 4) % b]);
  lcd.print(palavra[(a + 5) % b]);
  lcd.print(palavra[(a + 6) % b]);
  lcd.print(palavra[(a + 7) % b]);
  a = (a + 1) % b;
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
