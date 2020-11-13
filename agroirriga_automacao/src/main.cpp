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
#define sChuva 5 //sensor de chuva
#define solenoide 7 //rele para a valvula solenoide
#define sTempUmidade 6 //sensor temperatura e umidade do ar (DHT11)
#define DHTTYPE DHT11   // DHT 11 
DHT dht(sTempUmidade, DHTTYPE);
//=========================================================

int contTempo = 0; //contador de tempo 1seg para cada 1000 milisegundos

int umidadeSolo_ideal = 60;
//int min_umidadeSolo_ideal = umidadeSolo_ideal - 10;

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
  pinMode(solenoide, OUTPUT); //  #     a valvula solenoide a partir do rele como sinal de saída
// --------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  //------- DECLARANDO AS VARIAVEIS -----------------------
  double sensorSolo = analogRead(uSolo);
  double umidadeSolo = ((1024-sensorSolo)*100)/1024;    // #      #      umidade solo    #   #      'umidadeSolo'
  int chuva = digitalRead(sChuva);         // #      #   sensor de chuva    #   #      'chuva'
  int temperatura = dht.readTemperature(); // #      #       temperatura    #   #      'temperatura'
  int umidadeAr = dht.readHumidity();      // #      #     umidade do ar    #   #      'umidadeAr'
  int valvulaSolenoide = 0;
  //-------------------------------------------------------
  

if((umidadeSolo < umidadeSolo_ideal-10) && (chuva != 0)){ // se umidadeSolo < min_umidadeSolo_ideal(%)
  while((umidadeSolo < umidadeSolo_ideal) && (chuva != 0)){ // enquanto umidadeSolo < umidadeSolo_ideal(%)
    digitalWrite(solenoide, LOW); //VALVULA ABERTA
    valvulaSolenoide = 1; //envia 1 para o web indicando a valvula aberta 

    sensorSolo = analogRead(uSolo);
    umidadeSolo = ((1024-sensorSolo)*100)/1024;    // #      #      umidade solo    #   #      'umidadeSolo'
    chuva = digitalRead(sChuva);                          // #      #   sensor de chuva    #   #      'chuva'
    temperatura = dht.readTemperature();                  // #      #       temperatura    #   #      'temperatura'
    umidadeAr = dht.readHumidity();                       // #      #     umidade do ar    #   #      'umidadeAr'

    if(contTempo == 5){                                
      enviandoDados(valvulaSolenoide, umidadeSolo, chuva, temperatura, umidadeAr); 
      contTempo = 0;
    }
    
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print("  Umidade do Ar: ");
    Serial.print(umidadeAr);
    Serial.print("  Chuva: ");
    Serial.print(chuva);
    Serial.print("  Umidade do Solo:  ");
    Serial.print(umidadeSolo);
    Serial.println("  Valvula Aberta");
   

    contTempo++;
    delay(60000); //atraso de 1min
  }
  digitalWrite(solenoide, HIGH); //VALVULA FECHADA
  valvulaSolenoide = 0; //envia 0 para o web indicando valvula fechada  
}

/*
if((umidadeSolo > x ) && (chuva > 500) ){ //irriga quando umidade do solo é baixa e não está chovendo
  digitalWrite(solenoide, LOW);            //aciona e abre a valvula solenoide
  Serial.print("Solenoide ABERTA");
}
else{
  digitalWrite(solenoide, HIGH);          //fecha a valvula solenoide
  Serial.print("Solenoide FECHADA");
}    
*/

//se o contador chegar a 300seg (5min) faz a chamada do método enviandoDados
if(contTempo == 5){                                
   enviandoDados(valvulaSolenoide, umidadeSolo, chuva, temperatura, umidadeAr); 
   contTempo = 0;
}

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print("  Umidade do Ar: ");
    Serial.print(umidadeAr);
    Serial.print("  Chuva: ");
    Serial.print(chuva);
    Serial.print("  Umidade do Solo:  ");
    Serial.print(umidadeSolo);
    Serial.println("  Valvula Fechada");
    
    


  contTempo++;
  delay(60000); //atraso de 1min
}

//------------------------------------FIM DO LOOP-----------------------------------------------------------------------------------------------






//========== ENVIANDO DADOS PARA ARQUIVO WEB PHP ===================
void enviandoDados(int valvulaSolenoide, double umidadeSolo, int chuva, int temperatura, int umidadeAr){
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
      clienteArduino.print("?valvulaSolenoide=");
      clienteArduino.print(valvulaSolenoide);
      clienteArduino.print("&umidadeSolo=");
      clienteArduino.print(umidadeSolo);
      clienteArduino.print("&chuva=");
      clienteArduino.print(chuva);
      clienteArduino.print("&temperatura=");
      clienteArduino.print(temperatura);
      clienteArduino.print("&umidadeAr=");
      clienteArduino.print(umidadeAr);
      clienteArduino.println(" HTTP/1.0");
      clienteArduino.println("Host: agroirriga.gaviaopeixoto.sp.gov.br"); //faz uma requisição conectando com o servidor 
      clienteArduino.println("Connection: close");                     
      clienteArduino.println();                                           
      clienteArduino.stop();                              // fecha a conexão para depois fazer a requisição novamente                    
    } else {
      Serial.println("Falha na conexao com o servidor"); //se não conectar ao servidor
      clienteArduino.stop();                             //para com a requisição do clienteArduino
    }
    contTempo = 0;                                       //quando método é executado zera o conTempo para contar mais 5 min até próxima chamada
}

 //----- IMPRIMINDO VALORES SENSORES -----
  /*
   Serial.print("Sensor Chuva:");
   Serial.print(sensorChuvaA);
   Serial.print("   Umidade do solo:");
   Serial.print(umidadeSoloA);
   Serial.print("   Atuador:");

    Serial.print("   Temperatura:");
    Serial.print(temperatura);
    Serial.print("ºC ");
    Serial.print("   Umidade do Ar:");
    Serial.print(umidadeAr);
    Serial.print("%");
    Serial.println("  ");
    */