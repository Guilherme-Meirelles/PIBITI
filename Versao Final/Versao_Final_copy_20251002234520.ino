/*Bibliotecas utilizadas no projeto*/
#include <IRLibRecvPCI.h>
#include <IRLibSendBase.h>
#include <IRLib_HashRaw.h>
#include <EEPROM.h>
#include <dht11.h>
#include <LiquidCrystal.h>
#include <LowPower.h>

/*Objetos globais utilizados*/

LiquidCrystal lcd(6, 7, 10, 11, 12, 13); // Pinos utilizados no visor lcd
IRrecvPCI pino_receptor(2); //Pino de recepção de sinal IR
dht11 DHT11; //entidade do sensor de temperatura

/*Variaveis globais utilizadas*/

//Pinos utilizados pelos botões do controle
const uint8_t suspensao = 3;
const uint8_t ligar_desligar = A0;
const uint8_t auxiliar = A1;
const uint8_t descer = A2;
const uint8_t subir = A3;


//Variaveis utilizadas na emissão de IR
uint8_t val_ligar_desligar;
uint8_t val_ligar_desligar_antigo;
uint8_t estado_recepcao_emissao = 0;
uint8_t estado_configuracao_recursao = 0;
uint8_t flag_on_off = 1;
uint8_t flag_temperatura = 3;
uint8_t temperatura = 18;
uint8_t n_comando = 1;

//Variaveis utilizadas na recepção de IR
uint8_t cont_comandos = 0;
const uint16_t sentinela = 1023;
uint8_t tamanho_comando = EEPROM.read(sentinela);
uint8_t max_comandos = 1021 / tamanho_comando;
uint16_t* Buffer_de_sinal;

//Variaveis utilizadas para medir temperatura
const uint8_t pinoDHT11 = 4;
int8_t temperatura_ambiente = -100;

//Variaveis utilizadas para medir o tempo
uint64_t tempo;
uint32_t tempo_para_suspender = EEPROM.read(1021) * 60000;
uint8_t tempo_para_desligar = EEPROM.read(1022);

/*Funções utilizadas*/

//Escreve strings no visor LCD
void mostrar_lcd(char* palavra1, char* palavra2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(palavra1);
  lcd.setCursor(0,1);
  lcd.print(palavra2);
  delay(500);
}

//Mostra a temperatura ambiente no lcd
void mostrar_lcd_temp(){
  lcd.clear();
  temperatura_ambiente = DHT11.temperature;
  char temperatura_ambiente_string[8];
  sprintf(temperatura_ambiente_string, "T. %d C", temperatura_ambiente);
  mostrar_lcd("Ambiente", temperatura_ambiente_string);
}

//Salva comandos de sinais IR na EEPROM
void salvar_comando_na_EEPROM(uint8_t cmdIndex) {
  uint16_t endereco = cmdIndex * tamanho_comando * sizeof(uint8_t);
  for(uint8_t i = 0; i < tamanho_comando; i++) {
    EEPROM.update(endereco, (uint8_t)(Buffer_de_sinal[i] / 50));
    endereco += sizeof(uint8_t);
  }
}

//Carrega o comando de um sinal IR da EEPROM para a RAM
void carregar_comando_da_EEPROM(uint8_t cmdIndex) {
  uint16_t endereco = cmdIndex * tamanho_comando * sizeof(uint8_t);
  uint16_t variavel_auxiliar;
  for(uint8_t i = 0; i < tamanho_comando; i++) {
    variavel_auxiliar = EEPROM.read(endereco);
    Buffer_de_sinal[i] = variavel_auxiliar * 50;
    endereco += sizeof(uint8_t);
  }
}

