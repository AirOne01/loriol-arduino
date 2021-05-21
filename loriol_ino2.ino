#include <SPI.h>
#include <Ethernet.h>
int Moteur = 3;
int Valeur_Moteur;

// Mac address of the ethernet shield and corresponding ip address
byte mac[] {
  0x90, 0xA2, 0xDA, 0x0F, 0x14, 0xEA
};
IPAddress ip(192, 168, 13, 247);

int sndPort1 = A0;
int nbr = 0;

EthernetServer server(80);

void setup() {
  // # Analog setup
  pinMode(sndPort1, INPUT);
  pinMode(Moteur, OUTPUT);
  // # Ethernet setup
  Ethernet.init(10);

  Serial.begin(9600);
  while (!Serial) {
    ; // Waiting for the serial port to connect
  }

  // Starts the ethernet connection
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found. Try to reconnect it and run the program again.");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // Starts the serverx
  server.begin();
}

void loop() {
  // listen for clients
  EthernetClient client = server.available();
  if (client) { // request
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean firstSlashPassed = false;
    boolean copyNow = false;
    String path;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Copy here
        if (copyNow) path += c;
        if (c == '/' && !firstSlashPassed) {
          firstSlashPassed = true;
          copyNow = true;
        }
        if (c == '$') {
          copyNow = false;
          path[path.length() - 1] = '\0';
          if (path.length() > 5) {
            path = "";
          } else {
            nbr = path.toInt();
          }
          Serial.print("path : \"");
          Serial.print(path);
          Serial.print("\", nbr : \"");
          Serial.print(nbr);
          Serial.println("\"");
        }
        if (path == "favicon.ico") {
          copyNow = false;
          return;
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // # Sends response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("");
          client.print("<html><body>bruh</body></html>");
          client.println("");
          client.stop();
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
  }
  // > nbr récupéré ici <
}
