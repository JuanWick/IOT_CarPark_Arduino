#include <NewPingESP8266.h> // Capteur ultrason
#include <MPU6050_tockn.h>
#include <Wire.h>

int pinSpeaker = D4;
int ledPin = D1;
NewPingESP8266 sonar(D2, D3, 200);
int distance = 0;
int sState = LOW;
int lState = LOW;
long prev = 0;
const int interval = 800;
TwoWire i2c;
MPU6050 mpu6050(i2c);
long timeStamp = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(pinSpeaker, OUTPUT);
  i2c.begin(D6, D5);
  mpu6050.begin();
}

void loop() {
  distance = getDistance();
  unsigned long currentTime = millis();
//  Serial.println(distance);

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

  if (millis() - timeStamp > 10) {
    mpu6050.update();
//    Serial.print("2 5 ");
    //Serial.println();
          Serial.println(mpu6050.getAccZ());

    if (didMoved(mpu6050.getAccZ()) == true) {
      impactAction();
    }
    timeStamp = millis();
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

void impactAction() {
  printMessageImpact();
  //servo1.write(0);
  delay(700);
  //servo1.write(90);
}

void printMessageImpact() {
  Serial.println("Touche !");

  /*
    ecranRGB.clear();
    ecranRGB.setRGB(255, 0, 0);
    ecranRGB.setCursor(0,0);
    ecranRGB.print("Touche !");
  */
}

bool didMoved(int _x) {
  if (_x >  0.8 || _x < 0) {
    return true;
  }
  return false;
}
