#include <NewPingESP8266.h> // Capteur ultrason

int pinSpeaker = D4;
int ledPin = D1;
NewPingESP8266 sonar(D2, D3, 200);
int distance = 0;
int sState = LOW;
int lState = LOW;
long prev = 0;

void setup() {
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);
  pinMode(pinSpeaker, OUTPUT);
}

void loop() {
  distance = getDistance();
  unsigned long currentTime = millis();
  Serial.println(distance);

  if (currentTime - prev > distance * 10 && distance > 0 && distance <= 25) {
    prev = currentTime;
    if (sState == LOW && distance != 0) {
      sState = HIGH;
    } else {
      sState = LOW;
    }
    digitalWrite(ledPin, sState);
    playTone(distance, 20 / distance * 1500);
    //printMessageDistance(distance);
  }
}


int getDistance() {
  delay(40);
  return sonar.ping() / US_ROUNDTRIP_CM;
}

void playTone(int duration, int tone_) {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
    while (elapsed_time < duration) {
      digitalWrite(pinSpeaker, HIGH);
      delayMicroseconds(tone_ / 2);
      digitalWrite(pinSpeaker, LOW);
      delayMicroseconds(tone_ / 2);
      elapsed_time += (tone_);
    }
  }
  else {
    delayMicroseconds(duration);
  }
}
