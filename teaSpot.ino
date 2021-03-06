#include <Servo.h>
#include <AccelStepper.h>
#include <Stepper.h>
#include <OneWire.h>  
#include <DallasTemperature.h>

#define CHA sabor1
#define TODDY sabor2
#define ACUCAR sabor4
#define ADOCANTE sabor5

//-------------DEFINES MUX---------------//

//MUX1
#define BUZZER 0
#define EN_S1 1
#define EN_S5 2       //P VER A VER
#define EN_S2 3
#define EN_S3 5       //VER P VER A
#define EN_S4 4       //VER P VER A
#define ELEVADOR 6    //P VER VERD A
#define COPO_L 7
#define ESTEIRA 8
#define SONDA 15

#define MUX_BUZZER mux1
#define MUX_EN_S1 mux1
#define MUX_EN_S5 mux1
#define MUX_EN_S2 mux1
#define MUX_EN_S3 mux1
#define MUX_EN_S4 mux1
#define MUX_ENABLES mux1
#define MUX_ELEVADOR mux1
#define MUX_COPO_L mux1
#define MUX_ESTEIRA mux1
#define MUX_SONDA mux1


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
#define IR7 15
#define MUX_IR0 mux2
#define MUX_IR1 mux2
#define MUX_IR2 mux2
#define MUX_IR3 mux2
#define MUX_IR4 mux2
#define MUX_IR5 mux2
#define MUX_IR6 mux2
#define MUX_IR7 mux2
#define MUX_IRS mux2

#define COPO_R 11
#define SERVO_MIX 12
#define MUX_COPO_R mux2
#define MUX_SERVO_MIX mux2
//13 nao conectado
//#define SONDA 14
//15 nao conectado
struct pedido {
  bool aquecer;
  bool sabores[5] = {0,0,0,0,0}; //= {0,1,2,3,4};
  byte idCliente;
};
pedido pedidoAtual;
bool pedidoPronto = false;
bool temPedido = false;
const bool debug = false;

//MUX NO ARDUINO------------------------
#define SIG_MUX1 12 
#define SIG_MUX2 13 

#define EN_MUX1 A1
#define EN_MUX2 A0
//--------------------------------------

const int enables[] = {ELEVADOR, EN_S1, EN_S2, EN_S3, EN_S4, EN_S5};

const int servoMexedorInicial = 0;  //posição abaixada da alavanca
const int servoMexedorFinal = 90; //posição levantada da alavanca
int posServoMexedor = servoMexedorFinal;

//----------------PINOS MUX----------------//
//pinos de controle MUX1
const int s0mux1 = 2;
const int s1mux1 = 3;
const int s2mux1 = 4;
const int s3mux1 = 5;
//sinal MUX1
const int mux1 = SIG_MUX1;  //pino de dados do mux1

//MUX2
const int s0mux2 = 6;
const int s1mux2 = 7;
const int s2mux2 = 8;
const int s3mux2 = 9;
//sinal MUX1
const int mux2 = SIG_MUX2;  //pino de dados do mux2
//----------------------------------------//

//portas motores bipolares
const int dir = 10;
const int stp = 11; 


//portas motor arquimedes demonstraão
/*const int dirArquimedes = 10;
const int stepArquimedes = 11;
const int enArquimedes = 12;
bool isArquimedesEnabled = 0;*/


//portas motor elevador
bool isElevadorEnabled = 0;
AccelStepper motorBipolar(1, stp, dir);
bool elevadorDescendo = 1;
//vide setup

//portas motor sabor1
/*const int sabor1Pin1A = 11;
const int sabor1Pin1B = 10;
const int sabor1Pin2A = 9;
const int sabor1Pin2B = 8;
*/

//sonda
OneWire oneWire(MUX_SONDA); //protocolo oneWire para a sonda
DallasTemperature sonda(&oneWire); //encaminha referências OneWire para o sensor


int readMux(int channel, int muxSel/*, bool pullup = true*/);

