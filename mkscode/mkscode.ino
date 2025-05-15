#include <AccelStepper.h>

#define STEP_PINx     54
#define DIR_PINx      55
#define ENABLE_PINx   38
#define STEP_PINy     60
#define DIR_PINy      61
#define ENABLE_PINy   56

AccelStepper stepperX(AccelStepper::DRIVER, STEP_PINx, DIR_PINx);
AccelStepper stepperY(AccelStepper::DRIVER, STEP_PINy, DIR_PINy);

int uStep = 8;
int steps_per_rev = 200;
int acc = 500;

float currentRPM = 0;
bool yHoming = false;
bool base = false;

void disableDrivers() {
  digitalWrite(ENABLE_PINx, HIGH);
  digitalWrite(ENABLE_PINy, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(ENABLE_PINx, OUTPUT);
  pinMode(ENABLE_PINy, OUTPUT);
  disableDrivers();

  stepperX.setMaxSpeed(50000);
  stepperY.setMaxSpeed(20000);
  stepperY.setAcceleration(acc);
}

float rpm_to_step(float rpm){
  return (rpm * steps_per_rev * uStep) / 60.0;
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();

    if (input == "OX") {
      digitalWrite(ENABLE_PINx, LOW);
    } 
    else if (input == "OY") {
      digitalWrite(ENABLE_PINy, LOW);
    } 
    else if (input == "XX") {
      digitalWrite(ENABLE_PINx, HIGH);
    } 
    else if (input == "XY") {
      digitalWrite(ENABLE_PINy, HIGH);
    } 
    else if (input == "H") {
      stepperY.moveTo(5000); // new absolute "home"
      yHoming = true;
      Serial.println("[↩] Homing Y to 5000...");
    } 
    else if (input.startsWith("Y")) {
      long yTarget = input.substring(1).toInt();
      stepperY.moveTo(yTarget);
      Serial.println("Moved to yTarget"); // added this 
    } 
    else if (input.startsWith("B")){
      stepperY.moveTo(0);
      base = true;
      Serial.println("Y axis getting reset");
    }
    else {
      float rpm = input.toFloat();
      if (rpm > 0) {
        currentRPM = rpm;
        stepperX.setSpeed(rpm_to_step(rpm));
      }
    }
  }

  if (currentRPM > 0) {
    stepperX.runSpeed();
  }

  stepperY.run();

  if (yHoming && stepperY.distanceToGo() == 0) {
    stepperY.setCurrentPosition(5000); // updated home base
    yHoming = false;
    Serial.println("[✓] Y-axis Homed to 5000");
  }

  if (base && stepperY.distanceToGo() == 0){
    stepperY.setCurrentPosition(0);
    base = false;
    Serial.println("Y-axis Reset");
  }
}

