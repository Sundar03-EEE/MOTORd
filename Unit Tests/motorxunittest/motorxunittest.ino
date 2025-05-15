#include <AccelStepper.h>
#define STEP_PIN_X 54
#define DIR_PIN_X 55
#define ENABLE_PIN_X 38
#define ENCODER_A 10
#define ENCODER_B 11
#define PULSES_PER_REV 400

AccelStepper stepperX(AccelStepper::DRIVER, STEP_PIN_X, DIR_PIN_X);
volatile long encoderTicks = 0;
unsigned long lastPrint = 0;
int uStep = 8;
int steps_per_rev = 200;
float RPM = 0; 

void readEncoder() {
  encoderTicks++;
}

void setup() {
  Serial.begin(115200);
  pinMode(ENABLE_PIN_X, OUTPUT);
  digitalWrite(ENABLE_PIN_X, HIGH);

  stepperX.setMaxSpeed(50000);

 // pinMode(ENCODER_A, INPUT_PULLUP);
 // pinMode(ENCODER_B, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_A), readEncoder, FALLING);
}
float rpm_to_step(float rpm) {
  return (rpm * steps_per_rev * uStep) / 60;
}

void loop() {
  stepperX.setSpeed(-rpm_to_step(450));
  stepperX.runSpeed();

  //if (millis() - lastPrint > 1000) {
    //long ticks = encoderTicks;
    //encoderTicks = 0;
    //float rpm = (ticks / (float)PULSES_PER_REV) * 60.0;

   // Serial.print("Motor X RPM: ");
    //Serial.println(rpm);
    //lastPrint = millis();
  //}
}
