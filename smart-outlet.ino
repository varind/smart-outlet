
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#define RELAYPIN 2

const char* host = "outlet";
const char* ssid = "What's Really Good?";
const char* password = "sausages";

const char *ESPssid = "I'm an outlet";
const char *ESPpassword = "sausages";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

long timer;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);

  WiFi.softAP(ESPssid, ESPpassword);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}


void loop() {

  const uint16_t port = 80;
  const char * host = "www.google.com"; // ip or dns

  httpServer.handleClient();
  delay(1);

  if (timer + 30000 < millis()) {
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      digitalWrite(RELAYPIN, LOW);  // OFF
      Serial.println("Turning off, waiting 5 sec...");
      delay(5000);
      digitalWrite(RELAYPIN, HIGH);  // ON
      Serial.println("Tuning on, waiting 130 sec...");
      delay(130000);
      return;
    }

    // This will send the request to the server
    client.print("Send this data to server");

    //read back one line from server
    String line = client.readStringUntil('\r');
    client.println(line);
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("closing connection");
    client.stop();

    Serial.println("wait 30 sec...");
    timer = millis();
  }
  //  delay(30000);
}

