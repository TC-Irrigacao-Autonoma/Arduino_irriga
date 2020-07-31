#include <Arduino.h>

EthernetClient clienteArduino;

//=======================================================
// ÁREA RESERVADA PARA A DECLARAÇÃO DOS SENSORES
float sensor1 = 1;
float sensor2 = 3;
float sensor3 = 5;
//=======================================================

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);

  if(Ethernet.begin(mac) == 0{
    Serial.println("Falha ao conectar a rede");   
    Ethernet.begin(mac); 
  }

  Serial.print("Conectado a rede, no ip: ");
  Serial.println(Ethernet.localIP());
  
}

void loop() {
  //=======================================================
  // ÁREA RESERVADA PARA A LEITURA DOS SENSORES

      sensor1++;  sensor2++; sensor3++;
  //=======================================================
/*
  if(clienteArduino.available()){
    char dadosRetornados = clienteArduino.read();
    Serial.print(dadosRetornados);
  }

  if(!clienteArduino.connected(){
    clienteArduino.stop();
  }

  char comando = Serial.read();

  if(comando == '1'){
*/
    

    Serial.println("Conectando ao servidor e enviando os dados: ");
    Serial.print("Sensor1: ");
    Serial.println(sensor1);
    Serial.print("Sensor2: ");
    Serial.println(sensor2);
    Serial.print("Sensor3: ");
    Serial.println(sensor3);
    
    if(clienteArduino.connect(servidor, portaHTTP)){

      // http://10.0.10.216/projetos/Web_Irrigacao/dadosArduino.php
  
      //clienteArduino.println("GET /projetos/Web_Irrigacao/dadosArduino.php HTTP/1.0");
      
      //PARAMETROS DO CÓDIGO PHP
      clienteArduino.print("GET /projetos/Web_Irrigacao/Arduino/salvar.php");
      clienteArduino.print("?s1=");
      clienteArduino.print(sensor1);
      clienteArduino.print("&s2=");
      clienteArduino.print(sensor2);
      clienteArduino.print("&s3=");
      clienteArduino.print(sensor3);
      clienteArduino.println("HTTP/1.0");
      
      clienteArduino.println("Host: 10.0.10.216");
      clienteArduino.println("Connection: close");
      clienteArduino.println();

      clienteArduino.stop();
      
    } else {
      Serial.println("Falha na conexao com o servidor");

      clienteArduino.stop();
    } 
 // } final if
  
  delay(5000);
}