Servo dispenserLeft, dispenserRight;
const int tempoElevador = 4500;


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
  
  desativarMUX(mux1);
  desativarMUX(mux2);
  pinMode(MUX_COPO_L, OUTPUT);
  pinMode(MUX_COPO_R, OUTPUT);

  dispenserLeft.attach(MUX_COPO_L);
  dispenserRight.attach(MUX_COPO_R);

  seletorMux(COPO_L, MUX_COPO_L);
  seletorMux(COPO_R, MUX_COPO_R);
  delay(10);
  //esticarDispenser();
  if(debug) 
  Serial.println("Setup cup dipenser: ok");
  return;
}

void setupEsteira() {
  desativarMUX(MUX_ESTEIRA);
  pinMode(MUX_ESTEIRA, OUTPUT);
  digitalWrite(MUX_ESTEIRA, HIGH);
  seletorMux(ESTEIRA, MUX_ESTEIRA);
  if(debug) 
  Serial.println("Setup esteira: ok");
}

//bool fimCursoElevador;
void setupElevador(int velocidadeElevador = 800, int aceleracaoElevador = 500) {
  pinMode(dir, OUTPUT);
  pinMode(stp, OUTPUT);
  //sentidoHorarioElevador = 0;
  //AccelStepper motorBipolar la nos defines
  //int velocidadeElevador = 800; 
  //int aceleracaoElevador = 500;
  desativarMUX(MUX_FIM_CURSO);
  pinMode(MUX_FIM_CURSO, INPUT);
  seletorMux(FIM_CURSO, MUX_FIM_CURSO);

  desativarMUX(MUX_ELEVADOR);
  
  pinMode(MUX_ELEVADOR, OUTPUT);
  digitalWrite(MUX_ELEVADOR, HIGH);
  seletorMux(ELEVADOR, MUX_ELEVADOR);
  //while(!readMux(FIM_CURSO, MUX_FIM_CURSO)) {  }

  motorBipolar.setMaxSpeed(velocidadeElevador);
  motorBipolar.setSpeed(velocidadeElevador);
  motorBipolar.setAcceleration(1000);

  
  //fimCursoElevador = digitalRead(mux2);
  if(debug) 
  Serial.print("Fim de curso do elevador: ");
  //Serial.println(fimCursoElevador);
  if(debug) 
  Serial.println("Setup elevador: ok");
  }

void setupBomba() {
  desativarMUX(MUX_BOMBA);  
  pinMode(MUX_BOMBA, OUTPUT);
  digitalWrite(MUX_BOMBA, HIGH); 
  seletorMux(BOMBA, MUX_BOMBA);
  if(debug) 
  Serial.println("Setup bomba: ok");
}

int stepsPerRevolution = 200;
//Stepper motorUnipolar(stepsPerRevolution, A5, A4, A3, A2);
Stepper motorUnipolar(stepsPerRevolution, A2, A3, A4, A5);
  //Stepper motorUnipolar(stepsPerRevolution, 8, 9, 10, 11);
  //Stepper motorUnipolar(stepsPerRevolution, in4, in3 in2, in1);
  //Stepper motorUnipolar(stepsPerRevolution, A2, A3, A4, A5);

/*a5 a4 a3 a2 
in1 in2 in3 in4
8 9 10 11
in4 in3 in2 in1*/
void setupUnipolar(int stepsPerRevolution = 200, int velocidade = 60) {
  // initialize the stepper library on pins 8 through 11:
  //Stepper motorUnipolar(stepsPerRevolution, 8, 9, 10, 11);
  /*pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);*/
  motorUnipolar.setSpeed(velocidade);
}

void setupBipolar(int velocidade = 100, int aceleracao = 100, bool sentidoHorario = 0) {

  pinMode(dir, OUTPUT);
  pinMode(stp, OUTPUT);

  motorBipolar.setMaxSpeed(velocidade);
  motorBipolar.setSpeed(velocidade);
  motorBipolar.setAcceleration(aceleracao);
}



