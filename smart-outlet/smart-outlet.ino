//  Smart Outlet by Ben Lipsey www.varind.com 2016
//  http://www.variableindustries.com/smart-outlet/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#define RELAYPIN 2
bool debug = false;

const char* host = "outlet";
const char* ssid = "SSID";
const char* password = "PASSWORD";

const char *ESPssid = "I'm an outlet";
const char *ESPpassword = "PASSWORD";


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

long timer;
byte fails = 0;

void setup() {
  if (debug)Serial.begin(115200);
  delay(10);

  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);

  WiFi.softAP(ESPssid, ESPpassword);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    if (debug)Serial.println("WiFi failed, retrying.");
    delay(100);
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  if (debug)  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);


  if (debug)  Serial.println("");
  if (debug)  Serial.println("WiFi connected");
  if (debug)  Serial.println("IP address: ");
  if (debug) Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {

  const uint16_t port = 80;
  const char * host = "www.google.com"; // ip or dns

  httpServer.handleClient();
  delay(1);

  if (timer + 30000 < millis()) {
    if (debug) Serial.print("connecting to ");
    if (debug) Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
      if (debug) Serial.println("connection failed");
      fails = fails + 1;
      if (debug) Serial.println (fails);
      if (fails >= 3) {

        digitalWrite(RELAYPIN, LOW);  // OFF
        if (debug) Serial.println("Turning off, waiting 5 sec...");
        delay(5000);
        digitalWrite(RELAYPIN, HIGH);  // ON
        if (debug) Serial.println("Tuning on, waiting 130 sec...");
        delay(130000);
        fails = 0;
      }
      return;
    }
    if (client.connect(host, port)) {
      fails = 0;
    }
    if (debug) Serial.println (fails);
    // This will send the request to the server
    client.print("Send this data to server");

    //read back one line from server
    String line = client.readStringUntil('\r');
    client.println(line);
    digitalWrite(RELAYPIN, HIGH);
    if (debug) Serial.println("closing connection");
    client.stop();

    if (debug) Serial.println("wait 30 sec...");
    timer = millis();
  }
  //  delay(30000);
}
