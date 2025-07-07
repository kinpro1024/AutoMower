#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>

// Pins for Driving the Motors
const uint8_t MOTOR_ENA   = 11;
const uint8_t RIGHT_IN1   = A2;
const uint8_t RIGHT_IN2   = A3;
const uint8_t LEFT_IN1    = 4;
const uint8_t LEFT_IN2    = 5;

// Pins of HC-SR04 Ultrasonic DIstance Sensor
const uint8_t TRIG_PIN = 6;
const uint8_t ECHO_PIN = 7;

// ESC Servo Pin
const uint8_t ESC_PIN = 9;

enum State {
  FORWARD, STOP1,
  BACKWARD, STOP2,
  TURN_RIGHT, STOP3,
  TURN_LEFT, STOP4,
  OBSTACLE_STOP
};

State state = FORWARD;
unsigned long stateStart = 0;

Adafruit_LiquidCrystal lcd(0x20);
Servo esc;  // ESC Signal Pin

// Functions
void enterState(State newState);//Scope Declaration
long readDistance();

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);
  digitalWrite(MOTOR_ENA, HIGH);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  esc.attach(ESC_PIN);
  esc.writeMicroseconds(2000);  // Neutral throttle on ESC

  enterState(FORWARD);
}

void loop() {
  unsigned long now = millis();

  if (state != OBSTACLE_STOP &&
      state != STOP1 && state != STOP2 &&
      state != STOP3 && state != STOP4) {

    long distance = readDistance();
    if (distance > 0 && distance < 35) {
      enterState(OBSTACLE_STOP);
      return;
    }
  }

  switch (state) {
    case FORWARD:
    case BACKWARD:
    case TURN_LEFT:
    case TURN_RIGHT:
      if (now - stateStart >= 2000) {
        if (state == FORWARD)       enterState(STOP1);
        else if (state == BACKWARD) enterState(STOP2);
        else if (state == TURN_RIGHT) enterState(STOP3);
        else if (state == TURN_LEFT)  enterState(STOP4);
      }
      break;

    case STOP1:
    case STOP2:
    case STOP3:
    case STOP4:
      if (now - stateStart >= 500) {
        if (state == STOP1)       enterState(BACKWARD);
        else if (state == STOP2)  enterState(TURN_RIGHT);
        else if (state == STOP3)  enterState(TURN_LEFT);
        else if (state == STOP4)  enterState(FORWARD);
      }
      break;

    case OBSTACLE_STOP:
      if (readDistance() >= 30) {
        enterState(FORWARD);
      }
      break;
  }
}

void enterState(State newState) {
  state = newState;
  stateStart = millis();

  lcd.clear();
  Serial.println();

  switch (newState) {
    case FORWARD:
      Serial.println("FORWARD");
      lcd.print("FORWARD");
      digitalWrite(LEFT_IN1,  HIGH); digitalWrite(LEFT_IN2,  LOW);
      digitalWrite(RIGHT_IN1, HIGH); digitalWrite(RIGHT_IN2, LOW);
      esc.writeMicroseconds(2000);  // Simulate Full Throttle Spinning
      break;

    case BACKWARD:
      Serial.println("BACKWARD");
      lcd.print("BACKWARD");
      digitalWrite(LEFT_IN1,  LOW); digitalWrite(LEFT_IN2,  HIGH);
      digitalWrite(RIGHT_IN1, LOW); digitalWrite(RIGHT_IN2, HIGH);
      esc.writeMicroseconds(1500);  // Neutral Throttle to avoid misalignment of the caster wheel
      break;

    case TURN_LEFT:
      Serial.println("TURN LEFT");
      lcd.print("LEFT TURN");
      digitalWrite(LEFT_IN1,  LOW);  digitalWrite(LEFT_IN2, HIGH);
      digitalWrite(RIGHT_IN1, HIGH); digitalWrite(RIGHT_IN2, LOW);
      esc.writeMicroseconds(1700);  // Partial Throttle during turn
      break;

    case TURN_RIGHT:
      Serial.println("TURN RIGHT");
      lcd.print("RIGHT TURN");
      digitalWrite(LEFT_IN1,  HIGH); digitalWrite(LEFT_IN2, LOW);
      digitalWrite(RIGHT_IN1, LOW);  digitalWrite(RIGHT_IN2, HIGH);
      esc.writeMicroseconds(1700);  // Partial Throttle during turn
      break;

    case STOP1:
    case STOP2:
    case STOP3:
    case STOP4:
      Serial.println("STOP");
      lcd.print("STOP");
      digitalWrite(LEFT_IN1, LOW); digitalWrite(LEFT_IN2, LOW);
      digitalWrite(RIGHT_IN1, LOW); digitalWrite(RIGHT_IN2, LOW);
      esc.writeMicroseconds(1500);  // Neutral Throttle on ESC
      break;

    case OBSTACLE_STOP:
      Serial.println("Obstacle Detected!");
      lcd.print("Obstacle!");
      digitalWrite(LEFT_IN1, LOW); digitalWrite(LEFT_IN2, LOW);
      digitalWrite(RIGHT_IN1, LOW); digitalWrite(RIGHT_IN2, LOW);
      esc.writeMicroseconds(1500);  // Neutral Throttle on ESC
      break;
  }
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms Timeout
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}