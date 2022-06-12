#include <Servo.h>

//posicoes dos servos do dispenser
int maxLeft = 143*1.1;
int minLeft = 100;
int maxRight = 55;
int minRight = 21*0.9;
void esticarDispenser();
void encolherDispenser();
void dispensarCopo();



Servo dispenserLeft, dispenserRight;

void setup() {
  // put your setup code here, to run once:

  dispenserLeft.attach(7);
  dispenserRight.attach(8);
  encolherDispenser();
  Serial.println("Setup cup dipenser: ok");
}

void loop() {
  int comando = 0;
    
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  // put your main code here, to run repeatedly:
  switch(comando) {  
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

    case 'q': { //MaxL +
      maxLeft +=2;
      break;  
    }
    case 'w': { //MaxL -
      maxLeft -=2;
      break;  
    }
    case 'e': { //minL +
      minLeft +=2;
      break;  
    }
    case 'r': { //minL -
      minLeft -=2;
      break;  
    }

    case 'a': { //MaxR +
      maxRight +=2;
      break;  
    }
    case 's': { //MaxR -
      maxRight -=2;
      break;  
    }
    case 'd': { //minR +
      minRight +=2;
      break;  
    }
    case 'f': { //minR -
      minRight -=2;
      break;  
    }
  }
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

void dispensarCopo() {
  esticarDispenser();
  delay(1500);
  encolherDispenser();
  Serial.println("Copo dispensado");
  return;
}
