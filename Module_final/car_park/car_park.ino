#include <NewPingESP8266.h> // Capteur ultrason

int pinSpeaker = D4;
int ledPin = D1;
NewPingESP8266 sonar(D2, D3, 200);
int distance = 0;
int sState = LOW;
int lState = LOW;


void setup() {
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);
 pinMode(pinSpeaker, OUTPUT);
}

void loop() {
  distance = getDistance();
  Serial.println(distance);
}


int getDistance() {
  delay(40);
  return sonar.ping() / US_ROUNDTRIP_CM;
}
