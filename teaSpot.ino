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
#define EBULIDOR A2
#define MEXEDOR A3

//portas IR
const int IRdispenser = 3;
const int IRaquecer = 4;
//int IRsabores[5] = {5,NULL,NULL,NULL,NULL};
const int IRmexedor = 6;
const int IRfinalizar = 7;



//portas motor aquecedor
const int dirElevador = 4;
const int stepElevador = 7;
int enElevador = 10;
bool isElevadorEnabled = 0;

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

int velocidadeElevador; 
int aceleracaoElevador;
int sentidoHorarioElevador;


struct pedido {
  bool aquecer;
  bool sabores[5];
  //bool sabor1, sabor2, sabor3, sabor4, sabor5;  
};
/*const int maxLeft = 200;
const int minLeft = 90;
const int maxRight = 55;
const int minRight = 21*0.9;*/


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
  Serial.println("setup dipenser");
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

  motorElevador.setMaxSpeed(800);
  motorElevador.setSpeed(800);
  motorElevador.setAcceleration(500);
  Serial.println("Elevador configurado.");
  }

void setupBomba() {
  pinMode(BOMBA, OUTPUT);
  digitalWrite(BOMBA, HIGH); 
  Serial.println("setup bomba");
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
  digitalWrite(ESTEIRA, HIGH);
}

bool desligarEsteira() {
  digitalWrite(ESTEIRA, LOW);
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
  Serial.println("bomba ligada");
  }
void desligarBomba() {
  digitalWrite(BOMBA, HIGH);
  Serial.println("bomba desligada");
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
  /*if(digitalRead(IRaquecer) == LOW)
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
 
  delay(3000);*/

  
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

  
  return 1;
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
  if(elevadorDescendo && isElevadorEnabled) {
    motorElevador.moveTo(10000);
  } else if(!elevadorDescendo && isElevadorEnabled) {
    motorElevador.moveTo(-10000);  
  } else if(!isElevadorEnabled) {
    
  }
  motorElevador.run();
}

int pedidosNaFila = 0;

void setup() {
  Serial.begin(9600);
  //configura servos do dispenser de copos
  setupEsteira();
  setupCupDispenser();
  setupElevador();
  setupSabores();
  setupMexedor();
  setupBomba();
  
 
}

void loop() {
  // put your main code here, to run repeatedly:
  pedido pedidoTeste;
  pedidoTeste.aquecer = 1;
  pedidoTeste.sabores[0] = 1;
  for (int i = 0; i < 5; i++)  
    pedidoTeste.sabores[i] = 0;

  //funções de teste
  
  //desligarBomba();  
  //delay(3000);
  modoTeste();
  
  //prepararPedido(pedidoTeste);
  //exit(0);
}
