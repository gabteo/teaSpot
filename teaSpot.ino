#include <Servo.h>
#include <AccelStepper.h>

#define CHA sabor1
#define TODDY sabor2
#define ACUCAR sabor4
#define ADOCANTE sabor5


//-----------defines temporarios----------//
//adaptar para MUX!!!

//portas dos reles
#define ESTEIRA A0
#define BOMBA A2 
#define EBULIDOR A3
#define MEXEDOR A4
#define MEXEDOR_SERVO 9

const int servoMexedorInicial = 90;
const int servoMexedorFinal = 180;

//portas IR
//int IRsabores[5] = {5,NULL,NULL,NULL,NULL};
//pinos de controle MUX1
const int s0mux1;
const int s1mux1;
const int s2mux1;
const int s3mux1;
//sinal MUX1
const int mux1;

//portas motor aquecedor
const int dirElevador = 4;
const int stepElevador = 7;
const int enElevador = 6;
bool isElevadorEnabled = 0;

//portas motor arquimedes demonstraão
const int dirArquimedes = 10;
const int stepArquimedes = 11;
const int enArquimedes = 12;
bool isArquimedesEnabled = 0;


//portas motor elevador
AccelStepper motorElevador(1, stepElevador, dirElevador);
bool elevadorDescendo = 1;
//vide setup

//portas motor sabor1
const int sabor1Pin1A = 11;
const int sabor1Pin1B = 10;
const int sabor1Pin2A = 9;
const int sabor1Pin2B = 8;

//------------fim defines temporarios -----------//
Servo dispenserLeft, dispenserRight;
const int tempoElevador = 4500;

struct pedido {
  bool aquecer;
  bool sabores[5];
  //bool sabor1, sabor2, sabor3, sabor4, sabor5;  
};
/*const int maxLeft = 200;
const int minLeft = 90;
const int maxRight = 55;
const int minRight = 21*0.9;*/

//posicoes dos servos do dispenser
const int maxLeft = 143*1.1;
const int minLeft = 100;
const int maxRight = 55;
const int minRight = 21*0.9;
void esticarDispenser();
//------------------FUNÇÕES DE SETUP------------------------//
void setupCupDispenser() {
  dispenserLeft.attach(2);
  dispenserRight.attach(3);
  esticarDispenser();
  Serial.println("Setup cup dipenser: ok");
  return;
}

void setupEsteira() {
  pinMode(ESTEIRA, OUTPUT);
  digitalWrite(ESTEIRA, HIGH);
  Serial.println("Setup esteira: ok");
}

void setupElevador(int velocidadeElevador = 800, int aceleracaoElevador = 500) {
  //sentidoHorarioElevador = 0;
  //AccelStepper motorElevador la nos defines
  //int velocidadeElevador = 800; 
  //int aceleracaoElevador = 500;

  motorElevador.setMaxSpeed(velocidadeElevador);
  motorElevador.setSpeed(velocidadeElevador);
  motorElevador.setAcceleration(aceleracaoElevador);
  digitalWrite(enElevador, "HIGH");
  Serial.println("Setup elevador: ok");
  }

void setupBomba() {
  pinMode(BOMBA, OUTPUT);
  digitalWrite(BOMBA, HIGH); 
  Serial.println("Setup bomba: ok");
}

void setupSabores() {
  
}
Servo servoMexedor;
void setupMexedor() {
  pinMode(MEXEDOR, OUTPUT);
  digitalWrite(MEXEDOR, LOW);
  servoMexedor.attach(9);
  servoMexedor.write(90);
}

void setupEbulidor() {
  pinMode(EBULIDOR, OUTPUT);
  digitalWrite(EBULIDOR, HIGH);
  Serial.println("Setup ebulidor: ok");  
}

void setutMUX1() {
  pinMode(s0mux1, OUTPUT); 
  pinMode(s1mux1, OUTPUT); 
  pinMode(s2mux1, OUTPUT); 
  pinMode(s3mux1, OUTPUT); 

  pinMode(mux1, OUTPUT); 

  digitalWrite(s0mux1, LOW);
  digitalWrite(s1mux1, LOW);
  digitalWrite(s2mux1, LOW);
  digitalWrite(s3mux1, LOW);

  digitalWrite(mux1, LOW); 
  
  Serial.println("Setup MUX1: ok");
}


//-----------------FUNCOES TECNICAS/MUX-------------------------//
int readMux1(int channel){
  int controlPin[] = {s0mux1, s1mux1, s2mux1, s3mux1};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = digitalRead(mux1);

  //return the value
  return val;
}

//------------------FUNÇÕES DE LIGAR/DESLIGAR------------------------//
void esticarDispenser() {
  for (float posL = maxLeft, posR = minRight; posL>minLeft || posR<maxRight; posL -= 1.26, posR +=1) {
    if(posL > minLeft)
      dispenserLeft.write(posL);
    if(posR < maxRight)
      dispenserRight.write(posR);    
  } //termina em Lmin Rmax
  //dispenserLeft.write(100);
  //dispenserRight.write(55);  
}
void encolherDispenser() {
  for (float posL = minLeft, posR = maxRight; posL<maxLeft || posR>minRight; posL += 1.26, posR -=1) {
    if(posL < maxLeft)
      dispenserLeft.write(posL);
    if(posR > minRight)
      dispenserRight.write(posR);    
  } //termina em Lmax Rmin
  //dispenserLeft.write(143);
  //dispenserRight.write(21);
}

bool dispensarCopo() {
  esticarDispenser();
  delay(1500);
  encolherDispenser();
  Serial.println("copo dispensado");
  return 1;
}

bool ligarEsteira() {
  digitalWrite(ESTEIRA, LOW);
}

