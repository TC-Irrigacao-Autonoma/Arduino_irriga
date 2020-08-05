//=======BIBLIOTECAS=======================================
#include <Arduino.h>
#include <DHT.h> //biblioteca sensor de temperatura e umidade do ar
#include <SPI.h>
#include <Ethernet.h>
//========================================================= 
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //endereço mac da ethernet shield
//byte servidor[] = {10, 0, 10, 216};
char servidor[] = "agroirriga.gaviaopeixoto.sp.gov.br"; //servidor para comunicação com arduino
#define portaHTTP 80
EthernetClient clienteArduino; //criando um objeto do tipo EthernetClient

//======== DECLARANDO O NOME DOS PINOS ==========================================
#define uSolo A0 //sensor de umidade do solo
#define sChuva A1 //sensor de chuva
#define solenoide 7 //rele para a valvula solenoide
#define sTempUmidade 6 //sensor temperatura e umidade do ar (DHT11)
#define sVazaoAgua 8 //sensor vazao e fluxo de água
#define DHTTYPE DHT11   // DHT 11 
DHT dht(sTempUmidade, DHTTYPE);
//=========================================================

int contTempo = 0; //contador de tempo 1seg para cada 1000 milisegundos

void setup() {
  Serial.begin(9600);
//======== SETUP CONEXÃO COM SERVIDOR ======================
  Ethernet.begin(mac);                          //inicia a conexão do Arduino (shield) com a rede local 
  if(Ethernet.begin(mac) == 0){                 //Se o endereço mac não for encontrado pela rede local 
    Serial.println("Falha ao conectar a rede"); //é impresso na porta serial 'falha ao conectar',     
    Ethernet.begin(mac);                        //e tenta novamente fazer a conexão na rede local
  }
  Serial.print("Conectado a rede, no ip: ");
  Serial.println(Ethernet.localIP());
//=========================================================

// ----------- CONFIGURAÇÃO DOS SENSORES ------------------
  dht.begin();                //inicia captação de temperatura e umidade do ar
  pinMode(uSolo, INPUT);      //declara umidade do solo como sinal de entrada
  pinMode(sChuva, INPUT);     //  #     sensor de chuva como sinal de entrada
  pinMode(sVazaoAgua, INPUT); //  #     sensor de vazao de água como sinal de entrada
  pinMode(solenoide, OUTPUT); //  #     a valvula solenoide a partir do rele como sinal de saída
// --------------------------------------------------------
}

void loop() {
  //------- DECLARANDO AS VARIAVEIS -----------------------
  int vazaoAgua = analogRead(sVazaoAgua); //atribui valor da vazao de agua a variavel 'vazaoAgua'
  int umidadeSolo = analogRead(uSolo);    // #      #      umidade solo    #   #      'umidadeSolo'
  int chuva = analogRead(sChuva);         // #      #   sensor de chuva    #   #      'chuva'
  int temperatura = dht.readTemperature();// #      #       temperatura    #   #      'temperatura'
  int umidadeAr = dht.readHumidity();     // #      #     umidade do ar    #   #      'umidadeAr'
  //-------------------------------------------------------
    
if(contTempo == 300){ //se o contador chegar a 300seg (5min) faz a chamada do método enviandoDados 
   enviandoDados(vazaoAgua, umidadeSolo, chuva, temperatura, umidadeAr); //método que envia os dados para o arquivo salvar.php
} 
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

      Serial.print("Umidade de solo = ");
      Serial.print(umidadeSolo);
      Serial.print(" Sensor de chuva = ");
      Serial.print(chuva);
      Serial.print(" Temperatura = ");
      Serial.print(temperatura);
      Serial.print(" Umidade do ar = ");
      Serial.print(umidadeAr);
      Serial.print(" Vazão de água = ");
      Serial.print(vazaoAgua); 
      Serial.println();
  
  contTempo++;
  delay(1000); //atraso de 1seg
}

void enviandoDados(int vazaoAgua, int umidadeSolo, int chuva, int temperatura, int umidadeAr){
   //========== ENVIANDO DADOS PARA ARQUIVO WEB PHP ===================
  if(clienteArduino.available()){
    char dadosRetornados = clienteArduino.read();
    Serial.print(dadosRetornados);}
  if(!clienteArduino.connected()){
    clienteArduino.stop();}
  char comando = Serial.read();
    Serial.println("Conectando ao servidor e enviando dados ...");

    if(clienteArduino.connect(servidor, portaHTTP)){ //se conectar ao servidor envia os dados para arquivo salvar.php
      //--------- PARAMETROS DO CÓDIGO PHP -------------------------------------------
      //Passa os valores das variaveis para a pagina salvar.php pelo método GET
      clienteArduino.print("GET /Arduino/salvar.php");
      clienteArduino.print("?sensorUmidadeSolo=");
      clienteArduino.print(umidadeSolo);
      clienteArduino.print("&sensorChuva=");
      clienteArduino.print(chuva);
      clienteArduino.print("&sensorTemperatura=");
      clienteArduino.print(temperatura);
      clienteArduino.print("&sensorUmidadeAr=");
      clienteArduino.print(umidadeAr);
      clienteArduino.print("&fluxoVazaoDeAgua=");
      clienteArduino.print(vazaoAgua);
      clienteArduino.println(" HTTP/1.0");
      clienteArduino.println("Host: agroirriga.gaviaopeixoto.sp.gov.br"); //faz uma requisição conectando com o servidor 
      clienteArduino.println("Connection: close");                     
      clienteArduino.println();                                           
      clienteArduino.stop();                                 // fecha a conexão para depois fazer a requisição novamente                    
    } else {
      Serial.println("Falha na conexao com o servidor"); //se não conectar ao servidor
      clienteArduino.stop();                             //para com a requisição do clienteArduino
    }
    contTempo = 0; //quando método é executado zera o conTempo para contar mais 5 min até próxima chamada
}