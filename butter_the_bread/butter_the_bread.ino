
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>

#define NUM_INSTRUCTIONS 3
#define MAX_MS_NO_COMMS 250

#define LED_BUILTIN 13
#define ARM_UP_POS 180

// Motors
MeDCMotor left_motor(M1);
MeDCMotor right_motor(M2);

// Servo
MePort servo_port(PORT_1);
Servo arm_servo;

void setup() {
  // Setup.
  Serial.begin(115200);    // The factory default baud rate is 115200
  arm_servo.attach(servo_port.pin1());
  arm_servo.write(ARM_UP_POS);

  // Give a few blinks to show that the code is up and running.
  blink(3);
}

unsigned long last_time_rx = 0;

void loop() {
  tryReadComms();
  checkComms();
}

void tryReadComms() {
  // Wait until enough instructions have arrived.
  if (Serial.available() < NUM_INSTRUCTIONS + 1) {
    return;
  }
  
  // Look for the start byte (255, or 0xFF)
  int start = Serial.read();
  if (start != 255) {
    return;
  }

  // Indicate that we have signal by illuminating the on-board LED
  digitalWrite(LED_BUILTIN, HIGH);
  last_time_rx = millis();

  // Read the instructions.
  int left_motor_speed = Serial.read();
  int right_motor_speed = Serial.read();
  int arm_pos = Serial.read();

  setMotorSpeed(left_motor_speed, &left_motor);
  setMotorSpeed(right_motor_speed, &right_motor);
  arm_servo.write(arm_pos);
}


void checkComms() {
  if (millis() - last_time_rx > MAX_MS_NO_COMMS) {
    // Set all motors to neutral
    left_motor.run(0);
    right_motor.run(0);
    arm_servo.write(ARM_UP_POS);
    // Indicate that we have lost comms by turning off the on-board LED
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("No comms");
  }
}

bool setMotorSpeed(int motor_speed, MeDCMotor* motor) {
  // Motor speed is betwen 0-254, map it back to -255, 255.
  if (motor_speed >= 0 && motor_speed <= 254) {
    motor_speed *= 2;
    motor_speed -= 254;
    motor->run(motor_speed);
  }
}

void blink(int num_blinks) {
  for (int i = 0; i < num_blinks; ++i) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

