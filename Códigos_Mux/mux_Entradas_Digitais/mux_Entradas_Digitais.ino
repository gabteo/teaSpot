
// mapeamento dos pinos
#define SIG  7 // pino digital
#define S0   8
#define S1   9
#define S2   10
#define S3   11
// O pino EN do mux vai no GND para Habilitar

int mix[4]={S3,S2,S1,S0};
boolean botoes[16];

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

pinMode(SIG,INPUT_PULLUP);

}

void loop(){
  
 
multiplex(); // Lê os pinos do Mux

// Adicionar os demais botoes

   if(botoes[0] == LOW){
    Serial.println("Botao 0 pressionado!");
    }

  if(botoes[7] == LOW){
    Serial.println("Botao 7 pressionado!");
    }

  if(botoes[15] == LOW){
    Serial.println("Botao 15 pressionado!");
    }



}// fim do loop







// Rotina de varredura dos pinos
void multiplex(){

  for(int x=0; x <= 15; x++){
   for (int y = 0; y <= 3; y++){
      digitalWrite (mix[y], ci[x][y]);   
    }
   botoes[x]=digitalRead(SIG);
  }
 
}// fim da rotina




  