//Reconfigura o tamanho de salvar e carregar os registros na EEPROM
void reconfigurar_memoria(){

  cont_comandos = 0;
  tamanho_comando = recvGlobal.recvLength - 1;
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

//Responsável por analisar o sinal IR recevido e salva-lo na EEPROM
void recepcao() {
  
  if (pino_receptor.getResults()) {
    if (recvGlobal.recvLength > 15){
      if (tamanho_comando != recvGlobal.recvLength - 1){
        reconfigurar_memoria();
      }

      for (uint8_t i = 1; i <= tamanho_comando; i++) {
        Buffer_de_sinal[i-1] = recvGlobal.recvBuffer[i];
      }
      tempo = millis();

      if (cont_comandos < max_comandos) {
      
        salvar_comando_na_EEPROM(cont_comandos);
        cont_comandos++;

        char cont_comando_string[8];
        sprintf(cont_comando_string, "Salvo %d", cont_comandos);
      
        
        mostrar_lcd("Comando", cont_comando_string);
      
      } 
      else {
        
        mostrar_lcd("Memoria", "Lotada");
      }
      
    }
    pino_receptor.enableIRIn();
  }
}



// Responsável por emitir sinais IR
void enviar_sinalIR(uint8_t cmdIndex) {
  carregar_comando_da_EEPROM(cmdIndex);
  IRsendRaw mySender; // Entidade utilizadas para emitir sinais IR
  mySender.send(Buffer_de_sinal, tamanho_comando, 36);
  delay(100);
  tempo = millis();
}
//Muda o controle do estado de emissão de IR para recepção de IR e vice-versa.
void mudanca_para_recepcao_ou_emissao(){

    delay(10);
    estado_recepcao_emissao = 1 - estado_recepcao_emissao;

    if (estado_recepcao_emissao == 0){
      pino_receptor.disableIRIn();
      mostrar_lcd("Recepcao", "Desativ.");
      delay(2000);
      tempo = millis();
      mostrar_lcd("Emissao", "Ativada");
      pino_receptor.disableIRIn();
    }
    else {
      pino_receptor.enableIRIn();
      cont_comandos = 0;
      mostrar_lcd("Emissao", "Desativ.");
      delay(2000);
      tempo = millis();
      mostrar_lcd("Recepcao", "Ativada");
      pino_receptor.enableIRIn();
    }
}
//Ativa a mudança de estado pressionando por 3s o botão de ligar_desligar do controle
void ativar_mudanca_para_recepcao_ou_emissao(){

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

//Comando de alterar a temperatura do ar
void comando_alterar_temperatura(){

  char temperatura_string[8];
  sprintf(temperatura_string, "T. %d C", temperatura);
  enviar_sinalIR(flag_temperatura);
  mostrar_lcd("Comando", temperatura_string);
      
}



//Responsável pelo funcionamento do botões do controle, emitindo uma resposta ao pressioná-los
void funcionamento_botoes_do_controle(){

  val_ligar_desligar = val_ligar_desligar_antigo;
  val_ligar_desligar = digitalRead(ligar_desligar);
  delay(50);
  
  if ((val_ligar_desligar == HIGH) && (val_ligar_desligar_antigo == LOW)){
    
    flag_on_off = 1 - flag_on_off;
    enviar_sinalIR(flag_on_off);
    if (flag_on_off == 0){
      mostrar_lcd("Comando", "Desligar");
      
    }
    else{
      mostrar_lcd("Comando", "Ligar");
    }
  }
  else if (digitalRead(auxiliar) == HIGH){
    
    enviar_sinalIR(2);
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

    if (flag_temperatura > 3){
      flag_temperatura --;
      temperatura --;
    }
    comando_alterar_temperatura();
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
  }
  else if (digitalRead(subir) == HIGH){
    
    if (flag_temperatura < max_comandos - 1){
      flag_temperatura ++;
      temperatura ++;
    }
    comando_alterar_temperatura();
    }   
}

//Ativa o modo de configuração do controle, altera o tempo para suspender o controle e enviar o sinal de desligar o ar, enquanto está suspenso.

void modo_configuracao(){

  mostrar_lcd("Modo de", "Config.");
  delay(2500);
  mostrar_lcd("T. para", "Suspend.");
  delay(2500);
  uint8_t tempo_suspensao = tempo_para_suspender / 60000;
  uint16_t tempo_desligar = tempo_para_desligar * 30;
  char string_tempo_suspensao[8];
  char string_tempo_desligar[8];
  sprintf(string_tempo_suspensao, "t. %dm.", tempo_suspensao);
  mostrar_lcd("Definir", string_tempo_suspensao);
  while (digitalRead(auxiliar) == LOW){
    delay(100);
    if (digitalRead(subir) == HIGH && tempo_suspensao < 60){
        tempo_suspensao ++;
        sprintf(string_tempo_suspensao, "t. %dm.", tempo_suspensao);
        mostrar_lcd("Definir", string_tempo_suspensao);
      }
    else if (digitalRead(descer) == HIGH && tempo_suspensao > 1){
      tempo_suspensao --;
      sprintf(string_tempo_suspensao, "t. %dm.", tempo_suspensao);
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
  sprintf(string_tempo_desligar, "t. %dm.", tempo_desligar);
  mostrar_lcd("Definir", string_tempo_desligar);
  while (digitalRead(auxiliar) == LOW){
    delay(100);
    if (digitalRead(subir) == HIGH && tempo_desligar < 600){
        tempo_desligar = tempo_desligar + 30;
        sprintf(string_tempo_desligar, "t. %dm.", tempo_desligar);
        mostrar_lcd("Definir", string_tempo_desligar);
      }
    else if (digitalRead(descer) == HIGH && tempo_desligar > 0){
      tempo_desligar = tempo_desligar - 30;
      sprintf(string_tempo_suspensao, "t. %dm.", tempo_desligar);
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
// Modo iterativo do contorle: escolhe-se os comandos clicando os botões de subir e descer, sendo emitidos ao clicar no botão auxiliar
void modo_recursivo(){
  delay(50);
  char string_n_comando[8];
  if (digitalRead(descer) == HIGH && n_comando > 1){
    n_comando --;
    sprintf(string_n_comando, "N. %d", n_comando);
    mostrar_lcd("Comando", string_n_comando);
    tempo = millis();
  }
  else if (digitalRead(subir) == HIGH && n_comando < max_comandos){
    n_comando ++;
    sprintf(string_n_comando, "N. %d", n_comando);
    mostrar_lcd("Comando", string_n_comando);
    tempo =  millis();
  }
  else if (digitalRead(auxiliar) == HIGH){
    enviar_sinalIR(n_comando - 1);
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

//Responsável por suspender o funcionamento do controle
void suspender() {
  
  attachInterrupt(digitalPinToInterrupt(suspensao), acordar_controle, LOW);
  pino_receptor.disableIRIn();
  
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
  // Se ainda não foi acordado pela interrupção
  if (verificador && cont_para_desligar > 0) {
    enviar_sinalIR(0);
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
  
  estado_recepcao_emissao = 0;
  delay(2000);
  mostrar_lcd("Emissao", "Ativada");
}
  
//Responsável por ativar o controle
void acordar_controle() {
  verificador = false; // acorda o loop
}
  
//Instruções executadas ao ligar o controle
void setup() {

  Serial.begin(9600);
  pino_receptor.setFrameTimeout(100000);
  pinMode(suspensao, INPUT_PULLUP);
  
  Buffer_de_sinal = (uint16_t *)malloc(tamanho_comando * sizeof(uint16_t));
  tempo = millis();
  lcd.begin(8,2);
  lcd.clear();
  mostrar_lcd("Controle", "Ligado");                     
  
  delay(1000);
}

//Principais instruções do controle, executadas em loop
void loop() {

  
  if (estado_recepcao_emissao == 1){
    recepcao();
    delay(50);
    if (digitalRead(ligar_desligar) == HIGH){
      ativar_mudanca_para_recepcao_ou_emissao();
    }
  }
  else if (estado_recepcao_emissao == 0){
    
    funcionamento_botoes_do_controle();
    if (digitalRead(ligar_desligar) == HIGH){
      ativar_mudanca_para_recepcao_ou_emissao();
    }
    else if (estado_configuracao_recursao == 1){
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
    
    suspender();
  }

  
  if ((millis() - tempo) > tempo_para_suspender){
    suspender();
  }
  
}













