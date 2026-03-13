#include <PRIZM.h>
#include "HUSKYLENS.h"
#include <Wire.h>

int redpin = 2;
int greenpin = 9;
PRIZM prizm;
HUSKYLENS huskylens;

long start = 0;
int currentMode = 0;
unsigned long lastSwitchTime = 0;
const long switchInterval = 2000;

void setup() {
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  prizm.PrizmBegin();
  prizm.setMotorInvert(1, 1);

  while (!huskylens.begin(Wire)) {
    Serial.println(F("Помилка ініціалізації HuskyLens!"));
    delay(1000);
  }
  
  digitalWrite(greenpin, HIGH);
  digitalWrite(redpin, HIGH);
  huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);
  Serial.println(F("HuskyLens готовий!"));
}

void loop() {
  if (millis() - lastSwitchTime > switchInterval) {
    currentMode = (currentMode == 0) ? 1 : 0;
    lastSwitchTime = millis();

    if (currentMode == 0) {
      huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);
    } else {
      huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION);
    }
    delay(50);
  }

  if (huskylens.request() && huskylens.available() > 0) {
    HUSKYLENSResult result = huskylens.get(0);
Serial.println(result.xCenter);
    if (currentMode == 0 && result.ID == 1) {
      start = millis();
      
      if (result.xCenter < 130) {
        prizm.setMotorPower(1, 10); 
        prizm.setMotorPower(2, 20);
      }
      else if (result.xCenter > 230) {
        prizm.setMotorPower(1, 20);
        prizm.setMotorPower(2, 10);
      }
      else {
        int distance = prizm.readSonicSensorCM(4);
        if (distance > 30) {
          prizm.setMotorPower(1, 35);
          prizm.setMotorPower(2, 35);
        } else {
          prizm.setMotorPower(1, 0);
          prizm.setMotorPower(2, 0);
        }
      }
    }
    else if (currentMode == 1) {
      if (result.ID == 1) {
        digitalWrite(greenpin, LOW);
        digitalWrite(redpin, HIGH);
        start = millis();
      }
      else if (result.ID == 2) {
        digitalWrite(greenpin, HIGH);
        digitalWrite(redpin, LOW);
        start = millis();
      }
    }
  }

  if (millis() - start > 3000) {
    digitalWrite(greenpin, HIGH);
    digitalWrite(redpin, HIGH);
    prizm.setMotorPower(1, 0);
    prizm.setMotorPower(2, 0);
  }
}
