#include <Servo.h>
#include <AccelStepper.h>
#include <Stepper.h>


#define CHA sabor1
#define TODDY sabor2
#define ACUCAR sabor4
#define ADOCANTE sabor5

//-------------DEFINES MUX---------------//

//MUX1
#define BUZZER 0
#define EN_S1 1
#define EN_S5 2
#define EN_S2 3
#define EN_S3 4
#define EN_S4 5
#define ELEVADOR 6
#define COPO_L 7
#define ESTEIRA 8

#define MUX_BUZZER mux1
#define MUX_EN_S1 mux1
#define MUX_EN_S5 mux1
#define MUX_EN_S2 mux1
#define MUX_EN_S3 mux1
#define MUX_EN_S4 mux1
#define MUX_ELEVADOR mux1
#define MUX_COPO_L mux1
#define MUX_ESTEIRA mux1


//MUX2
#define FIM_CURSO 0
#define BOMBA 1
#define EBULIDOR 2
#define MOTOR_MIX 3

#define MUX_FIM_CURSO mux2
#define MUX_BOMBA mux2
#define MUX_EBULIDOR mux2
#define MUX_MOTOR_MIX mux2

#define IR0 4
#define IR1 5
#define IR2 6
#define IR3 7
#define IR4 8
#define IR5 9
#define IR6 10
#define MUX_IR0 mux2
#define MUX_IR1 mux2
#define MUX_IR2 mux2
#define MUX_IR3 mux2
#define MUX_IR4 mux2
#define MUX_IR5 mux2
#define MUX_IR6 mux2

#define COPO_R 11
#define SERVO_MIX 12
#define MUX_COPO_R mux2
#define MUX_SERVO_MIX mux2
//13 nao conectado
//#define SONDA 14
//15 nao conectado




//MUX NO ARDUINO------------------------
#define SIG_MUX1 12
#define SIG_MUX2 13

#define EN_MUX1 A1
#define EN_MUX2 A0
//--------------------------------------

const int enables[] = {ELEVADOR, EN_S1, EN_S2, EN_S3, EN_S4, EN_S5};

const int servoMexedorInicial = 0;
const int servoMexedorFinal = 90;
int posServoMexedor = servoMexedorFinal;

//----------------PINOS MUX----------------//
//pinos de controle MUX1
const int s0mux1 = 2;
const int s1mux1 = 3;
const int s2mux1 = 4;
const int s3mux1 = 5;
//sinal MUX1
const int mux1 = SIG_MUX1;

//MUX2
const int s0mux2 = 6;
const int s1mux2 = 7;
const int s2mux2 = 8;
const int s3mux2 = 9;
//sinal MUX1
const int mux2 = SIG_MUX2;
//----------------------------------------//

//portas motores bipolares
const int dir = 10;
const int step = 11;


//portas motor arquimedes demonstraão
/*const int dirArquimedes = 10;
const int stepArquimedes = 11;
const int enArquimedes = 12;
bool isArquimedesEnabled = 0;*/


//portas motor elevador
bool isElevadorEnabled = 0;
AccelStepper motorBipolar(1, step, dir);
bool elevadorDescendo = 1;
//vide setup

//portas motor sabor1
/*const int sabor1Pin1A = 11;
const int sabor1Pin1B = 10;
const int sabor1Pin2A = 9;
const int sabor1Pin2B = 8;
*/

int readMux(int channel, int muxSel, bool pullup = true);

Servo dispenserLeft, dispenserRight;
const int tempoElevador = 4500;

struct pedido {
  bool aquecer;
  bool sabores[5] = {0,1,2,3,4};
};
/*const int maxLeft = 200;
const int minLeft = 90;
const int maxRight = 55;
const int minRight = 21*0.9;*/

//--------------posicoes dos servos do dispenser
//posicao esticado
//Lmin Rmax
const int minLeft = 72;
const int maxRight = 75;

//posicoa encolhido
//Lmax Rmin
const int minRight = 21*1.66;
const int maxLeft = 143*0.85;


