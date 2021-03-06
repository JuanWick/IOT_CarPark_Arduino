#include <NewPingESP8266.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>
#include <rgb_lcd.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>

// Blynk constants
const char AUTH[] = "9cb213f5a57e4c389d7cd5318ee4f643";
BlynkTimer timer;
BlynkTimer timer_distance;


// WiFi constants
const char* WIFI_SSID = "Mfbiya";
const char* WIFI_PASSWORD = "gptopidq";
const int WIFI_UNKNOWN_RETRY_DELAY = 10;
const int WIFI_KNOWN_RETRY_DELAY = 2000;
const char* PING_HOST = "www.google.com";
const char* THINGSBOARD_SERVER = "http://demo.thingsboard.io/api/v1/GUmgsqDiPPDMpzsMklM7/telemetry";
bool is_network_op = false;
int current_ssid = 0;

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

// GYRO+ACC constants
TwoWire i2c;
MPU6050 mpu6050(i2c);
long timeStamp = 0;

// MECA constants
Servo servo1;

// LCD constants
rgb_lcd ecranRGB;

void setup() {
  Serial.begin(115200);

  setup_lcd();
  show_message("Setup", ".....", 255, 175, 31, 2000);
  show_message("Setup", "WIFI", 255, 175, 31, 2000);
  setup_wifi();
  if (is_network_op) {
    show_message("Setup", "WIFI FOUND", 255, 175, 31, 2000);
  } else {
    show_message("Setup", "WIFI NOT FOUND", 255, 175, 31, 2000);
  }
  setup_timer();
  show_message("Setup", "LED", 255, 175, 31, 2000);
  setup_led();
  show_message("Setup", "SOUND", 255, 175, 31, 2000);
  setup_hp();
  show_message("Setup", "ACCELERO", 255, 175, 31, 2000);
  setup_giro();
  show_message("Setup", "SERVO", 255, 175, 31, 2000);
  setup_servo();
  show_message("Setup done", ":-)", 255, 175, 31, 2000);

  Serial.println("Setup done.");
}

/**
   Setup BLINK.
*/
void setup_blink() {
  if (is_network_op) {
    if (WiFi.SSID(current_ssid) == WIFI_SSID) {
      Blynk.begin(AUTH, WIFI_SSID, WIFI_PASSWORD);
    } else {
      Blynk.begin(AUTH, WIFI_SSID, "");
    }
    Blynk.run();
  }
}

/**
   Setup TIMER
*/
void setup_timer() {
  timer.setInterval(5000L, is_alive_kpi); //timer will run every 5 sec
  timer_distance.setInterval(250L, get_distance);
}


/**
   Setup WIFI mode.
*/
void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  connect_to_available_networks();
}

/**
   Setup LED mode.
*/
void setup_led() {
  pinMode(PIN_LED, OUTPUT);
  randomSeed(42);

  for (int i = 0; i < 50; i++) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    delay(random(50, 200));
  }
  digitalWrite(PIN_LED, LOW);
}

/**
   Setup HP mode.
*/
void setup_hp() {
  pinMode(PIN_SPEAKER, OUTPUT);

  for (int i = 0; i < 15; i++) {
    digitalWrite(PIN_SPEAKER, !digitalRead(PIN_SPEAKER));
    delay(random(50, 200));
  }
  digitalWrite(PIN_SPEAKER, LOW);
}

/**
   Setup GYRO+ACC mode.
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
  delay(500);
  servo1.write(0);
  delay(500);
  servo1.write(90);
}

/**
   Setup LCD mode.
*/
void setup_lcd() {
  ecranRGB.begin(16, 2, 0x00);
  for (int i = 0; i < 100; i++) {
    ecranRGB.clear();
    ecranRGB.setRGB(random(0, 255), random(0, 255), random(0, 255));
    delay(20);
  }
}

void show_message(char* message_haut, char* message_bas, int red, int green, int blue, int timer) {
  ecranRGB.clear();
  ecranRGB.setRGB(red, green, blue);
  ecranRGB.setCursor(0, 0);
  ecranRGB.print(message_haut);
  ecranRGB.setCursor(0, 1);
  ecranRGB.print(message_bas);
  if (timer > 0) {
    delay(timer);
  }
}

/**
   Evaluate sonar distance detection
*/
void get_distance() {
  distance = sonar.ping() / US_ROUNDTRIP_CM;
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
  show_message("Touche !", "", 255, 0, 0, 0);
  servo1.write(0);
  delay(700);
  servo1.write(90);

  internet_connection_handler();
  send_colide_state();
}

/**
   Print distance feedback to the LCD screen
*/
void print_message_distance(int distance) {
  int red = 0;
  int green = 165;
  int blue = 255;

  if (distance != 0 && distance <= 5) {
    red = 255;
    green = 0;
    blue = 0;
  } else if (distance != 0 && distance <= 10 ) {
    red = 255;
    green = 165;
    blue = 0;
  }

  String str = String(distance);
  char char_distance[16];
  str.toCharArray(char_distance,16);
  show_message("Vous etes a (cm) :", char_distance, red, blue, green, 0);
}

