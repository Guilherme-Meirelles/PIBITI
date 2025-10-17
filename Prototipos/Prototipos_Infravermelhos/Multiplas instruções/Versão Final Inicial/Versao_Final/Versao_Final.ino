#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>
#include <dht11.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <LowPower.h>

LiquidCrystal lcd(6, 7, 10, 11, 12, 13);

 // Altere para o pino desejado

IRrecvPCI myReceiver(2);

uint8_t suspensao = 3;
uint8_t ligar_desligar = A0;
uint8_t auxiliar = A1;
uint8_t descer = A2;
uint8_t subir = A3;


uint8_t val_atual;
uint8_t val_antigo;
uint8_t estado_recepcao_emissao = 0;
uint8_t estado_configuracao_recursao = 0;
uint8_t flag_on_off = 1;
uint8_t flag_temperatura = 3;
uint8_t temperatura = 18;
uint8_t n_comando = 1;

uint8_t cont_comandos = 0;
const uint16_t sentinela PROGMEM = 1023;
uint8_t tamanho_comando = EEPROM.read(sentinela);
uint8_t max_comandos = 1021 / tamanho_comando;
uint16_t* Buffer_de_sinal;

int8_t temperatura_ambiente = -100;
uint8_t pinoDHT11 = 4;
dht11 DHT11;

uint64_t tempo;
uint32_t tempo_para_suspender = EEPROM.read(1021) * 60000;
uint8_t tempo_para_desligar = EEPROM.read(1022);


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
}

void saveCommandToEEPROM(uint8_t cmdIndex) {
  uint16_t address = cmdIndex * tamanho_comando * sizeof(uint8_t);
  for(uint8_t i = 0; i < tamanho_comando; i++) {
    EEPROM.update(address, (uint8_t)(tempBuffer[i] / 50));
    address += sizeof(uint8_t);
  }
}

void loadCommandFromEEPROM(uint8_t cmdIndex) {
  uint16_t address = cmdIndex * tamanho_comando * sizeof(uint8_t);
  uint16_t variavel_auxiliar;
  for(uint8_t i = 0; i < tamanho_comando; i++) {
    variavel_auxiliar = EEPROM.read(address);
    tempBuffer[i] = variavel_auxiliar * 50;
    address += sizeof(uint8_t);
  }
}


void reconfigurar_memoria(){

  cont_comandos = 0;
  tamanho_instrucao = recvGlobal.recvLength - 1;
  max_comandos = 1021 / tamanho_comando;
  Buffer_de_sinal = (uint16_t *)realloc(Buffer_de_sinal, sizeof(tamanho_comando * sizeof(uint16_t)));
  EEPROM.update(sentinela, tamanho_comando);
  mostrar_lcd("Reconfig", "Memoria");
  delay(1000);
  char maximo_comando_string[8];
  sprintf(maximo_comando_string,"Max. %d", max_comandos);
  mostrar_lcd(maximo_comando_string, "Comandos");
  delay(3000);
}

void receiver() {
  
  if (myReceiver.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (tamanho_comando != recvGlobal.recvLength - 1){
        reconfigurar_memoria();
      }

      for (uint8_t i = 1; i <= tamanho_comando; i++) {
        Buffer_de_sinal[i-1] = recvGlobal.recvBuffer[i];
      }
      tempo = millis();

      if (cont_comandos < max_comandos) {
      
        saveCommandToEEPROM(cont_comandos);
        cont_comandos++;

        char cont_comando_string[8];
        sprintf(cont_comando_string, "Salvo %d", cont_comandos);
      
        
        mostrar_lcd("Comando", cont_comando_string);
      
      } 
      else {
        
        mostrar_lcd("Memoria", "Lotada");
      }
      
    }
    myReceiver.enableIRIn();
  }
  ativar_mudanca_para_recepcao_ou_emissao();
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