/*const int maxLeft = 143*1.1;
const int minLeft = 100;
const int maxRight = 55;
const int minRight = 21*0.9;
void esticarDispenser();
*/


//------------------FUNÇÕES DE SETUP------------------------//
void setupCupDispenser() {
  seletorMux(COPO_L, mux1);
  seletorMux(COPO_R, mux2);

  pinMode(mux1, OUTPUT);
  pinMode(mux2, OUTPUT);

  dispenserLeft.attach(mux1);
  dispenserRight.attach(mux2);
  esticarDispenser();
  Serial.println("Setup cup dipenser: ok");
  return;
}

void setupEsteira() {
  seletorMux(ESTEIRA, mux1);
  pinMode(mux1, OUTPUT);
  digitalWrite(mux1, HIGH);
  Serial.println("Setup esteira: ok");
}

bool fimCursoElevador;
void setupElevador(int velocidadeElevador = 800, int aceleracaoElevador = 500) {
  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);
  //sentidoHorarioElevador = 0;
  //AccelStepper motorBipolar la nos defines
  //int velocidadeElevador = 800; 
  //int aceleracaoElevador = 500;
  seletorMux(FIM_CURSO, mux2);
  pinMode(mux2, INPUT);

  seletorMux(ELEVADOR, mux1);
  pinMode(mux1, OUTPUT);


  while(!readMux(FIM_CURSO, mux2)) {

  }

  motorBipolar.setMaxSpeed(velocidadeElevador);
  motorBipolar.setSpeed(velocidadeElevador);
  motorBipolar.setAcceleration(aceleracaoElevador);

  digitalWrite(mux1, HIGH);
  fimCursoElevador = digitalRead(mux2);
  //Serial.print("Fim de curso do elevador: ");
  //Serial.println(fimCursoElevador);
  Serial.println("Setup elevador: ok");
  }

void setupBomba() {
  seletorMux(BOMBA, mux2);
  pinMode(mux2, OUTPUT);
  digitalWrite(mux2, HIGH); 
  Serial.println("Setup bomba: ok");
}

int stepsPerRevolution = 200;
Stepper motorUnipolar(stepsPerRevolution, A5, A4, A3,A2);
void setupUnipolar(int stepsPerRevolution = 200, int velocidade = 60) {
  // initialize the stepper library on pins 8 through 11:
  //Stepper motorUnipolar(stepsPerRevolution, 8, 9, 10, 11);
  
  motorUnipolar.setSpeed(velocidade);
}

void setupBipolar(int velocidade = 100, int aceleracao = 100, bool sentidoHorario = 0) {

  pinMode(dir, OUTPUT);
  pinMode(step, OUTPUT);

  motorBipolar.setMaxSpeed(velocidade);
  motorBipolar.setSpeed(velocidade);
  motorBipolar.setAcceleration(aceleracao);
}



void levantarMexedor();
/*
void setupMexedor() {
  seletorMux(SERVO_MIX, mux2);
  //seletorMux()
  pinMode(MEXEDOR, OUTPUT);
  digitalWrite(MEXEDOR, HIGH);
  servoMexedor.attach(9);
  //posServoMexedor = servoMexedorInicial;
  //servoMexedor.write(posServoMexedor);
  
  levantarMexedor();
}*/

void setupEbulidor() {
  seletorMux(EBULIDOR, MUX_EBULIDOR);
  pinMode(MUX_EBULIDOR, OUTPUT);
  digitalWrite(MUX_EBULIDOR, HIGH);
  Serial.println("Setup ebulidor: ok");  
}

