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

 // Altere para o pino desejado

IRrecvPCI myReceiver(2);

uint8_t suspensao = 3;
uint8_t recepcao = 4;
uint8_t subir = A1;
uint8_t descer = A2;
uint8_t ligar_desligar = A0;

uint8_t val;
uint8_t val_antigo;
uint8_t estado = 0;
uint8_t flag_on_off = 1;
uint8_t flag_temperatura = 2;
uint8_t temperatura = 18;

uint8_t cont_comandos = 0;
const uint16_t sentinela PROGMEM = 1023;
uint8_t len = EEPROM.read(sentinela);
uint8_t max_command = 1023 / len;
uint16_t* tempBuffer;

int16_t temperatura_ambiente = -100;
uint8_t pinoDHT11 = 5;
dht11 DHT11;

uint64_t tempo;


void mostrar_lcd(char* palavra1, char* palavra2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(palavra1);
  lcd.setCursor(0,1);
  lcd.print(palavra2);
  delay(500);
}

void mostrar_lcd_temp(){
  lcd.clear();
  temperatura_ambiente = DHT11.temperature;
  char temperatura_ambiente_string[8];
  sprintf(temperatura_ambiente_string, "T. %d C", temperatura_ambiente);
  mostrar_lcd("Ambiente", temperatura_ambiente_string);
  delay(500);
}

void saveCommandToEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint8_t);
  for(uint8_t i = 0; i < cmdLength; i++) {
    EEPROM.update(address, (uint8_t)(tempBuffer[i] / 50));
    address += sizeof(uint8_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex, uint8_t cmdLength) {
  uint16_t address = cmdIndex * len * sizeof(uint8_t);
  uint16_t variavel_auxiliar;
  for(uint8_t i = 0; i < cmdLength; i++) {
    variavel_auxiliar = EEPROM.read(address);
    tempBuffer[i] = variavel_auxiliar * 50;
    address += sizeof(uint8_t);
  }
}


void reconfigurar_memoria(){

  cont_comandos = 0;
  len = recvGlobal.recvLength - 1;
  max_command = 1023 / len;
  tempBuffer = (uint16_t *)realloc(tempBuffer, sizeof(len * sizeof(uint16_t)));
  EEPROM.update(sentinela, len);
  Serial.println(F("Reconfigurando Memoria"));
  mostrar_lcd("Reconfig", "Memoria");
  delay(1000);
  char numero_maximo_comando[8];
  sprintf(numero_maximo_comando,"Max. %d", max_command);
  mostrar_lcd(numero_maximo_comando, "Comandos");
  delay(2000);
}

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (len != recvGlobal.recvLength - 1){
        reconfigurar_memoria();
      }

      for (uint8_t i = 1; i <= len; i++) {
      tempBuffer[i-1] = recvGlobal.recvBuffer[i];
      }
      tempo = millis();
      if (cont_comandos < max_command) {
      
        saveCommandToEEPROM(cont_comandos, len);
        cont_comandos++;

        char cont_comando_string[8];
        sprintf(cont_comando_string, "Salvo %d", cont_comandos);
      
        Serial.println(F("Comando Salvo"));
        mostrar_lcd("Comando", cont_comando_string);
      
      } 
      else {
        Serial.println(F("Memoria cheia"));
        mostrar_lcd("Memoria", "Lotada");
      }
      
    }
    myReceiver.enableIRIn();
  }
}




void send_r(uint8_t cmdIndex) {
  loadCommandFromEEPROM(cmdIndex, len);
  IRsendRaw mySender;
  mySender.send(tempBuffer, len, 36);
  Serial.print(F("Signal sent, command: "));
  Serial.println(cmdIndex);
  delay(100);
  tempo = millis();
}

void alterar_entre_recepcao_e_emissao(){

    delay(10);
    estado = 1 - estado;

    if (estado == 0){
      Serial.println(F("Not Receiving"));
      myReceiver.disableIRIn();
      mostrar_lcd("Recepcao", "Desativ.");
      delay(2000);
      tempo = millis();
      mostrar_lcd("Emissao", "Ativada");
    }
    else {
      myReceiver.enableIRIn();
      cont_comandos = 0;
      Serial.println(F("Ready to receive"));
      mostrar_lcd("Emissao", "Desativ.");
      delay(2000);
      tempo = millis();
      mostrar_lcd("Recepcao", "Ativada");
    }
    
    
}

