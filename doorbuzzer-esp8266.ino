#include <ESP8266WiFi.h>
#include <string>

const char ssid[] = "";
const char pass[] = "";
String api_key = "<YOUR-PW-GOES-HERE>";

String password;
int buzztime;
const int MAX_TIME = 7000;
const int MIN_TIME = 500;
const int DEFAULT_TIME = 1500;

WiFiServer server(6543);
bool clientConnected = false;
int PIN = 3; // RX

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, LOW);

  connectWifi();
}

void loop() {
  if(clientConnected) {
    Serial.println("Client already connected");
    return;
  }
  connectWifi();

  WiFiClient client = server.available();
  if(!client) {
    return;
  }
  clientConnected = true;

  readRequest(&client);
  if(password == api_key) {
    client.write("opened");
    digitalWrite(PIN, HIGH);
    delay(buzztime);
    digitalWrite(PIN, LOW);
    delay(100);
  } else {
    delay(DEFAULT_TIME);
  }
  client.stop();
  clientConnected = false;
  delay(500); // Wait half a second before serving the next client.
}

void readRequest(WiFiClient* client) {
  password = readUntilNewline(client);
  buzztime = readUntilNewline(client).toInt();

  if(buzztime < MIN_TIME || buzztime > MAX_TIME) {
    buzztime = DEFAULT_TIME;
  }
}

String readUntilNewline(WiFiClient* client) {
  String str = "";
  char c;
  while(client->connected()) {
    while(client->available()) {
        c = client->read();
        if(c == '\n') {
          return str;
        }
        str += c;
    }
  }
  return str;
}

void connectWifi() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Trying to connect to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    long then = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
      if(millis() - then > 15000) {
        return;
      }
      delay(100);
    }

    Serial.println("Starting TCP-server");
    server.begin();
  }
}