void setupMUX() {
  //SETUP MUX1
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

  //SETUP MUX2
  pinMode(s0mux2, OUTPUT); 
  pinMode(s1mux2, OUTPUT); 
  pinMode(s2mux2, OUTPUT); 
  pinMode(s3mux2, OUTPUT); 

  pinMode(mux1, OUTPUT); 

  digitalWrite(s0mux2, LOW);
  digitalWrite(s1mux2, LOW);
  digitalWrite(s2mux2, LOW);
  digitalWrite(s3mux2, LOW);

  digitalWrite(mux2, LOW); 
  
  Serial.println("Setup MUX2: ok");

  //ENABLES MUXES
  pinMode(EN_MUX1, OUTPUT);
  pinMode(EN_MUX2, OUTPUT);
  digitalWrite(EN_MUX1, HIGH);
  digitalWrite(EN_MUX2, HIGH);
}


//-----------------FUNCOES TECNICAS/MUX-------------------------//

int seletorMux(int channel, int muxSel){ //muxSel aceita variaveis mux1 ou mux2
if(muxSel==mux1) {
  digitalWrite(EN_MUX1, LOW);
  Serial.println("MUX 1 habilitado");
} else if(muxSel==mux2) {
  digitalWrite(EN_MUX2, LOW);
  Serial.println("MUX 2 habilitado");
}

  int controlPin[2][4] = {
    {s0mux1, s1mux1, s2mux1, s3mux1},
    {s0mux2, s1mux2, s2mux2, s3mux2}
  };

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0    BUZZER    IR0
    {1,0,0,0}, //channel 1    EN_S1     IR1
    {0,1,0,0}, //channel 2    EN_S5     IR2
    {1,1,0,0}, //channel 3    EN_S2     IR3
    {0,0,1,0}, //channel 4    EN_S3     IR4
    {1,0,1,0}, //channel 5    EN_S4     IR5
    {0,1,1,0}, //channel 6    EN_ELEV   IR6
    {1,1,1,0}, //channel 7    COPO_L    SONDA
    {0,0,0,1}, //channel 8    ESTEIRA   COPO_R
    {1,0,0,1}, //channel 9              SERVO_MIX
    {0,1,0,1}, //channel 10             FIM_CURSO
    {1,1,0,1}, //channel 11             BOMBA
    {0,0,1,1}, //channel 12             EBULIDOR
    {1,0,1,1}, //channel 13             
    {0,1,1,1}, //channel 14             MOTOR_MIX
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[muxSel][i], muxChannel[channel][i]);
  }

  Serial.print("Selecionado o canal ");
  Serial.print(channel);
  Serial.print(" do mux ");
  if(muxSel == mux1) {
    Serial.print("1.");
  } else 
    Serial.print("2.");
  return muxSel;
}

int readMux(int channel, int muxSel, bool pullup = false){ //muxSel aceita variaveis mux1 ou mux2
  if(pullup) {
    pinMode(muxSel, INPUT_PULLUP);
  } else {
    pinMode(muxSel, INPUT);
  }  
  
  seletorMux(channel, muxSel);

  //read the value at the SIG pin
  int val = digitalRead(muxSel);

  //disable mux
  if(muxSel==mux1) {
    digitalWrite(EN_MUX1, HIGH);
    Serial.println("MUX 1 desabilitado");
  } else if(muxSel==mux2) {
    digitalWrite(EN_MUX2, HIGH);
    Serial.println("MUX 2 desabilitado");
  }


  return val;
}

//------------------FUNÇÕES DE LIGAR/DESLIGAR------------------------//
void esticarDispenser() {
  for (float posL = maxLeft, posR = minRight; posL>minLeft || posR<maxRight; posL -= 1.22, posR +=1) {
    if(posL > minLeft)
      dispenserLeft.write(posL);
    if(posR < maxRight)
      dispenserRight.write(posR); 
      delay(50);   
  } //termina em Lmin Rmax
  //dispenserLeft.write(100);
  //dispenserRight.write(55);  
}
void encolherDispenser() {
  for (float posL = minLeft, posR = maxRight; posL<maxLeft || posR>minRight; posL += 1.22, posR -=1) {
    if(posL < maxLeft)
      dispenserLeft.write(posL);
    if(posR > minRight)
      dispenserRight.write(posR);  
      delay(50);  
  } //termina em Lmax Rmin
  //dispenserLeft.write(143);
  //dispenserRight.write(21);
}

