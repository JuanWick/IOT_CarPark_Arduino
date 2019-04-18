#include <NewPingESP8266.h> // Capteur ultrason
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>
#include <rgb_lcd.h>

// WiFi constants
const char *WIFI_SSID = "AndroidAP0DCF";
const char *WIFI_PASSWORD = "hyny0433";
const int WIFI_RETRY_DELAY = 1000;

// HP constants
const int PIN_SPEAKER = D4;

// LED constants
const int PIN_LED = D3;
int sState = LOW;
int lState = LOW;

// SONAR constants
NewPingESP8266 sonar(D0, D5, 200);
int distance = 0;
long prev = 0;

// GIRO+ACC constants
TwoWire i2c;
MPU6050 mpu6050(i2c);
long timeStamp = 0;

// MECA constants
Servo servo1;

// LCD constants
rgb_lcd ecranRGB;

void setup() {
  Serial.begin(115200);

  setup_led();
  setup_hp();
  setup_giro();
  setup_servo();
  setup_lcd();

  Serial.println("Setup done.");
}

/**
   Setup LED mode.
*/
void setup_led() {
  pinMode(PIN_LED, OUTPUT);
}

/**
   Setup HP mode.
*/
void setup_hp() {
  pinMode(PIN_SPEAKER, OUTPUT);
}

/**
   Setup GIRO+ACC mode.
*/
void setup_giro() {
  i2c.begin(D2, D1);
  mpu6050.begin();
}

/**
   Setup SERVO mode.
*/
void setup_servo() {
  servo1.attach(D6);
  servo1.write(90);
}

/**
   Setup LCD mode.
*/
void setup_lcd() {
  ecranRGB.begin(16, 2, 0x00);
}

/**
   Evaluate sonar distance detection
*/
int get_distance() {
  delay(40);
  return sonar.ping() / US_ROUNDTRIP_CM;
}

/**
   Play sound with a frequency proportional to the close distance
*/
void play_tone(int duration, int tone_) {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
    while (elapsed_time < duration) {
      digitalWrite(PIN_SPEAKER, HIGH);
      delayMicroseconds(tone_ / 2);
      digitalWrite(PIN_SPEAKER, LOW);
      delayMicroseconds(tone_ / 2);
      elapsed_time += (tone_);
    }
  }
  else {
    delayMicroseconds(duration);
  }
}

/**
   Actions done when impact is detected
*/
void impact_event() {
  print_message_impact_event();
  servo1.write(0);
  delay(700);
  servo1.write(90);
}

/**
   Print impact feedback to the LCD screen
*/
void print_message_impact_event() {
  ecranRGB.clear();
  ecranRGB.setRGB(255, 0, 0);
  ecranRGB.setCursor(0, 0);
  ecranRGB.print("Touche !");
}

/**
   Print distance feedback to the LCD screen
*/
void print_message_distance(int distance) {
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

bool did_moved(int _x) {
  if (_x >  0.8 || _x < 0) {
    return true;
  }
  return false;
}

/**
   ESP event loop.
*/
void loop() {
  distance = get_distance();
  unsigned long currentTime = millis();

  //Gestion de la distance à un obstacle
  if (currentTime - prev > distance * 10 && distance > 0 && distance <= 25) {
    prev = currentTime;
    if (sState == LOW && distance != 0) {
      sState = HIGH;
    } else {
      sState = LOW;
    }
    digitalWrite(PIN_LED, sState);
    play_tone(distance, 20 / distance * 1500);
    print_message_distance(distance);
  }

  //Gestion d'une colision
  if (millis() - timeStamp > 2) {
    mpu6050.update();
    if (did_moved(mpu6050.getAccZ()) == true) {
      impact_event();
    }
    timeStamp = millis();
  }
}