void levantarMexedor();
Servo servoMexedor;
void setupMexedor() {
  
  desativarMUX(MUX_SERVO_MIX);
  pinMode(MUX_SERVO_MIX, OUTPUT);
  //digitalWrite(MEXEDOR, HIGH);
  servoMexedor.attach(MUX_SERVO_MIX);
  seletorMux(SERVO_MIX, MUX_SERVO_MIX);
  
}

void setupEbulidor() {
  desativarMUX(MUX_EBULIDOR);
  pinMode(MUX_EBULIDOR, OUTPUT);
  digitalWrite(MUX_EBULIDOR, HIGH);
  seletorMux(EBULIDOR, MUX_EBULIDOR);
  if(debug) 
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
  
  if(debug) 
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
  
  if (debug)
    Serial.println("Setup MUX2: ok");

  //ENABLES MUXES
  pinMode(EN_MUX1, OUTPUT);
  pinMode(EN_MUX2, OUTPUT);
  desativarMUX(mux1);
  desativarMUX(mux2);
}


//-----------------FUNCOES TECNICAS/MUX-------------------------//

int seletorMux(int channel, int muxSel){ //muxSel aceita variaveis mux1 ou mux2
  

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
  desativarMUX(muxSel);
  if(muxSel==mux1) {
    for(int i = 0; i < 4; i ++){
      digitalWrite(controlPin[0][i], muxChannel[channel][i]);
    }
  } else if(muxSel==mux2) {
    for(int i = 0; i < 4; i ++){
      digitalWrite(controlPin[1][i], muxChannel[channel][i]);
    }
  }

  if(muxSel==mux1) {
    digitalWrite(EN_MUX1, LOW);
    if(debug) 
  Serial.println("MUX 1 habilitado");
  } else if(muxSel==mux2) {
    digitalWrite(EN_MUX2, LOW);
    if(debug) 
  Serial.println("MUX 2 habilitado");
  }

  if(debug) 
  Serial.print("Selecionado o canal ");
  if(debug) 
  Serial.print(channel);
  if(debug) 
  Serial.print(" do mux ");
  if(muxSel == mux1) {
    if(debug) 
  Serial.println("1.");
  } else 
    if(debug) 
  Serial.println("2.");
  return muxSel;
}

int readMux(int channel, int muxSel/*, bool pullup = false*/){ //muxSel aceita variaveis mux1 ou mux2
  
  pinMode(muxSel, INPUT);
 
  
  seletorMux(channel, muxSel);

  //read the value at the SIG pin
  int val = digitalRead(muxSel);

  //disable mux
  /*if(muxSel==mux1) {
    digitalWrite(EN_MUX1, HIGH);
    Serial.println("MUX 1 desabilitado");
  } else if(muxSel==mux2) {
    digitalWrite(EN_MUX2, HIGH);
    Serial.println("MUX 2 desabilitado");
  }*/


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
  delay(30);
  ///encolherDispenser();
  //delay(2000);
  esticarDispenser();
  delay(2000);
  encolherDispenser();
  if(debug) 
  Serial.println("Copo dispensado");
  return;
}

bool ligarEsteira() {
  setupEsteira();
  
  digitalWrite(MUX_ESTEIRA, LOW);  //logica negativa
  if(debug) 
  Serial.println("Esteira ligada");
}

bool desligarEsteira() {
  setupEsteira();
  digitalWrite(MUX_ESTEIRA, HIGH);
  if(debug) 
  Serial.println("Esteira desligada");
}

void pararElevador() {
  //isElevadorEnabled = 0;
  if(debug) 
  Serial.println("Parando elevador...");
  motorBipolar.moveTo(0);
  digitalWrite(MUX_ELEVADOR, HIGH);  
  if(debug) 
  Serial.println("Elevador parado");
}