void dispensarCopo() {
  setupCupDispenser();
  
  encolherDispenser();
  delay(2000);
  esticarDispenser();
  Serial.println("Copo dispensado");
  return;
}

bool ligarEsteira() {
  setupEsteira();
  seletorMux(ESTEIRA, mux1);
  
  pinMode(mux1, OUTPUT);
  digitalWrite(mux1, LOW);  //logica negativa
  Serial.println("Esteira ligada");
}

bool desligarEsteira() {
  seletorMux(ESTEIRA, mux1);

  pinMode(mux1, OUTPUT);
  digitalWrite(mux1, HIGH);
  Serial.println("Esteira desligada");
}

void pararElevador() {
  //isElevadorEnabled = 0;
  Serial.println("Parando elevador...");
  motorBipolar.moveTo(0);
  digitalWrite(mux1, HIGH);  
  Serial.println("Elevador parado");
}
void descerElevador() {
  setupElevador();
  //descer elevador: sentido horário
  digitalWrite(mux1, LOW);
  Serial.println("Descendo elevador...");
  //elevadorDescendo = 1;  
  //isElevadorEnabled = 1;
  while(/*elevadorDescendo && isElevadorEnabled && */!digitalRead(mux2)) {
    motorBipolar.moveTo(-10000);
  }
  pararElevador();

  /*if(!fimCursoElevador) {
    elevadorDescendo = 1;  
    isElevadorEnabled = 1;
    digitalWrite(enElevador, LOW);
  } else {
    pararElevador();
  }*/
}

void subirElevador() {
  setupElevador();
  //elevadorDescendo = 0;  
  //isElevadorEnabled = 1;
  digitalWrite(mux1, LOW);
  Serial.println("Subindo elevador...");
  int i = 0;
  while(true) { // TODO WHILE NINGUEM GRITOU
    motorBipolar.moveTo(10000);
    if (!i%1000) {
      Serial.println(i + " passos");
    }
  }
  pararElevador();
  //subir elevador
  //delay(tempoElevador);
  //parar elevador
}

void ligarBomba() {
  setupBomba();
  digitalWrite(MUX_BOMBA, LOW);
  Serial.println("Bomba ligada");
  }
void desligarBomba() {
  setupBomba();
  digitalWrite(MUX_BOMBA, HIGH);
  //digitalWrite(BOMBA, HIGH);
  Serial.println("Bomba desligada");
}

void ligarEbulidor() {
  setupEbulidor();
  digitalWrite(mux2, LOW);
  Serial.println("Ebulidor ligado");
}
void desligarEbulidor() {
  setupEbulidor();
  digitalWrite(mux2, HIGH);
  Serial.println("Ebulidor desligado");
}


void levantarMexedor() {
  Serial.println("Levantando mexedor...");
  Servo servoMexedor;
  seletorMux(SERVO_MIX, mux2);
  pinMode(mux2, OUTPUT);
  //digitalWrite(MEXEDOR, HIGH);
  servoMexedor.attach(mux2);

  for(int i = posServoMexedor; i < servoMexedorFinal; i += 1) {
    posServoMexedor = i;
    servoMexedor.write(posServoMexedor);
    Serial.println(posServoMexedor);
    delay(30);    
  }
  Serial.println("Mexedor levantado");
}

void abaixarMexedor() {
  Serial.println("Abaixando mexedor...");
  Servo servoMexedor;
  seletorMux(SERVO_MIX, mux2);
  pinMode(mux2, OUTPUT);
  //digitalWrite(MEXEDOR, HIGH);
  servoMexedor.attach(mux2);

  for(int i = posServoMexedor; i > servoMexedorInicial; i -= 1) {
    posServoMexedor = i;
    servoMexedor.write(posServoMexedor);
    Serial.println(posServoMexedor);
    delay(30);
  }  
  Serial.println("Mexedor abaixado");
}


