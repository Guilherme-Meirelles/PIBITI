#include <IRremote.hpp>
#define RECORD_GAP_MICROS_WARNING_THRESHOLD 8000  // Silencia o aviso
#define IR_RECEIVE_PIN 2
#define IR_SEND_PIN 3
#define botao 11
uint16_t endereco;
uint16_t comando;
int dado_bruto;
int bits;
int val;
decode_type_t protocolo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Controle inicializado");
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);
  pinMode(botao, INPUT);
}

void loop() {
  /*
  val = digitalRead(botao);
  if (val == HIGH){
    if (IrReceiver.decode()){
    recvSinal();
  }
  }
else {
  enviarSinal();
    delay(2000);
  }
  
*/
val = digitalRead(botao);
if (IrReceiver.decode()){
    recvSinal();
}
while (val == HIGH){
enviarSinal();
val = digitalRead(botao);
}


  
    
    
  


  
}
void recvSinal(){
  // Imprime informações cruas sobre o sinal recebido
  if (!((protocolo == IrReceiver.decodedIRData.protocol && comando == IrReceiver.decodedIRData.command && endereco == IrReceiver.decodedIRData.address ))){

    Serial.println("\n--- Sinal IR Detectado ---");
    Serial.print("Protocolo: ");
    Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
    Serial.print("Endereço: 0x");
    Serial.println(IrReceiver.decodedIRData.address, HEX);
    Serial.print("Comando: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    Serial.print("Dados brutos (32 bits): 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    // Imprime resumo formatado
    IrReceiver.printIRResultShort(&Serial);
      endereco = IrReceiver.decodedIRData.address;
      comando = IrReceiver.decodedIRData.command;
      dado_bruto = IrReceiver.decodedIRData.decodedRawData;
      protocolo = IrReceiver.decodedIRData.protocol;
      bits = IrReceiver.decodedIRData.numberOfBits;
    }

    // Prepara para receber o próximo sinal
    IrReceiver.resume();
}

void enviarSinal(){
  
  switch(protocolo){
    case NEC :
    Serial.print("Enviando NEC -> Endereço: 0x");
    Serial.print(endereco, HEX);
    Serial.print(", Comando: 0x");
    Serial.println(comando, HEX);
    IrSender.sendNEC(endereco, comando, 0);
    delay(100);
    break;
    case SAMSUNG :
      Serial.print("Enviando SAMSUNG -> Endereço: 0x");
      Serial.print(endereco, HEX);
      Serial.print(", Comando: 0x");
      Serial.println(comando, HEX);
      IrSender.sendSamsung(endereco, comando, 0);
    break;
    case SONY :
      Serial.print("Enviando SONY -> Endereço: 0x");
      Serial.print(endereco, HEX);
      Serial.print(", Comando: 0x");
      Serial.println(comando, HEX);
      IrSender.sendSony(endereco, comando, 0);
    break;
    default:
    Serial.println("Sinal não enviado");
    break;
  }
}
