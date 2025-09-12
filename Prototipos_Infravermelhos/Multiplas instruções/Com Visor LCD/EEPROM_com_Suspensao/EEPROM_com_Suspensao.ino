#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>
#include <dht11.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <LowPower.h>

LiquidCrystal lcd(6, 7, 10, 11, 12, 13);

char* comando[]= {"Recepcao", "Emissao", "Ativada", "Desativ.", "Controle", "Hiberna.", "Acordan.", 
  "Sinal", "Captado", "Enviado", "Aparelho", "Desliga.", "Ligando", "Temp.", "Modo", "Ar", "Tv", "Comando", "Salvo", "Carregado", "Ambiente"};


 // Altere para o pino desejado


IRrecvPCI myReceiver(2);
uint8_t suspensao = 3;
uint8_t recepcao = 4;
uint8_t subir = A0;
uint8_t descer = A1;
uint8_t ligar_desligar = A2;

uint8_t val;
uint8_t val_antigo;
uint8_t estado = 0;
uint8_t flag_on_off = 1;
uint8_t flag_temperatura = 2;
uint8_t temperatura = 18;
unsigned long tempo;

uint8_t cont_comandos = 0;
const uint16_t sentinela PROGMEM = 1023;
uint8_t len = EEPROM.read(sentinela);
uint8_t max_command = 1023 / (len * 2);
uint16_t* tempBuffer;

uint8_t temperaturaAmbiente = 1;
dht11 DHT11;

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (len != recvGlobal.recvLength - 1){
        cont_comandos = 0;
        len = recvGlobal.recvLength - 1;
        max_command = 1023 / (len*(sizeof(uint16_t)));
        tempBuffer = (uint16_t *)realloc(tempBuffer, sizeof(len * sizeof(uint16_t)));
        EEPROM.update(sentinela, len);
        //Serial.println(F("Reconfigurando Memoria"));
        mostrar_lcd(14, 15);
        delay(1000);
      }

    tempo = millis();
    for (uint8_t i = 1; i <= len; i++) {
      tempBuffer[i-1] = recvGlobal.recvBuffer[i];
    }
    
    if (cont_comandos < max_command) {
      saveCommandToEEPROM(cont_comandos, len);
      cont_comandos++;
      EEPROM.update(sentinela, len);
      // Serial.println(F("Comando Salvo"));
      mostrar_lcd(17, 18);
    } else {
      //Serial.println(F("Memoria cheia"));
      mostrar_lcd(15, 16);
    }
    }
    myReceiver.enableIRIn();
    tempo = millis();
  }

}

void send_r(uint8_t cmdIndex) {
  loadCommandFromEEPROM(cmdIndex, len);
  IRsendRaw mySender;
  mySender.send(tempBuffer, len, 36);
  //Serial.print(F("Signal sent, command: "));
  //Serial.println(cmdIndex);
  delay(2000);
  
}

void setup() {
  Serial.begin(9600);
  myReceiver.setFrameTimeout(100000);
  pinMode(suspensao, INPUT_PULLUP);
  pinMode(recepcao,INPUT);
  pinMode(ligar_desligar, INPUT);
  tempBuffer = (uint16_t *)malloc(len * sizeof(uint16_t));
  tempo = millis();
  mostrar_lcd(0,2);                     
  //Serial.println(F("Ready to receive IR signals"));
}

void loop() {

  val = digitalRead(recepcao);
  delay(50);
  if ((val == HIGH) && (val_antigo == LOW)){
    estado = 1 - estado;
    delay(10);
    tempo = millis();
    if (estado == 0){
      //Serial.println(F("Not Receiving"));
      myReceiver.disableIRIn();
      mostrar_lcd(0,3);
      delay(2000);
      mostrar_lcd(1,2);
    }
    else {
      myReceiver.enableIRIn();
      cont_comandos = 0;
      //Serial.println(F("Ready to receive"));
      mostrar_lcd(1,3);
      delay(2000);
      mostrar_lcd(0,2);
    }
  }
  val_antigo = val;

  if (estado == 1){
    receiver();
  }
  if (estado == 0){
    delay(50);
    if (digitalRead(ligar_desligar) == HIGH){
      delay(10);
      flag_on_off = 1 - flag_on_off;
      send_r(flag_on_off);
      if (flag_on_off == 0){
        mostrar_lcd(10,12);
      }
      else{
        mostrar_lcd(10,11);
      }
    }
    else if (digitalRead(descer) == HIGH){
      delay(10);
      if (flag_temperatura > 2){
        flag_temperatura --;
        temperatura --;
      }
      send_r(flag_temperatura);
      mostrar_lcd(13, 20);
    }
    else if (digitalRead(subir) == HIGH){
      delay(10);
      if (flag_temperatura < max_command - 1){
        flag_temperatura ++;
        temperatura ++;
      }
      send_r(flag_temperatura);
      mostrar_lcd(13, 19);
     }
  }

    if (digitalRead(suspensao) == LOW ){
      while (digitalRead(suspensao) == LOW){
    }
    delay(50);
    durmaBem();
  }
  if ((millis() - tempo > 10000) && (millis() - tempo < 15000)){
    mostrar_lcd_temp();
  }
  if (millis() - tempo > 30000){

  }
  /*
  */
  /*
  if (millis() - tempo > 60000){
    durmaBem();
  }
*/
}



void mostrar_lcd(int palavra1, int palavra2){
  lcd.clear();
  char* a = comando[palavra1];
  char* b = comando[palavra2];

    lcd.setCursor(0, 0);
    lcd.print(a);
    lcd.setCursor(0, 1);
    lcd.print(b);
 
}

void mostrar_lcd_temp(){
  char* temp = comando[13];
  char temp_val[8];
  sprintf(temp_val, "%d °C", temperatura);
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print(temp_val);
}
void mostrar_lcd_temp_ambiente(){
  char* temp = comando[13];
  char temp_val[8];
  sprintf(temp_val, "%d °C", DHT11.temperature);
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print(temp_val);
}


void saveCommandToEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.put(address, tempBuffer[i]);
    address += sizeof(uint16_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint16_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.get(address, tempBuffer[i]);
    address += sizeof(uint16_t);
  }
}

uint8_t verificador;
void durmaBem() {

  
  //Serial.println(F("Indo mimir"));
  mostrar_lcd(4,5);
  delay(2000);
  lcd.clear();
  

  attachInterrupt(digitalPinToInterrupt(suspensao), acorda, LOW);
  uint16_t contador = 0;
  verificador = 1;
  while (contador < 450 && verificador){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    contador ++;
  }
  send_r(1);
  if (verificador){
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
}

void acorda() {
  
 
  detachInterrupt(digitalPinToInterrupt(suspensao));
  verificador = 0;
  
  while (digitalRead(suspensao) == LOW){
    delay(50);
    }
  delay(50);
  //Serial.println(F("Interrupção ativada, acordando arduino"));
  tempo = millis();
  
  mostrar_lcd(4,6);
  
}

