#include <Encoder.h>

//This is a TinkerCAD issue, if at any point the Encoder Motors are in "coast"(00) or "brake"(11). 
//The model breaks and does not restart until the simulation environment is completely reset.
//This in turn forced me to re-do the whole thing from scratch with non-encoded motors.

// Pins for Driving Motors
const uint8_t MOTOR_ENA   = 11;
const uint8_t RIGHT_IN1   = A5;
const uint8_t RIGHT_IN2   = A4;
const uint8_t LEFT_IN1    = 4;
const uint8_t LEFT_IN2    = 5;

// Assigning Pins to Encoders
const uint8_t ENC_LEFT_A  = 3;
const uint8_t ENC_LEFT_B  = 12;
const uint8_t ENC_RIGHT_A = 2;
const uint8_t ENC_RIGHT_B = A3;

Encoder leftEnc(ENC_LEFT_A, ENC_LEFT_B);
Encoder rightEnc(ENC_RIGHT_A, ENC_RIGHT_B);

const long TARGET_COUNT = 20000;

enum Direction { FORWARD, BACKWARD, STOPPED };
Direction direction = FORWARD;

void setup() {
  Serial.begin(115200);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_IN1,  OUTPUT);
  pinMode(LEFT_IN2,  OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  pinMode(ENC_LEFT_A,  INPUT_PULLUP);
  pinMode(ENC_LEFT_B,  INPUT_PULLUP);
  pinMode(ENC_RIGHT_A, INPUT_PULLUP);
  pinMode(ENC_RIGHT_B, INPUT_PULLUP);

  digitalWrite(MOTOR_ENA, HIGH); // ALways Pulled High because of Simulation Issues
  leftEnc.write(0);
  rightEnc.write(0);

  moveForward();
}

void loop() {
  if (direction == STOPPED) return;

  long leftPos  = leftEnc.read();
  long rightPos = rightEnc.read();

  // Serial Prints Every 100 ms
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 10) {
    Serial.print("L="); Serial.print(leftPos);
    Serial.print("  R="); Serial.println(rightPos);
    lastPrint = millis();
  }

  if (direction == FORWARD && leftPos <= -TARGET_COUNT && rightPos <= -TARGET_COUNT) {
    moveBackward();
  }

  else if (direction == BACKWARD && leftPos >= TARGET_COUNT && rightPos >= TARGET_COUNT) {
    stopMotors();
  }
}

void moveForward() { //(1,0)
  Serial.println("FORWARD");
  direction = FORWARD;

  digitalWrite(LEFT_IN1,  HIGH);
  digitalWrite(LEFT_IN2,  LOW);
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
}

void moveBackward() { //(0,1)
  Serial.println("BACKWARD");
  direction = BACKWARD;

  digitalWrite(LEFT_IN1,  LOW);
  digitalWrite(LEFT_IN2,  HIGH);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
}

void stopMotors() { //Coast(0,0)
  Serial.println("STOPPED");
  direction = STOPPED;

  digitalWrite(LEFT_IN1,  LOW);
  digitalWrite(LEFT_IN2,  LOW);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
} 