#include <NewPingESP8266.h> // Capteur ultrason
//#include <rgb_lcd.h>

//#include <MPU6050_tockn.h>
//#include <Wire.h>
//#include <Servo.h>

int pinSpeaker = D4;
int ledPin = 1;
NewPingESP8266 sonar(D2, D3, 200);
int distance = 0;
long prev = 0;
int sState = LOW;
int lState = LOW;
const int interval = 800;
int timeInterval = 200;
int previousTime = 0;
/*
//rgb_lcd ecranRGB;
TwoWire i2c;
MPU6050 mpu6050(i2c);*/
long timeStamp = 0;

//Servo servo1;
//int positionMoteur = 0;

int position = 0;
const int MPU = 0x68;
int16_t oX, oY, oZ;
int16_t _x, _y, _z;


void setup() {
  Serial.begin(57600);
  Serial.println("TOTO");
 //ecranRGB.begin(16, 2, 0x00);

  //pinMode(ledPin, OUTPUT);
  //pinMode(pinSpeaker, OUTPUT);
 // i2c.begin(D3, D4);
  //mpu6050.begin();

  //servo1.attach(8);
  /*  Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 12, true);
    oX = Wire.read() << 8 | Wire.read();
    oY = Wire.read() << 8 | Wire.read();
    oZ = Wire.read() << 8 | Wire.read();*/
}

void loop() {
  int distance = getDistance();
      Serial.println(distance);

  /*
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    printMessageDistance(4);
  */
/*
  unsigned long currentTime = millis();
  if (currentTime - prev > distance * 10 && distance > 0 && distance <= 25) {
    Serial.println("DEDANS");
    Serial.println(sState);

    prev = currentTime;
    if (sState == LOW && distance != 0)
      sState = HIGH;
    else sState = LOW;
    digitalWrite(ledPin, sState);
    playTone(distance, 20 / distance * 1500);
    //printMessageDistance(distance);
  }
  previousTime = currentTime;

  if (millis() - timeStamp > 10) {
    mpu6050.update();
    Serial.print("-2 2 ");
    Serial.println(mpu6050.getAccZ());
    timeStamp = millis();
  }
  /*
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 12, true);
      _x = Wire.read() << 8 | Wire.read();
      _y = Wire.read() << 8 | Wire.read();
      _z = Wire.read() << 8 | Wire.read();
 
  if (didMoved(_x, _y, _z) == true) {
    Serial.println("Moved");
    // impactAction();
  } */
}
/*
  void impactAction() {
  printMessageImpact();
  servo1.write(0);
  delay(700);
  servo1.write(90);
  }*/
/*
  void printMessageImpact() {
  ecranRGB.clear();
  ecranRGB.setRGB(255, 0, 0);
  ecranRGB.setCursor(0, 0);
  ecranRGB.print("Touche !");
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
*/

int getDistance() {
  delay(40);
  return sonar.ping() / US_ROUNDTRIP_CM;
}
/*
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


bool didMoved(int _x, int _y, int _z) {
  if ((oY - _y  > interval || oY - _y  < -interval)) {
    return true;
  }
  return false;
  */
//}
