#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <secrets.h> // INFO: Edit this or the project will not function

const int trigPin = 5;
const int echoPin = 14;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

// just using the public MQTT broker for now.
// this could all be in secrets.h it's essentially an environment variable
const char* MQTT_BROKER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char* topic = "parkSenseUTS/";

String clientID;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);
void connect();

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS); // these shoud be defined in secrets.h
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to WiFi with local IP: ");
  Serial.println(WiFi.localIP());

  // TODO: maybe tie the client ID to the position in the parking area or something like that?
  clientID = "parkSenseUTS-";
  clientID += WiFi.macAddress();

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.printf("The client %s connects to the MQTT broker\n", clientID.c_str());
    if (client.connect(clientID.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("MQTT broker connected");
    } else {
      Serial.print("Failed with state: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // The device will light a green LED for an unoccupied normal park and a blue LED for an unoccupied disabled park
  // Whether a given parking spot is disabled should be user configurable
  client.subscribe("parkSenseUTS/msgIn/A1/disabled_park");
}

void loop() {
  if (!client.connected()) {
    connect();
  }
  // we had issues with callbacks when this wasn't in a serial.print and i'm not sure why
  // the control flow makes no sense but it only worked like this
  Serial.println(client.loop());

  // sets the trigPin LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // sets the trigPin to HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length){
  // Stub for now
}

void connect(){
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientID.c_str())) {
      client.subscribe("parkSenseUTS/msgIn/A1/disabled_park");
      Serial.println("Connected");
    }
    else {
      delay(5000);
    }
  }
}

