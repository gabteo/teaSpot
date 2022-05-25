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
#define BOMBA A1 
#define EBULIDOR A2;
#define MEXEDOR A3;

//portas IR
const int IRdispenser = 3;
const int IRaquecer = 4;
int IRsabores[5] = {5,NULL,NULL,NULL,NULL};
const int IRmexedor = 6;
const int IRfinalizar = 7;



//portas motor aquecedor
const int dirElevador;
const int stepElevador;
//const int enElevador;

//portas motor elevador
AccelStepper motorElevador(1, 5, 4);
//vide setup

//portas motor sabor1
const int sabor1Pin1A = 11;
const int sabor1Pin1B = 10;
const int sabor1Pin2A = 9;
const int sabor1Pin2B = 8;

//------------fim defines temporarios -----------//
Servo dispenserLeft, dispenserRight;
const int tempoElevador = 4500;

int velocidadeElevador; 
int aceleracaoElevador;
int sentidoHorarioElevador;


struct pedido {
  bool aquecer;
  bool sabores[5];
  //bool sabor1, sabor2, sabor3, sabor4, sabor5;  
};


//------------------FUNÇÕES DE SETUP------------------------//
void setupCupDispenser() {
  dispenserLeft.attach(2);
  dispenserRight.attach(3);
  return;
}

void setupEsteira() {
  pinMode(ESTEIRA, OUTPUT);
  digitalWrite(ESTEIRA, LOW);
}

void setupElevador() {
  //velocidadeElevador = 100; 
  //aceleracaoElevador = 100;
  //sentidoHorarioElevador = 0;
  //AccelStepper motorElevador la nos defines

  motorElevador.setMaxSpeed(300);
  motorElevador.setSpeed(300);
  motorElevador.setAcceleration(300);
  Serial.println("Elevador configurado.");
  }

void setupBomba() {
  pinMode(BOMBA, OUTPUT);
  digitalWrite(BOMBA, LOW);
}

void setupSabores() {
  
}
void setupMexedor() {
  pinMode(MEXEDOR, OUTPUT);
  digitalWrite(MEXEDOR, LOW);
}

void setupEbulidor() {
  pinMode(EBULIDOR, OUTPUT);
  digitalWrite(EBULIDOR, LOW);  
}


//------------------FUNÇÕES DE LIGAR/DESLIGAR------------------------//

bool dispensarCopo() {
  //estica (solta o copo)
  for (float posL = 143, posR = 21; posL>100 || posR<55; posL -= 1.26, posR +=1) {
    if(posL>100)
      dispenserLeft.write(posL);
    if(posR<55)
      dispenserRight.write(posR);    
  }
  //dispenserLeft.write(100);
  //dispenserRight.write(55);
  
  delay(1500);

  //encolhe novamente
  for (float posL = 100, posR = 55; posL<143 || posR>21; posL += 1.26, posR -=1) {
    if(posL<143)
      dispenserLeft.write(posL);
    if(posR>21)
      dispenserRight.write(posR);    
  }
  //dispenserLeft.write(143);
  //dispenserRight.write(21);

  return 1;
}

bool ligarEsteira() {
  digitalWrite(ESTEIRA, HIGH);
}

bool desligarEsteira() {
  digitalWrite(ESTEIRA, LOW);
}

void descerElevador() {
  //descer elevador
  delay(tempoElevador);
  //parar elevador
}

void subirElevador() {
  //subir elevador
  delay(tempoElevador);
  //parar elevador
}

void ligarBomba() {
  digitalWrite(BOMBA, HIGH);
}
void desligarBomba() {
  digitalWrite(BOMBA, LOW);
}

void ligarEbulidor() {
  digitalWrite(EBULIDOR, HIGH);
}
void desligarEbulidor() {
  digitalWrite(EBULIDOR, LOW);
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
  // Estação: dispenser de copos
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
        
      }
      desligarEsteira();
      dispensarSabor(i);   
    }
  }   
      
  delay(3000);

  
  //estação: mexedor
  if(digitalRead(IRsabores[0]) == LOW)
    ligarEsteira();
  else {
    error();
    return 0;
  }

  if(digitalRead(IRmexedor) == LOW)
    desligarEsteira();

  mexer();

  
  return 1;
}

void modoTeste() {
  int comando = 0;
    
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  switch(comando) {
    //elevador    
    case 'h':
      subirElevador();
      break;      
    case 'j':
      descerElevador();
      break;
    
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

    //mexedor    
    case 'q':
      abaixarColher();
      break;
    case 'w':
      levantarColher();
      break;
    case 'a':
      ligarMexedor();
      break;      
    case 's':
      desligarMexedor();
      break;
    }
}

int pedidosNaFila = 0

void setup() {
  //configura servos do dispenser de copos
  setupEsteira();
  setupCupDispenser();
  setupElevador();
  setupSabores();
  setupMexedor();
  setupBomba();
  Serial.begin(9600);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  pedido pedidoTeste;
  pedidoTeste.aquecer = 1;
  pedidoTeste.sabores[0] = 1;
  for (int i = 0; i < 5; i++)  
    pedidoTeste.sabores[i] = 0;

  //funções de teste
  ligarBomba();
  delay(3000);
  desligarBomba();  
  delay(3000);
  //modoTeste();
  
  //prepararPedido(pedidoTeste);
  //exit(0);
}