bool desligarEsteira() {
  digitalWrite(ESTEIRA, HIGH);
}

void descerElevador() {
  //descer elevador: sentido horário
  elevadorDescendo = 1;  
  isElevadorEnabled = 1;
  digitalWrite(enElevador, LOW);
  //delay(tempoElevador);
  //parar elevador
}

void subirElevador() {
  elevadorDescendo = 0;  
  isElevadorEnabled = 1;
  digitalWrite(enElevador, LOW);
  
  //subir elevador
  //delay(tempoElevador);
  //parar elevador
}

void ligarBomba() {
  digitalWrite(BOMBA, LOW);
  Serial.println("Bomba ligada");
  }
void desligarBomba() {
  digitalWrite(BOMBA, HIGH);
  Serial.println("Bomba desligada");
}

void ligarEbulidor() {
  digitalWrite(EBULIDOR, LOW);
  Serial.println("Ebulidor ligado");
}
void desligarEbulidor() {
  digitalWrite(EBULIDOR, HIGH);
  Serial.println("Ebulidor desligado");
}

void abaixarColher() {

}
void levantarColher() {

}

void ligarMexedor() {
  digitalWrite(MEXEDOR, HIGH);  
}
void desligarMexedor() {
  digitalWrite(MEXEDOR, LOW);  
}


void encherCopo() {
  ligarBomba();
  delay(3000);
  //if nivel de agua
  desligarBomba();
  return;
  }


void aquecer() {
  ligarEbulidor();
  delay(4000);

  //if temperatura
  desligarEbulidor();
  return;
}

void mexer() {
  abaixarColher();
  ligarMexedor();
  
  delay(60000*1);  //1 minutos
  
  desligarMexedor();
  levantarColher();  
  return;
  }
  


void error() {
  Serial.print("Um erro aconteceu!");
}

void dispensarSabor(int sabor) {
  //ligar motor
  //tempo e desliga
}

bool prepararPedido(pedido pedido) {
 /* // Estação: dispenser de copos
  dispensarCopo();
  delay(3000);

  if(digitalRead(IRdispenser) == LOW)
    ligarEsteira();
  else {
    error();
    return 0;
  }


  //estação: água e aquecimento
  if(digitalRead(IRaquecer) == LOW)
    desligarEsteira();

  descerElevador();
  delay(2000);

  encherCopo();
  delay(3000);

  if(pedido.aquecer)
    aquecer();
  delay(2000);

  subirElevador();
  delay(2000);

  //estação: sabores
  if(digitalRead(IRaquecer) == LOW)
    ligarEsteira();
  else {
    error();
    return 0;
  }
  for(int i = 0; i<5; i++) {
    if(pedido.sabores[i]) {
        while(digitalRead(IRsabores[i]) == HIGH) {
       
      desligarEsteira();
      dispensarSabor(i);   
    }
  }   
 
  delay(3000);

  
  //estação: mexedor
//  if(digitalRead(IRsabores[0]) == LOW)
    ligarEsteira();
  //else {
    //error();
    //return 0;
 // }

  if(digitalRead(IRmexedor) == LOW)
    desligarEsteira();

  mexer();

  
  return 1;*/
}

void modoTeste() {
  int comando = 0;
    
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  switch(comando) {
    //elevador    
    case 'h': { 
      subirElevador();
      Serial.println("SUBINDO");
      break;           
      }    
    case 'j': {     
      descerElevador();
      Serial.println("descendo");
      break;      
    }
    case 'k': {
      isElevadorEnabled = 0;
      motorElevador.moveTo(0);
      digitalWrite(enElevador, HIGH);
      Serial.println("parando elevador");
      break;
    }
    //aquecedor
    case 'q': {     
      ligarEbulidor();
      Serial.println("Aquecendo");
      break;      
    }
    case 'w': {     
      desligarEbulidor();
      Serial.println("Ebulidor desligado");
      break;      
    }
      
    
    //esteira
    case 'e':
      ligarEsteira();
      break;
    case 'r':
      desligarEsteira();
      break;  

    //bomba    
    case 'b':
      ligarBomba();
      break;
    case 'n':
      desligarBomba();
      break;
      

    //sabor1    
    case 'd':
      dispensarSabor(1);
      break;
    
    //dispenser de copos    
    case 'c':
      dispensarCopo();
      break; 
     case 'z':
      esticarDispenser();
      break;     
     case 'x':
      encolherDispenser();
      break;  

    //mexedor    
    case '1':
      abaixarColher();
      break;
    case 's':
      levantarColher();
      break;
    case 'a':
      ligarMexedor();
      break;      
    case 'p':
      desligarMexedor();
      break;
    }
  if(elevadorDescendo && isElevadorEnabled) {
    motorElevador.moveTo(-10000);
  } else if(!elevadorDescendo && isElevadorEnabled) {
    motorElevador.moveTo(10000);  
  } else if(!isElevadorEnabled) {
    
  }
  motorElevador.run();
}

int pedidosNaFila = 0;

void setup() {
  Serial.begin(9600);

  setupEsteira();
  setupCupDispenser();
  setupElevador();
  setupSabores();
  //setupMexedor();
  setupBomba();
  setupEbulidor();
  
 
}

void loop() {
  
  /*pedido pedidoTeste;
  pedidoTeste.aquecer = 1;
  pedidoTeste.sabores[0] = 1;
  for (int i = 0; i < 5; i++)  
    pedidoTeste.sabores[i] = 0;
  */
  //funções de teste
  
  //desligarBomba();  
  //delay(3000);
  modoTeste();
  
  //prepararPedido(pedidoTeste);
  //exit(0);
}
