#include <NewPingESP8266.h> // Capteur ultrason
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>
#include <rgb_lcd.h>

//HP
int pinSpeaker = D4;

//LED
int ledPin = D3;
int sState = LOW;
int lState = LOW;

//SONAR
NewPingESP8266 sonar(D0, D5, 200);
int distance = 0;
long prev = 0;

//GIRO+ACC
TwoWire i2c;
MPU6050 mpu6050(i2c);
long timeStamp = 0;

//MECA
Servo servo1;

//LCD
rgb_lcd ecranRGB;

void setup() {
  Serial.begin(115200);

  //Init LED
  pinMode(ledPin, OUTPUT);
  //Init HP
  pinMode(pinSpeaker, OUTPUT);
  //Init GIRO+ACC
  i2c.begin(D2, D1);
  mpu6050.begin();
  //Init Servo
  servo1.attach(D6);
  servo1.write(90);
  //LCD
  ecranRGB.begin(16, 2, 0x00);

  Serial.println("Setup done.");
}

void loop() {
  distance = getDistance();
  unsigned long currentTime = millis();

  //Gestion de la distance à un obstacle
  if (currentTime - prev > distance * 10 && distance > 0 && distance <= 25) {
    prev = currentTime;
    if (sState == LOW && distance != 0) {
      sState = HIGH;
    } else {
      sState = LOW;
    }
    digitalWrite(ledPin, sState);
    playTone(distance, 20 / distance * 1500);
    printMessageDistance(distance);
  }

  //Gestion d'une colision
  if (millis() - timeStamp > 10) {
    mpu6050.update();
    if (didMoved(mpu6050.getAccZ()) == true) {
      impactAction();
    }
    timeStamp = millis();
  }
}

//Retourne la distance évaluée par le sonar
int getDistance() {
  delay(40);
  return sonar.ping() / US_ROUNDTRIP_CM;
}

//Emet un bip donc la fréquence est proportionnelle à la distance de l'obstacle
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
  servo1.write(0);
  delay(700);
  servo1.write(90);
}

void printMessageImpact() {
  Serial.println("Touche !");

    ecranRGB.clear();
    ecranRGB.setRGB(255, 0, 0);
    ecranRGB.setCursor(0,0);
    ecranRGB.print("Touche !");
}

bool didMoved(int _x) {
  if (_x >  0.8 || _x < 0) {
    return true;
  }
  return false;
}

void printMessageDistance(int distance) {
  ecranRGB.clear();
  if (distance != 0) {
    if (distance <= 5) {
      ecranRGB.setRGB(255, 0, 0);
    } else if (distance <= 10 ) {
      ecranRGB.setRGB(255, 165, 0);
    } else {
      ecranRGB.setRGB(165, 255, 0);
    }
  } else {
    ecranRGB.setRGB(165, 255, 0);
  }
  ecranRGB.setCursor(0, 0);
  ecranRGB.print("Vous etes a :");
  ecranRGB.setCursor(0, 1);
  ecranRGB.print(distance);
  ecranRGB.print(" cm");
}
