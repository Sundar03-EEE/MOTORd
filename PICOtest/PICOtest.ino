#include <HX711_ADC.h>
#include "hardware/gpio.h"

#define HX711_dout 2
#define HX711_sck 3
#define ENCODER_A 10
#define ENCODER_B 11
#define PULSES_PER_REV 400

HX711_ADC LoadCell(HX711_dout, HX711_sck);
volatile long encoderTicks = 0;

float peakTorque = 0, currentTorque = 0, calibrationValue = 139200;
bool tareInProgress = false;

unsigned long rpmWindow = 1000;
unsigned long lastRpmTime = 0;
long rpmTickBuffer = 0;

void readEncoder() {
  if (gpio_get(ENCODER_B)) {
    encoderTicks++;
  } else {
    encoderTicks--;
  }
}

void handleSerialInput() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input == "tare") {
    LoadCell.tareNoDelay();
    tareInProgress = true;
  } else if (input == "RESET") {
    peakTorque = 0;
    Serial.println(" torque reset");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), readEncoder, FALLING);

  LoadCell.begin();
  LoadCell.start(2000, true);
  LoadCell.setCalFactor(calibrationValue);
}

void loop() {
  handleSerialInput();

  if (tareInProgress && LoadCell.getTareStatus()) {
    Serial.println("Tare complete.");
    tareInProgress = false;
  }

  LoadCell.update();

  rpmTickBuffer += encoderTicks;
  encoderTicks = 0;

  if (millis() - lastRpmTime >= rpmWindow) {
    float rpm = (rpmTickBuffer * 60.0) / PULSES_PER_REV;
    rpmTickBuffer = 0;
    lastRpmTime = millis();

    float weight = abs(LoadCell.getData());
    currentTorque = weight * 7.0;
    if (currentTorque > peakTorque) peakTorque = currentTorque;

    Serial.print("RPM:"); Serial.println(rpm);
    Serial.print("Torque:"); Serial.println(peakTorque);
  }
}