void comando_alterar_temperatura(){

  char temperatura_string[8];
      sprintf(temperatura_string, "T. %d C", temperatura);
      send_r(flag_temperatura);
      mostrar_lcd("Comando", temperatura_string);
      Serial.println(F("Temperatura Reduzida"));
      
}

void funcionamento_botoes_do_controle(){

   if (digitalRead(ligar_desligar) == HIGH){
      delay(10);
      flag_on_off = 1 - flag_on_off;
      send_r(flag_on_off);
      if (flag_on_off == 0){
        mostrar_lcd("Comando", "Desligar");
        Serial.println(F("Desligou"));
      }
      else{
        mostrar_lcd("Comando", "Ligar");
        Serial.println(F("Ligou"));
      }
    }
    
    else if (digitalRead(descer) == HIGH){
      delay(10);
      if (flag_temperatura > 2){
        flag_temperatura --;
        temperatura --;
      }
      comando_alterar_temperatura();
      
      
    }
    else if (digitalRead(subir) == HIGH){
      delay(10);
      if (flag_temperatura < max_command - 1){
        flag_temperatura ++;
        temperatura ++;
      }
      comando_alterar_temperatura();
     }
  }


volatile bool verificador = true;

void durmaBem() {

  attachInterrupt(digitalPinToInterrupt(suspensao), acorda, LOW);

  Serial.println("Indo mimir");
  mostrar_lcd("Controle", "Hibern.");
  delay(2000);
  lcd.clear();

  uint16_t contador = 0;
  verificador = true;

  // Ativa a interrupção no pino 3
  

  while (contador < 450 && verificador) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    contador++;
  }
  delay(100);
  
  // Se ainda não foi acordado pela interrupção
  if (verificador) {
    
    send_r(1);
    delay(100);
    // Dorme até ser acordado por interrupção
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  // Saiu do sono
  
  while (digitalRead(suspensao) == LOW){
    delay(100);
  }
  detachInterrupt(digitalPinToInterrupt(suspensao));
  tempo = millis();
  mostrar_lcd("Controle", "Acordan.");
  Serial.println("Interrupção ativada, acordando arduino");
  
  
}
  
  
void acorda() {
  verificador = false; // acorda o loop
}
  

void setup() {

  Serial.begin(9600);
  myReceiver.setFrameTimeout(100000);
  pinMode(suspensao, INPUT_PULLUP);
  pinMode(recepcao, INPUT);
  
  tempBuffer = (uint16_t *)malloc(len * sizeof(uint16_t));
  tempo = millis();
  lcd.begin(8,2);
  lcd.clear();
  mostrar_lcd("Controle", "Ligado");                     
  Serial.println(F("Ready to receive IR signals"));
  delay(1000);
}

void loop() {

  val = digitalRead(recepcao);
  delay(50);
  if ((val == HIGH) && (val_antigo == LOW)){
    alterar_entre_recepcao_e_emissao();
  }
  val_antigo = val;
  if (estado == 1){
    receiver();
  }
  else if (estado == 0){
    
    funcionamento_botoes_do_controle();
    uint32_t tempo_temperatura = millis() - tempo;
    DHT11.read(pinoDHT11);
    if ((tempo_temperatura > 10000 && tempo_temperatura < 10500) || 
      (DHT11.temperature != temperatura_ambiente && (tempo_temperatura % 10000 > 0 && tempo_temperatura % 10000 < 500))){
      mostrar_lcd_temp();
  }
  
  }
  if (digitalRead(suspensao) == LOW ){
    while (digitalRead(suspensao) == LOW){
      delay(100);
    }
    delay(100);
    durmaBem();
  }

  
  
  if ((millis() - tempo) > 60000){
    durmaBem();
  }
  
}













