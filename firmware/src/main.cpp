#include <Arduino.h>
#include <secrets.h> // INFO: Edit this or the project will not function
const int trigPin = 5;
const int echoPin = 14;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
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