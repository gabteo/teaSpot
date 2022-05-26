// Mapeamento dos pinos
#define SIG  7 // pino digital
#define S0   8
#define S1   9
#define S2   10
#define S3   11
// o pino EN do mux vai no GND


int mix[4]={S3,S2,S1,S0};


byte ci[16][4]={
  {0,0,0,0},
  {0,0,0,1},
  {0,0,1,0},
  {0,0,1,1},
  {0,1,0,0},
  {0,1,0,1},
  {0,1,1,0},
  {0,1,1,1},
  {1,0,0,0},
  {1,0,0,1},
  {1,0,1,0},
  {1,0,1,1},   
  {1,1,0,0},
  {1,1,0,1},
  {1,1,1,0},
  {1,1,1,1},
};

void setup(){
Serial.begin(9600);

pinMode(S0,OUTPUT);
pinMode(S1,OUTPUT);
pinMode(S2,OUTPUT);
pinMode(S3,OUTPUT);

pinMode(SIG,OUTPUT);
}


void loop(){

// Coloquei 3 efeitos de luz para testes e Basta você escolher qual o efeito desejado e apagar ou "comentar" os outros.

 
 
  // efeito 1 SEQUENCIAL 
  for (int x = 0; x <= 15; x++){
     digitalWrite(SIG,HIGH); // liga o pino SIG
     multiplex(x); // vai para a rotina de escrita no mux  

     Serial.print("Pino ");
     Serial.print(x);
     Serial.println(" Ligado");
     delay(100);   
  }
  Serial.println(""); 
  // fim do efeito 1
  
   
   
   // efeito 2 SEQUENCIAL PISCA PISCA
   for (int x = 0; x <= 15; x++){

     multiplex(x); // vai para a rotina de escrita no mux  

     Serial.print("Pino ");
     Serial.print(x);
     Serial.print(" piscando");

     for(int x=0; x <= 4; x++){
      digitalWrite(SIG,HIGH); // liga o pino SIG
      delay(50);
      digitalWrite(SIG,LOW); // desliga o pino SIG
      delay(50);
}

     Serial.println("");   
   }
 // fim do efeito 2




 // efeito 3 ALEATORIO
  digitalWrite(SIG,HIGH);      // liga o pino SIG
  int sorteio = random(0,16);  // Sorteia um valor entre 0 e 15 para acender um led
  Serial.print("O led sorteado foi: ");
  Serial.println(sorteio);
  multiplex(sorteio);          // vai para a rotina de escrita no mux  
  delay(random(100));          // sorteia um valor entre 0 e 99 para acender o proximo led
 Serial.println("");  
 // fim do efeito 3




}// fim do loop







//Função de escrita dos valores
void multiplex(int valor){

   for (int y = 0; y <= 3; y++){
      digitalWrite (mix[y], ci[valor][y]);   
    }
    
 
}// fim do multiplex
