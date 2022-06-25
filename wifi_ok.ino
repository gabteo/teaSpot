#include <ESP8266WiFi.h>
#include <CircularBuffer.h>
#define MAX_CLIENTS 5

const char* ssid = "teaSpot";
const char* password = "12345678";
WiFiServer server(80);
bool chaEmPreparo = false;
bool debug = true;
struct pedido {
  byte cliente;
  byte escolha;
};


WiFiClient *clients[MAX_CLIENTS] = { NULL };

CircularBuffer<pedido,5> filaPedidos;
pedido pedidoAtual;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  
  IPAddress staticIP(192, 168, 4, 2); // IP set to Static
  IPAddress gateway(192, 168, 4, 1);// gateway set to Static
  IPAddress subnet(255, 255, 255, 0);// subnet set to Static

  WiFi.mode(WIFI_AP);// Working mode only as Acess Point 

  WiFi.softAP(ssid, password, 2, 0, 8);
  WiFi.config(staticIP, gateway, subnet);

  server.begin();

  Serial.print("Chegou aquino setup"); 
  //Serial.println("Server started"); 
  //Serial.println(WiFi.softAPIP());

  //Serial.print("MAC: ");
  //Serial.println(WiFi.macAddress());
}

void loop() {
    //Serial.print("Chegou aqui no loop "); 


 WiFiClient newClient = server.available();
 // wait for a client (web browser) to connect
 if (newClient)
 {
   if(debug) {Serial.println("\n[New client connected]");}
   for (int i=0 ; i<MAX_CLIENTS ; i++) {
       if (NULL == clients[i]) {
           clients[i] = new WiFiClient(newClient);
           break;
       }
   }
 }  
  Serial.println("loop");
  if(sizeof(clients)>0) {
  for (int i=0 ; i<MAX_CLIENTS ; i++)  //pra cada cliente
  {
    Serial.print(i);
    // read line by line what the client (web browser) is requesting
    if (clients[i] != NULL && clients[i]->available() )
    {
      if(debug) {Serial.println("entrou aqui ");}  
      String message = clients[i]->readStringUntil('\r');
      delay(1);// give the web browser time to receive the data
      clients[i]->print("HTTP/1.1 200 helllooooooooo");
      clients[i]->print("\r\nConnection: Closed\r\n\r\n");
      if(message.indexOf("pedido")!=-1) {
        if(filaPedidos.size()<5) {
          //TRATAR A STRING/pedido QUE RECEBE
          if(debug) {Serial.println("recebeu pedido ");}          
          String strPedido = message.substring(message.indexOf("pedido") + 6, message.indexOf("pedido") + 12);
          Serial.println(strPedido);
          //temperatura, 3 sabores, 2 açucar
          pedido novoPedido;
          novoPedido.escolha = 0;
          for(int j = 0; j<6; j++) {
            novoPedido.escolha += strPedido.charAt(j)-'0';
            novoPedido.escolha<<1;
          }
          novoPedido.escolha>>1;
          novoPedido.cliente = i;
          filaPedidos.unshift(novoPedido);  //usar buffer.pop();
          
          clients[i]->print("HTTP/1.1 200 ");
          clients[i]->print("RECEBIDO");
          clients[i]->print("\r\nConnection: Closed\r\n\r\n");
          if(debug) {Serial.println("Resposta de recebimento do pedido ");}  
          
        } else {
          clients[i]->print("HTTP/1.1 200 ");
          clients[i]->print("RECUSADO");
          clients[i]->print("\r\nConnection: Closed\r\n\r\n");
          if(debug) {Serial.println("resposta pedido recusado ");}  
        }/*
          clients[pedidoAtual.cliente]->print("HTTP/1.1 200 ");
          clients[pedidoAtual.cliente]->print("RECEBIDO");
          clients[pedidoAtual.cliente]->print("\r\nConnection: Closed\r\n\r\n");
          if(debug) {Serial.println("Resposta de recebimento do pedido ");}  
          
        } else {
          clients[pedidoAtual.cliente]->print("HTTP/1.1 200 ");
          clients[pedidoAtual.cliente]->print("RECUSADO");
          clients[pedidoAtual.cliente]->print("\r\nConnection: Closed\r\n\r\n");
          if(debug) {Serial.println("resposta pedido recusado ");}  
        }*/
        //clients[i]->flush();
      }       
  }
  }
  if(!filaPedidos.isEmpty() && !chaEmPreparo) {
    //manda pro arduino
    chaEmPreparo = true;
    pedidoAtual = filaPedidos.pop();
    Serial.write(pedidoAtual.cliente);
    Serial.write(pedidoAtual.escolha);
    if(debug) {Serial.println("enviou pedido pro arduino ");}  
    clients[pedidoAtual.cliente]->print("HTTP/1.1 200 ");
      clients[pedidoAtual.cliente]->print("PRO ARDUINO");
      clients[pedidoAtual.cliente]->print("\r\nConnection: Closed\r\n\r\n");
  }
  } //do if clients size>0

  if(Serial.available()) {
    String arduinoMsg = Serial.readString();
    if(arduinoMsg.equals("PRONTO")) {
      clients[pedidoAtual.cliente]->print("HTTP/1.1 200 ");
      clients[pedidoAtual.cliente]->print("PRONTO");
      clients[pedidoAtual.cliente]->print("\r\nConnection: Closed\r\n\r\n");
      if(debug) {Serial.println("resposta pedido pronto ");}  
      chaEmPreparo = false;
      //avisar q ta pronto
    }
  }

   /*------------OK ate aqui-------------*/
  
// If you want to disconnect the client here, then do this:
      /* clients[i]->stop();
       delete clients[i];
       clients[i] = NULL;
       Serial.println("[Client disconnected]");
     */
}
   
  