int numero = 3;
int sentido_horario =0, sentido_antihorario = 1;
void testePasso() {
  // Aguarda os caracteres no serial monitor
  
  if (Serial.available() > 0) 
  {
    numero = Serial.read();
    {
      if (numero == '1')
      {
        if(debug) 
  Serial.println("Numero 1 recebido - Girando motor sentido horario.");
        digitalWrite(MUX_ELEVADOR, LOW);
        sentido_horario = 1;
        sentido_antihorario = 0;
      } 
      
      if (numero == '2')
      {
        if(debug) 
  Serial.println("Numero 2 recebido - Girando motor sentido anti-horario.");
        digitalWrite(MUX_ELEVADOR, LOW);
        sentido_horario = 0;
        sentido_antihorario = 1;
      }
     
      if (numero == '3')
      {
        if(debug) 
  Serial.println("Numero 3 recebido - Parando motor...");
        sentido_horario = 0;
        sentido_antihorario = 0;
        motorBipolar.moveTo(0);
        digitalWrite(MUX_ELEVADOR, HIGH);
      } 
    }
  }
  
  // Move o motor no sentido horario
  if (sentido_horario == 1)
  {
    motorBipolar.moveTo(10000);
  }
  // Move o motor no sentido anti-horario
  if (sentido_antihorario == 1)
  {
    motorBipolar.moveTo(-10000);
  }
  // Comando para acionar o motor no sentido especificado
  motorBipolar.run();
}


void descerElevador() {
  setupElevador();
  //descer elevador: sentido horário
  
  if(debug) 
  Serial.println("Descendo elevador...");
  //elevadorDescendo = 1;  
  //isElevadorEnabled = 1;

  unsigned long i=0;
  int j = 0;
  int flag=0;
  if(debug) 
    Serial.print("Valor fim_curso: ");
  if(debug) 
    Serial.println(digitalRead(MUX_FIM_CURSO));
  delay(50);
  bool fimCurso = 0;
  digitalWrite(MUX_ELEVADOR, LOW);
  while(!flag) {
    if(digitalRead(MUX_FIM_CURSO) == 1) {
      flag = 1;
      for(j=0;j<350;j++)
      {
        if(!digitalRead(MUX_FIM_CURSO))
          flag = 0;
      }
      //Serial.print("Valor fim_curso");
      //Serial.print(i);
      //Serial.println(digitalRead(MUX_FIM_CURSO));
    }

    /*if(!(i%1000))
      Serial.println(i);*/
    motorBipolar.moveTo(-10000);
    motorBipolar.run();
    i++;
    //fimCurso = digitalRead(MUX_FIM_CURSO);
  }
  /*Serial.print("Valor2 fim_curso: ");
  Serial.println(digitalRead(MUX_FIM_CURSO));
  Serial.print(i);
  Serial.println(" passos");*/
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
  digitalWrite(MUX_ELEVADOR, LOW);
  if(debug) 
  Serial.println("Subindo elevador...");
  unsigned long i = 0;
  while(i < 160000UL*5UL) { // TODO WHILE NINGUEM GRITOU
    motorBipolar.moveTo(10000);
    motorBipolar.run();
    i++;
    /*if (!(i%1000)) {
      Serial.print(i);
      Serial.println(" passos");
    }*/
  }
  pararElevador();
  //subir elevador
  //delay(tempoElevador);
  //parar elevador
}

void ligarBomba() {
  setupBomba();
  digitalWrite(MUX_BOMBA, LOW);
  if(debug) 
  Serial.println("Bomba ligada");
  }
void desligarBomba() {
  //setupBomba();
  digitalWrite(MUX_BOMBA, HIGH);
  //digitalWrite(BOMBA, HIGH);
  if(debug) 
  Serial.println("Bomba desligada");
}

void ligarEbulidor() {
  setupEbulidor();
  digitalWrite(MUX_EBULIDOR, LOW);
  if(debug) 
  Serial.println("Ebulidor ligado");
}
void desligarEbulidor() {
  setupEbulidor();
  digitalWrite(MUX_EBULIDOR, HIGH);
  if(debug) 
  Serial.println("Ebulidor desligado");
}


