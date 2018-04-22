#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

// wifi connection variables
const char* ssid = "tick";
const char* password = "boomboom";
boolean wifiConnected = false;

// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back

// use PWM pins 3, 4, 6, 9, 10 ???
int pins[] = {5};
#define numNotes 1
// how long to pulse in ms
unsigned int onTime = 25;
// keep track of the last time a motor channel (note) was triggered
unsigned long lastHit[numNotes];
// keep track if a note is currently on (high) or if it has been released
bool noteHigh[numNotes];


void setup() {
  // Initialise Serial connection
  Serial.begin(115200);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  // only proceed if wifi connection successful
  if (wifiConnected) {
    udpConnected = connectUDP();
    if (udpConnected) {
      // initialise pins
      //pinMode(5, OUTPUT);
      // initialize output pins to control the motors
      for (int i = 0; i < numNotes; i++) {
        pinMode(pins[i], OUTPUT);
        noteHigh[i] = false;
        lastHit[i] = 0;
      }
    }
  }
}

void loop() {
  // for all motor driver pins
  // check if onTime since lastHit has elapsed
  for (int i = 0; i < numNotes; i++) {
    // if so, turn the motor off
    //Serial.println((millis() - lastHit[0]));
    if (noteHigh[i] && (millis() - lastHit[i] > onTime)) {

      noteHigh[i] = false;
      analogWrite(pins[i], 0);
    }
  }

  // check if the WiFi and UDP connections were successful
  if (wifiConnected) {
    if (udpConnected) {

      // if there’s data available, read a packet
      int packetSize = UDP.parsePacket();
      if (packetSize)
      {
        Serial.println("");
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = UDP.remoteIP();
        for (int i = 0; i < 4; i++)
        {
          Serial.print(remote[i], DEC);
          if (i < 3)
          {
            Serial.print(".");
          }
        }
        Serial.print(", port ");
        Serial.println(UDP.remotePort());

        // read the packet into packetBufffer
        UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents: ");
        int value = packetBuffer[0] * 256 + packetBuffer[1];
        Serial.println(value);

        // send a reply, to the IP address and port that sent us the packet we received
        //        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        //        UDP.write(ReplyBuffer);
        //        UDP.endPacket();

        // turn LED on or off depending on value recieved
        //        digitalWrite(5, value);

        if (value == 1) {
          analogWrite(pins[0], 300+random(800));

          // keep track of which note is high and reset the time since the lastHit to now
          noteHigh[0] = true;
          lastHit[0] = millis();
        }
      }
      //      delay(5);

    }

  }

}

// connect to UDP – returns true if successful or false if not
boolean connectUDP() {
  boolean state = false;

  Serial.println("");
  Serial.println("Connecting to UDP");

  if (UDP.begin(localPort) == 1) {
    Serial.println("Connection successful");
    state = true;
  }
  else {
    Serial.println("Connection failed");
  }

  return state;
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  return state;
}
