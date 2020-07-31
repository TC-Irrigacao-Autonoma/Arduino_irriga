//=======BIBLIOTECAS=======================================
#include <Arduino.h>
#include <dht.h> //biblioteca sensor de temperatura e umidade do ar
#include <SPI.h>
#include <Ethernet.h> 

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//byte servidor[] = {10, 0, 10, 216};
char servidor[] = "https://agroirriga.gaviaopeixoto.sp.gov.br";
#define portaHTTP 80

EthernetClient clienteArduino;

//=========================================================
// ÁREA RESERVADA PARA A DECLARAÇÃO DOS SENSORES
#define uSolo A0
#define sChuva A1
#define solenoide 7
#define sTempUmidade 6
#define sVazaoAgua 8

float sensor1 = 1;
float sensor2 = 3;
float sensor3 = 5;
//=========================================================

// ---- DECLARAÇÃO DO OBJETO -----
dht my_dht;//objeto para o sensor de temperatura e umidade do ar

void setup() {
//========SETUP CONEXÃO COM SERVIDOR ======================
  Serial.begin(9600);
  Ethernet.begin(mac);
  if(Ethernet.begin(mac) == 0{
    Serial.println("Falha ao conectar a rede");   
    Ethernet.begin(mac); 
  }
  Serial.print("Conectado a rede, no ip: ");
  Serial.println(Ethernet.localIP());
//=========================================================


// ----------- CONFIGURAÇÃO DOS SENSORES ------------------
  pinMode(uSolo, INPUT);
  pinMode(sChuva, INPUT);    
  pinMode(sVazaoAgua, INPUT);
  pinMode(solenoide, OUTPUT);
// --------------------------------------------------------
}

void loop() {
  //=======================================================
  // ÁREA RESERVADA PARA A LEITURA DOS SENSORES

  my_dht.read11(sensorTempUmidade);
  //------- DECLARANDO AS VARIAVEIS -----------------------
  int vazaoAgua = analogRead(sVazaoAgua);
  int umidadeSolo = analogRead(uSolo);
  int chuva = analogRead(sChuva);
  int temperatura = my_dht.temperature;
  int umidadeAr = my_dht.humidity;

      sensor1++;  sensor2++; sensor3++;
  //=======================================================

   //----- IMPRIMINDO VALORES SENSORES -----
  /*
   Serial.print("Sensor Chuva:");
   Serial.print(sensorChuvaA);
   Serial.print("   Umidade do solo:");
   Serial.print(umidadeSoloA);
   Serial.print("   Atuador:");

   if(umidadeSoloA > 700){
    digitalWrite(solenoide, LOW);
    Serial.print("Solenoide ABERTA");
   }
   else{
    digitalWrite(solenoide, HIGH);
    Serial.print("Solenoide FECHADA");
   }
    Serial.print("   Temperatura:");
    Serial.print(temperatura);
    Serial.print("ºC ");
    Serial.print("   Umidade do Ar:");
    Serial.print(umidadeAr);
    Serial.print("%");
    Serial.println("  ");
    */

    Serial.println("Conectando ao servidor e enviando os dados: ");
    Serial.print("Sensor1: ");
    Serial.println(sensor1);
    Serial.print("Sensor2: ");
    Serial.println(sensor2);
    Serial.print("Sensor3: ");
    Serial.println(sensor3);
    
    //========== ENVIANDO DADOS PARA ARQUIVO WEB PHP ===================
    if(clienteArduino.connect(servidor, portaHTTP)){

      // http://10.0.10.216/projetos/Web_Irrigacao/dadosArduino.php
  
      //clienteArduino.println("GET /projetos/Web_Irrigacao/dadosArduino.php HTTP/1.0");
      
      //PARAMETROS DO CÓDIGO PHP
      clienteArduino.print("GET /projetos/Web_Irrigacao/Arduino/salvar.php");
      clienteArduino.print("&sensorUmidadeSolo=");
      clienteArduino.print(umidadeSolo);
      clienteArduino.print("&sensorChuva=");
      clienteArduino.print(chuva);
      clienteArduino.print("&sensorTemperatura=");
      clienteArduino.print(temperatura);
      clienteArduino.print("&sensorUmidadeAr=");
      clienteArduino.print(umidadeAr);
      clienteArduino.print("&fluxoVazaoDeAgua=");
      clienteArduino.print(vazaoAgua);
      clienteArduino.println("HTTP/1.0");
      
      clienteArduino.println("Host: https://agroirriga.gaviaopeixoto.sp.gov.br/");
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