void levantarMexedor() {
  if(debug) 
  Serial.println("Levantando mexedor...");
  //Servo servoMexedor;
  desativarMUX(MUX_SERVO_MIX);
  pinMode(MUX_SERVO_MIX, OUTPUT);
  servoMexedor.attach(MUX_SERVO_MIX);
  seletorMux(SERVO_MIX, MUX_SERVO_MIX);
  
  //digitalWrite(MEXEDOR, HIGH);
  

  for(int i = posServoMexedor; i < servoMexedorFinal; i += 1) {
    posServoMexedor = i;
    servoMexedor.write(posServoMexedor);
    if(debug) 
  Serial.println(posServoMexedor);
    delay(10);    
  }
  if(debug) 
  Serial.println("Mexedor levantado");
}

void abaixarMexedor() {
  if(debug) 
  Serial.println("Abaixando mexedor...");
  setupMexedor();
  

  for(int i = posServoMexedor; i > servoMexedorInicial; i -= 1) {
    posServoMexedor = i;
    servoMexedor.write(posServoMexedor);
    if(debug) 
  Serial.println(posServoMexedor);
    delay(10);
  }  
  if(debug) 
  Serial.println("Mexedor abaixado");
}


void ligarMexedor() {
  desativarMUX(MUX_MOTOR_MIX);
  pinMode(MUX_MOTOR_MIX, OUTPUT);
  digitalWrite(MUX_MOTOR_MIX, LOW);
  seletorMux(MOTOR_MIX, MUX_MOTOR_MIX);
  
  if(debug) 
  Serial.println("Mexedor ligado");
}
void desligarMexedor() {
  desativarMUX(MUX_MOTOR_MIX);
  //seletorMux(MOTOR_MIX, MUX_MOTOR_MIX);
  //pinMode(MUX_MOTOR_MIX, OUTPUT);
  //digitalWrite(MUX_MOTOR_MIX, HIGH); 
  if(debug) 
  Serial.println("Mexedor desligado");
}
void mexer(int tempo = 15000) {
  if(debug) 
  Serial.println("Mexendo bebida...");
  abaixarMexedor();
  delay(1000);

  ligarMexedor();
  for (int i = 0; i< 3; i++)
    delay(tempo);
  desligarMexedor();

  delay(500);
  levantarMexedor();
  desativarMUX(MUX_SERVO_MIX);
}

void encherCopo(int tempo = 5000) {
  if(debug) 
  Serial.println("Enchendo copo...");
  ligarBomba();
  delay(tempo);
  //if nivel de agua
  digitalWrite(MUX_BOMBA, HIGH);
  seletorMux(16,MUX_BOMBA);
  //desligarBomba();
  if(debug) 
  Serial.println("Copo cheio");
  return;
  }




void aquecer(int tempo = 40000) {
  ligarEbulidor();
  //for (int i = 0; i < 100; i++)

  //desativarMUX(MUX_SONDA);
  //pinMode(MUX_SONDA, INPUT);

  /*seletorMux(SONDA, MUX_SONDA);
  do
  {
    sonda.requestTemperatures();
    delay(2500);
  } while (sonda.getTempCByIndex(0) < 40.0);
  */

  delay(tempo);

  //if temperatura
  desligarEbulidor();
  return;
} 



