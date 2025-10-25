#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <secrets.h> // INFO: Edit this or the project will not function

const int trigPin = 5;
const int echoPin = 14;
const int redLEDPin = 19;   // Red LED pin
const int greenLEDPin = 18; // Green LED pin
const int blueLEDPin = 21;  // Blue LED pin

// define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;
bool occupied = false;
bool lastOccupied = false;
int occupiedThresholdCm = 12; // distance threshold to consider the parking spot occupied
bool disabledPark = false; // whether this parking spot is a disabled park

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

void publishOccupancy(bool occupied);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);

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
  
  if (distanceCm < occupiedThresholdCm) {
    Serial.println("Occupied");
    occupied = true;
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(blueLEDPin, LOW);
  } else {
    Serial.println("Unoccupied");
    occupied = false;
    if (disabledPark) {
      digitalWrite(redLEDPin, LOW);
      digitalWrite(greenLEDPin, LOW);
      digitalWrite(blueLEDPin, HIGH);
    } else {
      digitalWrite(redLEDPin, LOW);
      digitalWrite(greenLEDPin, HIGH);
      digitalWrite(blueLEDPin, LOW);
    }
    
  }
  if (occupied != lastOccupied) {
    publishOccupancy(occupied);
    lastOccupied = occupied;
  }
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length){
  // Stub for now - just print the message to serial
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  if (String(topic) == "parkSenseUTS/msgIn/A1/disabled_park") {
    if (message == "1") {
      Serial.println("This parking spot is now marked as a disabled park.");
      disabledPark = true;
    } else if (message == "0") {
      Serial.println("This parking spot is now marked as a normal park.");
      disabledPark = false;
    } else {
      Serial.println("Invalid message for disabled park status.");
    } 
  }
}

void publishOccupancy(bool occupied) {
  if (!client.connected()) {
    Serial.println("MQTT not connected, skipping occupancy publish");
    return;
  }
  const char* pubTopic = "parkSenseUTS/msgOut/occupied/A1"; // TODO: make this respect the actual parking spot ID and topic
  const char* payload = occupied ? "1" : "0";
  bool ok = client.publish(pubTopic, payload);
  if (!ok) {
    Serial.println("Publish failed");
  } else {
    Serial.print("Published occupancy ");
    Serial.print(payload);
    Serial.print(" to ");
    Serial.println(pubTopic);
  }
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

