int vermelho = 11;
int verde = 10;
int azul = 9;

void setup(){
  pinMode(vermelho, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);
}

void loop(){

  for (int i = 0; i < 256; i++){
    analogWrite(vermelho, i);

  
  
    for (int j = 0; j < 256; j++){
    analogWrite(verde, j);

    
    for (int h = 0; h < 256; h++){
    analogWrite(azul, h);
    }
    }
  }
    
  
}