void dispensarSabor(int sabor) {
  int tempo = 2500;
  bool sentidoHorario = false;
  desativarMUX(MUX_ENABLES);

  if(debug) 
  Serial.print("Dispensando sabor ");
  if(debug) 
  Serial.println(sabor);

  if(sabor==3 || sabor==4) {
    //bipolar
    switch(sabor) {
      case 3: {
        setupBipolar(100, 100);
        tempo = 9000;
        sentidoHorario = 1;
        break;
      }
       case 4: { 
        setupBipolar(100, 100);
        tempo = 3200;
        sentidoHorario = 1;
        break;
      }
    }

    int starttime, endtime, loopcount;
    starttime = millis();
    endtime = starttime;
    seletorMux(enables[sabor], MUX_ENABLES);
    digitalWrite(MUX_ENABLES, LOW);
    while ((endtime - starttime) <= tempo) // do this loop for up to tempo
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
        setupUnipolar(70, 60);  //30
        passos = 150; //175
        break;
      }
       case 2: {
        sentidoHorario = 1;
        setupUnipolar(35, 80);
        passos = 2500;
        break;
      }
       case 5: {
        sentidoHorario = 0;
        setupUnipolar(35, 35);
        passos = 75;
        break;
      }
    }

    if(!sentidoHorario)
          passos *= -1;

    int stepCount = 0; // number of steps the motor has taken
    seletorMux(enables[sabor], MUX_ENABLES);
    digitalWrite(MUX_ENABLES, LOW);
    //motorUnipolar.setSpeed(100);
    //for(int f = 0; f<3; f++) {
    motorUnipolar.step(passos);
    
    /*while(true) {
      //motorUnipolar.step(passos);
      //motorUnipolar.step(1);
      //Serial.print("steps:");
      //Serial.println(stepCount);
      //stepCount++;
      //delay(500);

      motorUnipolar.step(passos);
      delay(500);
    
      // step one revolution in the other direction:
      //Serial.println("counterclockwise");
      //motorUnipolar.step(-passos);
      //delay(500);
    }*/
    
    //stepCount++;
    //delay(500);
    //while(stepCount < passos) {   }
     // step one step:
    
  } 

  digitalWrite(MUX_ENABLES, HIGH);   //desliga enable
  //serial.print (loopcount,DEC);
  if(debug) 
  Serial.println("Sabor dispensado");
  }

void playBuzzer() {
  int tempo = 400;
  
  desativarMUX(MUX_BUZZER);
  pinMode(MUX_BUZZER, OUTPUT);
  seletorMux(BUZZER, MUX_BUZZER);
  
  tone(MUX_BUZZER,440,tempo); //LA
  delay(tempo);
  tone(MUX_BUZZER,294,tempo); //RE
  delay(tempo);
  tone(MUX_BUZZER,349,tempo/2); //FA - O tempo/2 faz com que demore metade do valor estipulado anteriormente, pois essa parte é mais rápida
  delay(tempo/2);
  tone(MUX_BUZZER,392,tempo/2); //SOL
  delay(tempo/2);
  tone(MUX_BUZZER,440,tempo); //LA
  delay(tempo);
  tone(MUX_BUZZER,294,tempo); //RE
  delay(tempo);
  tone(MUX_BUZZER,349,tempo/2); //FA
  delay(tempo/2);
  tone(MUX_BUZZER,392,tempo/2); //SOL
  delay(tempo/2);
  tone(MUX_BUZZER,330,tempo); //MI
  delay(tempo);
  desativarMUX(MUX_BUZZER);
}

void desativarMUX(int muxSel) {
  if(muxSel==mux1) {
    digitalWrite(EN_MUX1, HIGH);
  } else if(muxSel==mux2) {
    digitalWrite(EN_MUX2, HIGH);
  }
}

void setPedidoPronto() {
  pedidoPronto = true;
  Serial.print("PRONTO");
  if(debug) {Serial.println(" ");}
  temPedido = false;
}