void ligarMexedor() {
  seletorMux(MOTOR_MIX, mux2);
  digitalWrite(mux2, LOW);
  Serial.println("Mexedor ligado");
}
void desligarMexedor() {
  seletorMux(MOTOR_MIX, mux2);
  digitalWrite(mux2, HIGH); 
  Serial.println("Mexedor desligado");
}
void mexer(int tempo = 60000) {
  Serial.println("Mexendo bebida...");
  abaixarMexedor();
  delay(500);

  ligarMexedor();
  delay(tempo);
  desligarMexedor();

  delay(500);
  levantarMexedor();
}

void encherCopo() {
  ligarBomba();
  Serial.println("Enchendo copo...");
  delay(5000);
  //if nivel de agua
  desligarBomba();
  Serial.println("Copo cheio");
  return;
  }


void aquecer() {
  ligarEbulidor();
  delay(20000);

  //if temperatura
  desligarEbulidor();
  return;
} 



void dispensarSabor(int sabor) {
  int tempo = 2500;
  bool sentidoHorario = false;

  seletorMux(enables[sabor], mux1);
  digitalWrite(mux1, LOW);

  Serial.print("Dispensando sabor ");
  Serial.println(sabor);

  if(sabor==3 || sabor==4) {
    //bipolar
    switch(sabor) {
      case 3: {
        setupBipolar(100, 100);
        tempo = 2500;
        sentidoHorario = 0;
        break;
      }
       case 4: {
        setupBipolar(100, 100);
        tempo = 2500;
        sentidoHorario = 0;
        break;
      }
    }

    int starttime, endtime, loopcount;
    starttime = millis();
    endtime = starttime;
    while ((endtime - starttime) <=tempo) // do this loop for up to tempo
    {
      // code here
        if (sentidoHorario) {
        motorBipolar.moveTo(10000);
      } else  {
        motorBipolar.moveTo(-10000);
      }
      // Comando para acionar o motor no sentido especificado
      motorBipolar.run();
      loopcount = loopcount+1;
      endtime = millis();
    }

  }else if(sabor==1 || sabor==2 || sabor==5) {
    //unipolar
    int passos = 2000;
    switch(sabor) {
      case 1: {
        sentidoHorario = 0;
        setupUnipolar(2000);
        passos = 2000;
        break;
      }
       case 2: {
        sentidoHorario = 0;
        setupUnipolar(2000);
        passos = 2000;
        break;
      }
       case 5: {
        sentidoHorario = 0;
        setupUnipolar(2000);
        passos = 2000;
        break;
      }

    }

    if(!sentidoHorario)
          passos *= -1;

    int stepCount = 0; // number of steps the motor has taken
       
    motorUnipolar.step(passos);
    //stepCount++;
    //delay(500);
    //while(stepCount < passos) {   }
     // step one step:
    
  } 

  digitalWrite(mux1, HIGH);   //desliga enable
  //serial.print (loopcount,DEC);
  Serial.println("Sabor dispensado");
  }

void playBuzzer() {
  seletorMux(BUZZER, mux1);
  delay(1000);
  int tempo = 400;
  tone(10,440,tempo); //LA
  delay(tempo);
  tone(10,294,tempo); //RE
  delay(tempo);
  tone(10,349,tempo/2); //FA - O tempo/2 faz com que demore metade do valor estipulado anteriormente, pois essa parte é mais rápida
  delay(tempo/2);
  tone(10,392,tempo/2); //SOL
  delay(tempo/2);
  tone(10,440,tempo); //LA
  delay(tempo);
  tone(10,294,tempo); //RE
  delay(tempo);
  tone(10,349,tempo/2); //FA
  delay(tempo/2);
  tone(10,392,tempo/2); //SOL
  delay(tempo/2);
  tone(10,330,tempo); //MI
  delay(tempo);
}

void desativarMUX(int muxSel) {
  if(muxSel==mux1) {
    digitalWrite(EN_MUX1, HIGH);
  } else if(muxSel==mux2) {
    digitalWrite(EN_MUX2, HIGH);
  }
}

