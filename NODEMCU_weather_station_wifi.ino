#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <Servo.h>

// define WiFi connection
#ifndef STASSID
#define STASSID "jacob5"
#define STAPSK  "Timluc0101"
#endif

// Construct an LCD object and pass it the 
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT setup:
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);

int led_pin = 13;
int motion_pin = 12;
int motion_val = 0;
int motion_state = LOW;

// for posting
float h;
float t;

int interval = 300000;
int previousMillis = 0;

// WiFi Server setup

const char* ssid = STASSID;
const char* password = STAPSK;
const char* group = "weerstation-server.herokuapp.com";

ESP8266WebServer server(80);

void handleRoot() {
  digitalWrite(led_pin, 1);
  server.send(200, "text/plain", "hello from espTim!");
  digitalWrite(led_pin, 0);
}

void handleNotFound() {
  digitalWrite(led_pin, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led_pin, 0);
}

// Servo setup
Servo servo;

void setup() {
  pinMode(motion_pin, INPUT);     // declare PIR sensor pin as input
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  // Setup wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("TimESP")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  // Setup servo
  servo.attach(14); //D5
  servo.write(0);
  
  
  // Setup for LCD
  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16,2);
  lcd.init();
  
  // Turn on the backlight.
  lcd.backlight();

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  // lcd.setCursor(5, 0);

  // Print HELLO to the screen, starting at 5,0.
  //lcd.print("HELLO WELCOME TO MCDONALDS");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(5, 1);      
  //lcd.print("WORLD");
  delay(2000);
}

void loop() {
  server.handleClient();
  MDNS.update();
  motion_val = digitalRead(motion_pin);

  if (motion_val == HIGH)
  {
    servo.write(90);
    delay(20000);
    servo.write(0);
  }
  
  delay(2000);
  
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(t);      // set characters

//  Serial.print(t);
//  Serial.print(F("°C "));
  
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(h);

  if (millis() - previousMillis >= interval){
    previousMillis = millis();
    Serial.print("posting now");
    Connect_To_Server();
  }
}

void Connect_To_Server()
{

  Serial.print("connecting to ");
  Serial.print(group);
  Serial.print(':');

  WiFiClient client;
  if (!client.connect(group, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  Serial.println("sending data to server");
  if (client.connected()) {


    String PostData = "{\"temperature\": ";
    PostData.concat(t);
    PostData.concat(", \"humidity\": ");
    PostData.concat(h);
    PostData.concat("}");

    Serial.println("[Sending a post request]");
    client.print(String("POST /api/Measurements/f67daefa-0924-47b6-9702-8fbbc125e007" ) + " HTTP/1.1\r\n" +
                 "Host: " + group + "\r\n" +
                 "Content-Type: application/json \r\n" +
                 "Content-Length: " + PostData.length() + "\r\n"
                );
    client.println();
    client.println(PostData);
  }

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(60000);
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("receiving from remote server");
  // not testing 'client.connected()' since we do not need to send data here
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }
  Serial.println();
  Serial.println("closing connection");

  client.stop();
}