void prepararPedido(pedido pedido) {
  // Estação: dispenser de copos
  //delay(10000);
  //setPedidoPronto();

  delay(5000);
  dispensarCopo();
  
  delay(3000);

  ligarEsteira();
  delay(50);


  //-------estação: água e aquecimento------
  //int timer2 = millis();
  //int timerMax2 = timer2+7000;
 
  //desativarMUX(MUX_ESTEIRA);
  while(readMux(IR0, MUX_IR0) == HIGH/* && timer2<timerMax2*/) {
  }
  desativarMUX(MUX_ESTEIRA);
  /*if(timer2<timerMax2) {
    return;
  }*/
  //desligarEsteira();
  
  if(readMux(IR0, MUX_IR0)== LOW) {
    if(debug) 
      Serial.println("Copo detectado no aquecimento");
  }
  //delay(5000); //delay de debug
  
  delay(1000);
  descerElevador();
  delay(2000);

  encherCopo(6500);
  delay(3000);

  if(pedido.aquecer)
  for (int i = 0; i <3 ; i++)
    aquecer(15000);
  delay(2000);
  
  subirElevador();
  //delay(2000);


  //------------------estação: SABORES----------
  int irs[] = {IR0, IR1, IR2, IR3, IR4, IR5, IR6};

  for(int i = 0; i < 5; i++) {
    ligarEsteira();
    delay(5);
    /*Serial.print("vetor escolha ");
    for(int i = 0; i<5; i++) {
      Serial.print(pedidoAtual.sabores[i]);
    }
    Serial.println();
    Serial.print("vetor escolha ");
    for(int i = 0; i<5; i++) {
      Serial.print(pedido.sabores[i]);
    }
    Serial.println();
    Serial.println(pedido.sabores[i] == 1);*/
    if(pedido.sabores[i] == 1) {
      while(readMux(irs[i+1], MUX_IRS) == HIGH) {
      }
      //Serial.print("Entrou no sabor i");
      //Serial.println(i);
      desativarMUX(MUX_ESTEIRA);
      //desligarEsteira();
      delay(1000);
      dispensarSabor(i+1);  //i+1 sim: confia!
      delay(1000);
    }
  }
 
  //----------ESTAÇÃO MEXEDOR-------------//
  desativarMUX(MUX_ESTEIRA);
  delay(1000);
  levantarMexedor();
  delay(1000);
  ligarEsteira();
  while(readMux(IR6, MUX_IR6) == HIGH) {
  }
  desativarMUX(MUX_ESTEIRA);
  //desligarEsteira();
 
  delay(1000);
  mexer(15000);

  delay(2000);

  if(debug) 
  Serial.print("PRONTO");

  //-------------ESTAÇÃO ENTREGA------------
  ligarEsteira();
  int timer = millis();
  int timerMax = timer+5000;
  while(readMux(IR7, MUX_IR7) == HIGH && timer<timerMax) {
    timer = millis();
  }
  desativarMUX(MUX_ESTEIRA);
  abaixarMexedor();
  delay(1000);
  //setPedidoPronto();
  pedidoPronto = true;
  Serial.print("PRONTO");
  if(debug) {Serial.println(" ");}
  temPedido = false;
  do  {
    playBuzzer();
    delay(4000);
  } while  (readMux(IR7, MUX_IR7) == LOW);
    //enquanto o copo ta ali    
  
  desativarMUX(mux1);
  desativarMUX(mux2);
  }
  
//----------------ate aqui tudo ok-----------------------


void getPedido() {
  //verificar se a leitura serial é da esquerda pra direita
  pedidoPronto = false;
  char escolha;
  char temp;
  if(Serial.available()>0) {
    temPedido = true;
    //PRIMEIRO RECEBE CLIENTE:    
    Serial.readBytes(&temp, 1);
    pedidoAtual.idCliente = temp;

    //DEPOIS RECEBE PEDIDO:
    Serial.readBytes(&escolha, 1);
    //pedidoAtual.aquecer = bitRead(escolha, 5);  //bitread: 0 é o menos significativo
    /*for(int i = 4; i>=0; i--) {
      Serial.print(bitRead(escolha, i), BIN);
      if (bitRead(escolha, i)==true) {
        pedidoAtual.sabores[4-i] = true;
        
      }
        
      else {
        pedidoAtual.sabores[4-i] = false;
      }
      Serial.println(pedidoAtual.sabores[4-i]);
    }*/
    for (int i = 0; i < 5; i++)
    {
        if ((escolha % 2) == 1)
          pedidoAtual.sabores[4-i] = true;
        else
          pedidoAtual.sabores[4-i] = false;
        
        escolha = escolha/2;
    }
    if ((escolha % 2) == 1)
          pedidoAtual.aquecer = true;
    else
        pedidoAtual.aquecer = false;
  
    }
  if(debug) {
    Serial.print("char escolha ");
    Serial.println(escolha);
    Serial.print("id cliente ");
    Serial.println(pedidoAtual.idCliente);
    Serial.print("aquecer ");
    Serial.println(pedidoAtual.aquecer);
    Serial.print("vetor escolha ");
    for(int i = 0; i<5; i++) {
      Serial.print(pedidoAtual.sabores[i]);
    }
    Serial.println();
  }
  while (Serial.read() >= 0) {} ; // do nothing  
  
  return;
}


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