void prepararPedido(pedido pedido) {
  // Estação: dispenser de copos
  dispensarCopo();
  
  delay(3000);

  ligarEsteira();
  delay(50);
  while(readMux(IR0, mux2) == HIGH) {
  }
  desligarEsteira();
  
  //-------estação: água e aquecimento------
  delay(1000);
  descerElevador();
  delay(2000);

  encherCopo();
  delay(2000);

  if(pedido.aquecer)
    aquecer();
  delay(2000);

  subirElevador();
  delay(2000);
  //estação: sabores
  int irs[] = {IR0, IR1, IR2, IR3, IR4, IR5, IR6};

  for(int i = 0; i < 5; i++) {
    ligarEsteira();
    if(pedido.sabores[i]) {
      while(readMux(irs[i+1], mux2) == HIGH) {

      }
        desligarEsteira();
        dispensarSabor(i+i);  //i+1 sim: confia!
        delay(1000);
    }
  }
  delay(1000);
  ligarEsteira();
  while(readMux(IR6, mux2) == HIGH) {
  
  }
  desligarEsteira();
  delay(1000);
  mexer();

  delay(2000);

  ligarEsteira();
  delay(1500);
  desligarEsteira();

  playBuzzer();
  
  }
  
//----------------ate aqui tudo ok-----------------------



/*
void modoTeste() {
  int comando = 0;
    
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  switch(comando) {
    //elevador    
    case 'h': { 
      subirElevador();
      break;           
      }    
    case 'j': {     
      descerElevador();
      break;      
    }
    case 'k': {
      pararElevador();
      Serial.println("Elevador parado");
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
    case 'i': {
      aquecer();
      break;
    }
    
    //esteira
    case 'e': {
      ligarEsteira();
      break;
    }
    case 'r': {
      desligarEsteira();
      break;  
    }

    //bomba    
    case 'b': {
      ligarBomba();
      break;
    }
    case 'n': {
      desligarBomba();
      break;
    }
    case '1': {
      encherCopo();
      break;
    }

    //sabor1    
    case 'd': {
      dispensarSabor();
      break;
    }
    
    //dispenser de copos    
    case 'c': {
      dispensarCopo();
      break; 
    }
    case 'z': {
      esticarDispenser();
      break;  
    }   
    case 'x': {
      encolherDispenser();
      break;  
    }

    //mexedor    
    case 't': {
      abaixarMexedor();
      break;
    }
    case 's': {
      levantarMexedor();
      break;
    }
    case 'a': {
      ligarMexedor();
      break;  
    }    
    case 'p': {
      desligarMexedor();
      break;
    }
    case 'm': {
      mexer();
      break;
    }
    }

  fimCursoElevador = digitalRead(mux1);
  //Serial.print("fim de curso:");

  if(fimCursoElevador) {
    
    
    if(elevadorDescendo) {
      pararElevador();
      isElevadorEnabled = 0;
      Serial.println(fimCursoElevador);
    }
  } else {
    
  }
      
  if(elevadorDescendo && isElevadorEnabled) {
    motorBipolar.moveTo(-10000);
  } else if(!elevadorDescendo && isElevadorEnabled) {
    motorBipolar.moveTo(10000);  
  } else if(!isElevadorEnabled) {
    
  }
  motorBipolar.run();
}
*//*
int pedidosNaFila = 0;
*/
void setup() {
  Serial.begin(9600);
  setupMUX();

  
/*
  setupEsteira();
  setupCupDispenser();
  setupElevador();
  setupSabores();
  setupMexedor();
  setupBomba();
  setupEbulidor();

 */
}

void loop() {
  desativarMUX(mux1);
  desativarMUX(mux2);
  pedido pedidoTeste;
  pedidoTeste.aquecer = 1;
  for (int i = 0; i < 5; i++)  
    pedidoTeste.sabores[i] = 1;
  
  //modoTeste();
  
  prepararPedido(pedidoTeste);
  exit(0);
}
