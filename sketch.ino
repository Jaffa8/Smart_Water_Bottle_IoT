// Enter your own credentials here as I have uploaded directly from Wokwi and not any IDE so did not have the option to add .gitignore
#define BLYNK_TEMPLATE_ID "Enter your own id"
#define BLYNK_TEMPLATE_NAME "Enter your own template name"
#define BLYNK_AUTH_TOKEN "enter your own auth token"

#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

MPU6050 mpu(0x68);
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

BlynkTimer timer;
unsigned long lastSipTime = 0;
int lastWaterLevel = 100;

const unsigned long SIP_REMINDER_TIME = 10000; 

void checkBottle() {
  
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; 
  
 
  int waterLevel = map(distance, 5, 350, 100, 0);
  if(waterLevel > 100) waterLevel = 100;
  if(waterLevel < 0) waterLevel = 0;

 
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float tiltAngle = atan2(ax / 16384.0, az / 16384.0) * 180.0 / PI;

  String status = "";


  
  if (abs(tiltAngle) > 45.0) { 
  
    status = "Drinking...";
    lastSipTime = millis(); 
    lastWaterLevel = waterLevel;
  } 
  else if (waterLevel <= 0) {
   
    status = "Fill the water bottle!";
    lastWaterLevel = waterLevel;
  }
  else if (waterLevel >= 100) {
  
    status = "Water bottle filled!";
    lastWaterLevel = waterLevel;
    lastSipTime = millis(); 
  }
  else if (waterLevel < lastWaterLevel) {
   
    status = "You just had a sip!";
    lastSipTime = millis();
    lastWaterLevel = waterLevel;
  }
  else if (millis() - lastSipTime < 10000 && lastSipTime != 0) {
   
    status = "You just had a sip!";
  }
  else if (millis() - lastSipTime > SIP_REMINDER_TIME) {
  
    status = "Time to hydrate! Take a sip";
  }
  else {
  
    status = "Hydrated";
  }

 
  Blynk.virtualWrite(V0, waterLevel); 
  Blynk.virtualWrite(V1, status);     

  Serial.printf("Lvl: %d%% | Status: %s\n", waterLevel, status.c_str());
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  mpu.initialize();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(1000L, checkBottle);
  lastSipTime = millis();
}

void loop() {
  Blynk.run();
  timer.run();
}
