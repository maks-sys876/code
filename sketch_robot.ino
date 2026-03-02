#include <PRIZM.h>
#include "HUSKYLENS.h"
#include <Wire.h>
int redpin = 2;
int greenpin = 9;
PRIZM prizm;
HUSKYLENS huskylens;
long start = 0;

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
  digitalWrite(greenpin, 1);
  digitalWrite(redpin, 1);
  Serial.println(F("HuskyLens готовий!"));
}

void loop() {
huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);
  delay(250);
  if (huskylens.request() && huskylens.available() > 0) {
    for (int i = 0; i < huskylens.available(); i++) {
      HUSKYLENSResult result = huskylens.get(i);
      
      if (result.ID == 1) {
        Serial.println(F("Бачу Обличчя. Відстежую..."));
        start = millis();
        if (result.xCenter < 130) {
          prizm.setMotorPower(1, -10); 
          prizm.setMotorPower(2, 10);
        }
        else if (result.xCenter > 190) {
          prizm.setMotorPower(1, 10);
          prizm.setMotorPower(2, -10);
        }
        else {
          int distance = prizm.readSonicSensorCM(4);
          if (distance > 60) {
            prizm.setMotorPower(1, 30);
            prizm.setMotorPower(2, 30);
          } else {
            prizm.setMotorPower(1, 0);
            prizm.setMotorPower(2, 0);
          }
        }
      }
    }
  }
  huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION);
  delay(250); // Час на зміну режиму

  if (huskylens.request() && huskylens.available() > 0) {
    for (int i = 0; i < huskylens.available(); i++) {
      HUSKYLENSResult result = huskylens.get(i);
      
      if (result.ID == 1) {
        Serial.println(F("Знайдена Мітка 1. Хороший товар"));
        digitalWrite(greenpin, LOW);
        digitalWrite(redpin, HIGH);
        start = millis();
      }
      else if (result.ID == 2) {
        Serial.println(F("Знайдена Мітка 2. Поганий товар"));
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