int voltasEncher = 10;
int voltasEsvaziar = 10;
void poParafusos(int voltas = 10)
{/*
  for (int i = 0; i < 5; i++) {
    for(int j = 0; j < voltas; j++) {
      dispensarSabor(i+1);
    }
    delay(1500);
  }
    */
   while(true)    {
    dispensarSabor(5);
   }
    
}
void setup() {
  Serial.begin(9600);
  
  setupMUX();

  pedidoAtual.aquecer = false;
  for(int i = 0; i<5; i++) {
      pedidoAtual.sabores[i] = false;
    }
  delay(10);
  descerElevador();
  subirElevador();
  desativarMUX(MUX_ELEVADOR);
  /*setupCupDispenser();
  encolherDispenser();
  delay(30);
  desativarMUX(MUX_COPO_L);
  desativarMUX(MUX_COPO_R);*/
  /*
  descerElevador();
  subirElevador()
  desativarMUX(MUX_ELEVADOR);*/

  playBuzzer();
  //poParafusos(voltasEncher); //Encher os parafusos
  //poParafusos(voltasEsvaziar); //Esvaziar os parafusos
   
  
  //FAZER: processo de inicialização e modo de abastecimento
  //setupElevador();
/*
  setupEsteira();
  setupCupDispenser();
  setupElevador();
  setupSabores();
  setupMexedor();
  
  setupBomba();
  setupEbulidor();

 */
 
  //seletorMux(ELEVADOR, MUX_ELEVADOR);
  //digitalWrite(MUX_ELEVADOR, HIGH);
}
void loop() {
  
  //pedido pedidoAtual;
  //pedidoAtual.aquecer = true;
  //for (int i = 0; i < 5; i++)  
    //pedidoAtual.sabores[i] = false;
  //pedidoAtual.sabores[0] = true;
  //pedidoAtual.sabores[3] = true;
  //pedidoTeste.sabores[4] = true;
  
  //modoTeste();
  /*for (int i = 0; i<1000; i++) {
    getPedido();
    delay(30);
  }*/
  getPedido();
  //prepararPedido(pedidoAtual);
    delay(500);
  if(temPedido) {
    prepararPedido(pedidoAtual);
    delay(500);
  }
  desativarMUX(mux1);
  desativarMUX(mux2);
  Serial.begin(9600);
  
  setupMUX();

  pedidoAtual.aquecer = false;
  for(int i = 0; i<5; i++) {
      pedidoAtual.sabores[i] = false;
    }
  delay(10);

 // delay(15000);
//setPedidoPronto();


  //pedidoAtual = NULL;
/*
  seletorMux(8, mux1); //confunde com 2
  pinMode(mux1, OUTPUT);
  seletorMux(8, mux2); //confunde com 1
  pinMode(mux2, OUTPUT);
  
  
  
  digitalWrite(mux1, HIGH);
  digitalWrite(mux2, HIGH);
  Serial.println("aaa");
  while(true) {}
*/
  //testePasso();
  
  //exit(0);
}