void mudanca_para_recepcao_ou_emissao(){

    delay(10);
    estado_recepcao_emissao = 1 - estado_recepcao_emissao;

    if (estado_recepcao_emissao == 0){
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

void ativar_mudanca_para_recepcao_ou_emissao(){

  delay(50);
  if (digitalRead(ligar_desligar) == HIGH){
    bool a = true;
    tempo = millis();
    while (digitalRead(ligar_desligar) == HIGH && a){
      delay(100);
      if (millis() - tempo > 2500){
        a = false;
        mudanca_para_recepcao_ou_emissao();
      }
    }
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

  val_antigo = val;
  val = digitalRead(ligar_desligar);
  delay(50);
  
  if ((val == HIGH) && (val_antigo == LOW)){
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
  ativar_mudanca_para_recepcao_ou_emissao();  
    
  }
  
  else if (digitalRead(auxiliar) == HIGH){
    delay(10);
    send_r(2);
    mostrar_lcd("Comando", "Auxiliar");
    bool a = true;
    while (digitalRead(auxiliar) == HIGH && a){
      delay(100);
      if (millis() - tempo > 2500){
        a = false;
        estado_configuracao_recursao = 1;
        
      }
    }
  }
  

  else if (digitalRead(descer) == HIGH){
    delay(10);
    
    if (flag_temperatura == 3){
      tempo = millis();
      bool a = true;
      while (digitalRead(descer) == HIGH && a){
        delay(100);
        if (millis() - tempo > 2500){
          a = false;
          estado_configuracao_recursao = 2;
          estado_recepcao_emissao = 2;
          mostrar_lcd("Modo", "Recursi.");
          delay(2000);
          mostrar_lcd("Recursi.", "Ativado");
          delay(2000);
          mostrar_lcd("Comando", "N. 1");
        }
      }
    }
    else if (flag_temperatura > 3){
      flag_temperatura --;
      temperatura --;
    }
    if (estado_configuracao_recursao != 2){
      comando_alterar_temperatura();
    }  
        
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

void modo_configuracao(){

  mostrar_lcd("Modo de", "Config.");
  delay(2500);
  mostrar_lcd("T. para", "Suspend.");
  delay(2500);
  uint8_t tempo_suspensao = tempo_para_suspender / 60000;
  uint16_t tempo_desligar = tempo_para_desligar * 30;
  char string_tempo_suspensao[8];
  char string_tempo_desligar[8];
  sprintf(string_tempo_suspensao, "T. %dm.", tempo_suspensao);
  mostrar_lcd("Definir", string_tempo_suspensao);
  while (digitalRead(auxiliar) == LOW){
    delay(100);
    if (digitalRead(subir) == HIGH && tempo_suspensao < 60){
        tempo_suspensao ++;
        sprintf(string_tempo_suspensao, "T. %dm.", tempo_suspensao);
        mostrar_lcd("Definir", string_tempo_suspensao);
      }
    else if (digitalRead(descer) == HIGH && tempo_suspensao > 1){
      tempo_suspensao --;
      sprintf(string_tempo_suspensao, "T. %dm.", tempo_suspensao);
      mostrar_lcd("Definir", string_tempo_suspensao);
    }
  }
  tempo_para_suspender = tempo_suspensao * 60000;
  EEPROM.update(1021, tempo_suspensao);
  mostrar_lcd("Tempo", "Definido");
  delay(2000);
  mostrar_lcd("Tempo p.", "definir");
  delay(1000);
  mostrar_lcd("desligar", "ar - cd");
  delay(1000);
  sprintf(string_tempo_desligar, "T. %dm.", tempo_desligar);
  mostrar_lcd("Definir", string_tempo_desligar);
  while (digitalRead(auxiliar) == LOW){
    delay(100);
    if (digitalRead(subir) == HIGH && tempo_desligar < 600){
        tempo_desligar = tempo_desligar + 30;
        sprintf(string_tempo_desligar, "T. %dm.", tempo_desligar);
        mostrar_lcd("Definir", string_tempo_desligar);
      }
    else if (digitalRead(descer) == HIGH && tempo_desligar > 0){
      tempo_desligar = tempo_desligar - 30;
      sprintf(string_tempo_suspensao, "T. %dm.", tempo_desligar);
      mostrar_lcd("Definir", string_tempo_suspensao);
    }
  }
  tempo_para_desligar = (uint8_t)(tempo_desligar / 30);
  EEPROM.update(1022, tempo_para_desligar);
  mostrar_lcd("Tempo", "Definido");
  delay(2000);
  mostrar_lcd("Saindo", "Config.");
  tempo = millis();
  delay(2000);
  mostrar_lcd("Emissao", "Ativada");
  estado_configuracao_recursao = 0;
  estado_recepcao_emissao = 0;
  
}

void modo_recursivo(){
  delay(50);
  char string_n_comando[8];
  if (digitalRead(descer) == HIGH && n_comando > 1){
    n_comando --;
    sprintf(string_n_comando, "N. %d", n_comando);
    mostrar_lcd("Comando", string_n_comando);
    tempo = millis();
  }
  else if (digitalRead(subir) == HIGH && n_comando < max_command){
    n_comando ++;
    sprintf(string_n_comando, "N. %d", n_comando);
    mostrar_lcd("Comando", string_n_comando);
    tempo =  millis();
  }
  else if (digitalRead(auxiliar) == HIGH){
    send_r(n_comando - 1);
    mostrar_lcd("Comando", "Acionado");
    tempo = millis();
  }
  else if (millis() - tempo > 5000 && millis() - tempo < 5500){
    sprintf(string_n_comando, "N. %d", n_comando);
    mostrar_lcd("Comando", string_n_comando);
    
  }
  else if (digitalRead(ligar_desligar) == HIGH){
    bool a = true;
    tempo = millis();
    while (digitalRead(ligar_desligar) == HIGH && a){
      delay(100);
      if (millis() - tempo > 3000){
        a = false;
        estado_recepcao_emissao = 0;
        estado_configuracao_recursao = 0;
        mostrar_lcd("M. Recv.", "Desativado");
        delay(2000);
        mostrar_lcd("Emissao", "Ativada");
      }
    }
  }
}



volatile bool verificador = true;

void durmaBem() {
  
  attachInterrupt(digitalPinToInterrupt(suspensao), acorda, LOW);
  myReceiver.disableIRIn();
  Serial.println("Indo mimir");
  mostrar_lcd("Controle", "Hibern.");
  delay(2000);
  lcd.clear();

  uint16_t contador = 0;
  verificador = true;
  uint16_t cont_para_desligar = tempo_para_desligar * 225;
  // Ativa a interrupção no pino 3
  

  while (contador < cont_para_desligar && verificador) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    contador++;
  }
  delay(100);
  
  // Se ainda não foi acordado pela interrupção
  if (verificador && cont_para_desligar > 0) {
    
    send_r(1);
    delay(100);
    // Dorme até ser acordado por interrupção
    
  }
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  // Saiu do sono
  
  while (digitalRead(suspensao) == LOW){
    delay(100);
  }
  detachInterrupt(digitalPinToInterrupt(suspensao));
  tempo = millis();
  mostrar_lcd("Controle", "Acordan.");
  Serial.println("Interrupção ativada, acordando arduino");
  estado_recepcao_emissao = 0;
  delay(2000);
  mostrar_lcd("Emissao", "Ativada");
}
  
  
void acorda() {
  verificador = false; // acorda o loop
}
  

void setup() {

  Serial.begin(9600);
  myReceiver.setFrameTimeout(100000);
  pinMode(suspensao, INPUT_PULLUP);
  
  tempBuffer = (uint16_t *)malloc(len * sizeof(uint16_t));
  tempo = millis();
  lcd.begin(8,2);
  lcd.clear();
  mostrar_lcd("Controle", "Ligado");                     
  Serial.println(F("Ready to receive IR signals"));
  delay(1000);
}

void loop() {

  
  if (estado_recepcao_emissao == 1){
    receiver();
    if ()
  }
  else if (estado_recepcao_emissao == 0){
    
    funcionamento_botoes_do_controle();
    if (estado_configuracao_recursao == 1){
      modo_configuracao();  
  }

    uint32_t tempo_temperatura = millis() - tempo;
    DHT11.read(pinoDHT11);
    if ((tempo_temperatura > 10000 && tempo_temperatura < 10500) || 
      (DHT11.temperature != temperatura_ambiente && (tempo_temperatura % 10000 > 0 && tempo_temperatura % 10000 < 500))){
      mostrar_lcd_temp();
    }
  }
  else if (estado_configuracao_recursao == 2){
    modo_recursivo();
  }

  

  
  
  if (digitalRead(suspensao) == LOW ){
    while (digitalRead(suspensao) == LOW){
      delay(100);
    }
    delay(100);
    durmaBem();
  }

  
  
  if ((millis() - tempo) > tempo_para_suspender){
    durmaBem();
  }
  
  
}













