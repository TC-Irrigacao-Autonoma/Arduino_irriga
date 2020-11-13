#include <SPI.h>
#include <Ethernet.h>
String readString;

int rele_sala =4;
int rele_quarto =6;
int rele_cozinha =8;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(10,0,10,216/projetos/Web_Irrigacao/enviandoParaArduino/setor.html);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  pinMode(rele_sala, OUTPUT);
  pinMode(rele_quarto, OUTPUT);
  pinMode(rele_cozinha, OUTPUT);
  digitalWrite(rele_sala, HIGH);
  digitalWrite(rele_quarto, HIGH);
  digitalWrite(rele_cozinha, HIGH); 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (readString.length() < 100) {
          readString += c;
        }
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          if (readString.indexOf("?sala") > 0)
          {
            if(digitalRead(rele_sala)){
              digitalWrite(rele_sala, LOW);
              Serial.println("Sala desligada");
            }else{
              digitalWrite(rele_sala, HIGH);
              Serial.println("Sala ligada");
            }
          }
          
          if (readString.indexOf("?quarto") > 0)
          {
            if(digitalRead(rele_quarto)){
              digitalWrite(rele_quarto, LOW);
              Serial.println("Quarto desligada");
            }else{
              digitalWrite(rele_quarto,HIGH);
              Serial.println("Quarto ligado");
            }
          }

          if (readString.indexOf("?cozinha") > 0)
          {
            if(digitalRead(rele_cozinha)){
              digitalWrite(rele_cozinha, LOW);
              Serial.println("Cozinha desligada");
            }else{
              digitalWrite(rele_cozinha,HIGH);
              Serial.println("Cozinha ligada");
            }
          }
    
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
       // client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("</body>");
          client.println("69");
          client.println("</body>");
          // output the value of each analog input pin
          client.println("</html>");          
          readString = "";        
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