bool did_moved(int _x) {
  return _x >  0.5 || _x < 0;
}

/**
   Handler Sonar feedback to LCD and LED
*/
void distance_detection_handler() {
  unsigned long currentTime = millis();

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
}

/**
   Handle ACCELERO detection and feedback
*/
void colision_detection_handler() {
  if (millis() - timeStamp > 10) {
    mpu6050.update();
    if (did_moved(mpu6050.getAccZ()) == true) {
      impact_event();
    }
    timeStamp = millis();
  }
}

/**
   Check and handle auto-internet connection
*/
void internet_connection_handler() {
  make_ping_request(current_ssid);
  if (!is_network_op) {
    while (WiFi.status() != WL_CONNECTED && !is_network_op) {
      connect_to_available_networks();
    }
  }
  Serial.print("Current Network : ");
  Serial.println(WiFi.SSID(current_ssid));
  Serial.println("-----------------------------");
}

/**
   Test current internet connection
*/
void make_ping_request(int i) {
  Serial.print("Pinging host ");
  Serial.println(PING_HOST);

  if (Ping.ping(PING_HOST)) {
    is_network_op = true;
    current_ssid = i;

    Serial.println("Success!!");
  } else {
    is_network_op = false;
    Serial.println("Error :(");
  }
  Serial.println("-----------------------------");
  Serial.println("");
}

void is_alive_kpi()
{
  Blynk.virtualWrite(V1, "OK");  // sending sensor value to Blynk app
  send_connection_state();
}

/**
   Scan and try to connect to any open network or known wifi network
*/
void connect_to_available_networks() {

  Serial.println("-- Try Connecting --");
  Serial.println();

  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found.");

    // Check open networks
    for (int i = 0; i < n; ++i) {
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE && !is_network_op) {
        Serial.println("-----------------------------");
        Serial.print("Open Network discover : ");
        Serial.println(WiFi.SSID(i));

        WiFi.begin(WiFi.SSID(i));

        int nbAttempts = 0;
        while (WiFi.status() != WL_CONNECTED && nbAttempts < WIFI_UNKNOWN_RETRY_DELAY) {
          nbAttempts++;
          delay(500);
          Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("");
          Serial.print("WiFi connected with ip ");
          Serial.println(WiFi.localIP());
          delay(200);
          make_ping_request(i);
          setup_blink();
        } else {
          Serial.println();
          Serial.println("WiFi connection time out");
          Serial.println("-----------------------------");
          Serial.println("");
        }

      } else {
        if ((WiFi.SSID(i) == WIFI_SSID) && !is_network_op) {
          Serial.println("-----------------------------");
          Serial.print("Known Network discover : ");
          Serial.println(WiFi.SSID(i));
          WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
          int nbAttempts = 0;

          while (WiFi.status() != WL_CONNECTED  && nbAttempts < WIFI_KNOWN_RETRY_DELAY) {
            nbAttempts++;
            delay(100);
            Serial.print(".");
          }
          Serial.println("");
          Serial.print("WiFi connected with ip ");
          Serial.println(WiFi.localIP());
          delay(200);
          make_ping_request(i);
          setup_blink();
        }
      }
      delay(10);
    }
  }

}

/**
   Send current state information
*/
void send_state_using_post(String payload) {
  HTTPClient http;
  http.begin(THINGSBOARD_SERVER);
  http.POST(payload);
  http.end();

  Serial.println();
  Serial.println("-----------------------------");
  Serial.println("request sent");
  Serial.print(" STATE POST : ");
  Serial.println(payload);
  Serial.println("-----------------------------");
  Serial.println();
}

void send_state_using_blynk(String payload) {
  Blynk.notify(payload);

  Serial.println();
  Serial.println("-----------------------------");
  Serial.println("request sent");
  Serial.print(" STATE BLYNK : ");
  Serial.println(payload);
  Serial.println("-----------------------------");
  Serial.println();
}

/**
   Inform back-end of connection state
*/
void send_connection_state() {
  if ((WiFi.status() == WL_CONNECTED) && is_network_op) {
    String payload = "{";
    payload += "\"isConnected\":";
    payload += is_network_op;
    payload += "}";
    send_state_using_post(payload);
  }
}

/**
   Inform back-end of colide event
*/
void send_colide_state() {
  if ((WiFi.status() == WL_CONNECTED) && is_network_op) {
    String payload = "{";
    payload += "\"collideEvent\":";
    payload += true;
    payload += "}";
    send_state_using_post(payload);
    send_state_using_blynk("Your car is in danger !");
  }
}

/**
   ESP event loop.
*/
void loop() {

  timer.run();
  timer_distance.run();
  distance_detection_handler();
  colision_detection_handler();
}
