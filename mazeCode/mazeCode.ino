// VARIABLES

// How long to wait after each instruction
int delayLength = 500;

// How long to make a 90 degree turn
int turnNinetyL = 350;
int turnNinetyR = 350;

// How long to go forward 10 inches
int forwardTenIn = 450;

// How long to hit into wall to realign
int forwardToClear = 250;

// How far to back up after realigning
int backFromRealign = 150;

// DEFINITIONS

// Button
#define button 2
boolean buttonState;
boolean buttonPressed;
boolean codeHasRun = false;

// RGB LED
#include "FastLED.h"
#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

// Needed for LED but do not understand it fully
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

// Front Ultrasonic
const int pingPin = 11;
long durationF;
float distanceF;

// Right Ultrasonic
const int trigPinR = 13;
const int echoPinR = 12;
long durationR;
float distanceR;

// MPU6050
#include "Wire.h"
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
float angZ = 0;

// DC motors
#define PWMA 5
#define PWMB 6
#define BIN1 7
#define AIN1 8
#define STBY 3

// Robot Speed
int speedMed = 125; 
int speedFast = speedMed + 50; 
int speedSlow = speedMed - 50;
int motorACorrection = 4;

// PD Controller
#define goalR 2 // 2 inches away from right wall
const float Gp = 30.0; 
const float Gd = 60.0;
int leftPWM = speedMed;
int rightPWM = speedMed;
float errorP; // Proportional error
float errorD; // Derivative error
float initialPoint; 
float finalPoint;

// Important setup function
void setup() {
  // Set up the LED
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(10); // Don't increase because it gets too bright

  // Warning during setup
  FastLED.showColor(Color(255, 1, 1)); // Red

  // Start communication with the serial monitor
  Serial.begin(9600);

  // Enable the right ultrasonic sensor
  pinMode(trigPinR, OUTPUT); 
  pinMode(echoPinR, INPUT);

  // Set up the button pin
  pinMode(button, INPUT_PULLUP); 

  // Set up the DC motors
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Set the initial motor speed
  motorSpeed(speedSlow);

  // Set up the MPU
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // Stop everything if could not connect to it
  Serial.println("Calculating offsets, do not move MPU6050");
  delay(1000);
  mpu.calcOffsets(); // Gyro and accelerometer
  Serial.println("Ready");

  // Setup complete
  FastLED.showColor(Color(1, 255, 1)); // Green
}

// Important loop function
void loop() {
  // Determine whether the button is pressed
  buttonState = digitalRead(button);

  if (buttonState == 0) {
    // Button has been pressed
    FastLED.showColor(Color(1, 1, 255)); // Blue

    // Can execute main logic if it is not already run
    if (!codeHasRun) {
      // WEEEEEE
      mazelogic2025();

      codeHasRun = true;
      Serial.println("Code Has Run");
      FastLED.showColor(Color(255, 255, 255)); // White
    }
  } else if (codeHasRun) {
    // Code has run once before
    FastLED.showColor(Color(255, 255, 255)); // White
  }
}

// Functions for the logic of the different mazes 

// Full 100'x100' maze
void fullMazeLogic() {
  // =========
  // EDIT THIS
  // =========
}

// The maze layout from 2025
void mazelogic2025() {
  tenInForward();
  tenInForward();
  tenInForward();
  // 4
  turnLeft();
  tenInForward();
  tenInForward();
  // 6
  turnLeft();
  tenInForward();
  // 7
  turnRight();
  tenInForward();
  // 8
  turnLeft();
  tenInForward();
  // 9
  turnRight();
  tenInForward();
  // 10
  turnRight();
  tenInForward();
  tenInForward();
  // 12
  ninetyRight();
  tenInForward();
  // 13
  turnLeft();
  tenInForward();
  // 14
  turnRight();
  tenInForward();
  tenInForward();
  tenInForward();
  // 17
  turnLeft();
  tenInForward();
  tenInForward();
  // 19
  turnLeft();
  tenInForward();
  tenInForward();
  tenInForward();
  // 22
  ninetyRight();
  tenInForward();
  tenInForward();
  // 24
  ninetyLeft();
  tenInForward();
  tenInForward();
  // 26
  turnLeft();
  tenInForward();
  tenInForward();
  // 28
  turnRight();
  tenInForward();
  // 29
  turnLeft();
  tenInForward();
  // 30
  turnRight();
  tenInForward();
  // 31
  turnLeft();
  tenInForward();
  // 32
  turnRight();
  tenInForward();
  // 33
  turnRight();
  tenInForward();
  hugWallUntil(12);
  tenInForward();
  // 38
  turnLeft();
  tenInForward();
  // 39
  turnLeft();
  hugWallUntil(2);
}

// Functions to continuously update the MPU when the robot is moving

// Actual logic
void dddelay(int dTime) {
  long currTime = millis();
  while (millis() - currTime < dTime) {
    mpu.update();
    angZ = mpu.getAngleZ();
    delay(5);
  }
}

// Also update LED
void ddelay(int dTime) {
  FastLED.showColor(Color(255, 1, 255)); // Magenta
  dddelay(dTime);
  FastLED.showColor(Color(0, 0, 0)); // Off
}

// Functions to set the motor speed

// Set both speeds
void motorSpeed(int speed) {
  int constrainedSpeed = constrain(speed, speedSlow, speedFast);
  motorASpeed(constrainedSpeed);
  motorBSpeed(constrainedSpeed);
}

// Independently set X
void motorASpeed(int speed) {
  analogWrite(PWMA, speed + motorACorrection);
}

// Independently set Y
void motorBSpeed(int speed) {
  analogWrite(PWMB, speed);
}

// Functions to have the robot move in a certain direction for a specified time

// Forward
void moveForward(int milliseconds) {
  FastLED.showColor(Color(255, 255, 1)); // Yellow
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(BIN1, HIGH);
  // Ease in and out for longer distances
  if (milliseconds == forwardTenIn) {
    for (int i = 0; i <= 25; i++) {
      motorSpeed(speedSlow + 2 * i);
      dddelay(5);
    }
    motorSpeed(speedMed);
    dddelay(milliseconds - 250);
    for (int i = 0; i <= 25; i++) {
      motorSpeed(speedMed - 2 * i);
      dddelay(5);
    }
  } else {
    dddelay(milliseconds);
  }
  motorSpeed(speedSlow);
  FastLED.showColor(Color(0, 0, 0)); // Off
  digitalWrite(STBY, LOW);
}

// Backward
void moveBackward(int milliseconds) {
  FastLED.showColor(Color(255, 255, 1)); // Yellow
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, LOW);
  dddelay(milliseconds);
  FastLED.showColor(Color(0, 0, 0)); // Off
  digitalWrite(STBY, LOW);
}

// Left
void moveLeft(int milliseconds) {
  FastLED.showColor(Color(1, 255, 255)); // Teal
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, LOW);
  digitalWrite(BIN1, HIGH);
  dddelay(milliseconds);
  FastLED.showColor(Color(0, 0, 0)); // Off
  digitalWrite(STBY, LOW);
}

// Right
void moveRight(int milliseconds) {
  FastLED.showColor(Color(1, 255, 255)); // Teal
  digitalWrite(STBY, HIGH);
  digitalWrite(AIN1, HIGH);
  digitalWrite(BIN1, LOW);
  dddelay(milliseconds);
  FastLED.showColor(Color(0, 0, 0)); // Off
  digitalWrite(STBY, LOW);
}

// Turning functions that realign the robot each time

// Align and turn left
void turnLeft() {
  // Move forward until it hits the wall
  Serial.println("Forward");
  moveForward(forwardToClear);
  ddelay(delayLength);

  // Back up a little to clear the turn
  Serial.println("Back");
  moveBackward(backFromRealign);
  ddelay(delayLength);

  // Get the goal value to pass into function below
  mpu.update();
  float initAngle = mpu.getAngleZ();
  Serial.print("AngZ: ");
  Serial.println(initAngle);

  // Execute the actual turn
  Serial.println("Left");
  moveLeft(turnNinetyL);
  ddelay(delayLength);  

  // Check if the turn is accurate using goal
  Serial.println("Check Turn");
  checkAccurateTurn(initAngle, true, false);
  ddelay(delayLength);
}

// Align and turn right
void turnRight() {
  // Move forward until it hits the wall
  Serial.println("Forward");
  moveForward(forwardToClear);
  ddelay(delayLength);

  // Back up a little to clear the turn
  Serial.println("Back");
  moveBackward(backFromRealign);
  ddelay(delayLength);

  // Get the goal value to pass into function below
  mpu.update();
  float initAngle = mpu.getAngleZ();
  Serial.print("AngZ: ");
  Serial.println(initAngle);

  // Execute the actual turn
  Serial.println("Right");
  moveRight(turnNinetyR);
  ddelay(delayLength);

  // Check if the turn is accurate using goal
  Serial.println("Check Turn");
  checkAccurateTurn(initAngle, false, true);
  ddelay(delayLength);
}

// Turning functions that do not reset the robot each time

// Left 90 degrees
void ninetyLeft() {
  // Get the goal value to pass into function below
  mpu.update();
  float initAngle = mpu.getAngleZ();
  Serial.print("AngZ: ");
  Serial.println(initAngle);

  // Execute the actual turn
  Serial.println("Left 90 Deg");
  moveLeft(turnNinetyL);
  ddelay(delayLength);  

  // Check if the turn is accurate using goal
  Serial.println("Check Turn");
  checkAccurateTurn(initAngle, true, false);
  ddelay(delayLength);
}

// Right 90 degrees
void ninetyRight() {
  // Get the goal value to pass into function below
  mpu.update();
  float initAngle = mpu.getAngleZ();
  Serial.print("AngZ: ");
  Serial.println(initAngle);

  // Execute the actual turn
  Serial.println("Right 90 Deg");
  moveRight(turnNinetyR);
  ddelay(delayLength);  

  // Check if the turn is accurate using goal
  Serial.println("Check Turn");
  checkAccurateTurn(initAngle, false, true);
  ddelay(delayLength);
}

// Functions to just move forward/backward

// Move forward 10 inches through one maze segment
void tenInForward() {
  Serial.println("Forward 10in");
  moveForward(forwardTenIn);
  ddelay(delayLength);
}

// Move forward until a certain distance from front wall
void forwardUntil(int inches) {
  float distFromFrontWall;
  do {
    distFromFrontWall = frontUltrasonic();
    moveForward(50);
  } while (distFromFrontWall > inches);
  ddelay(delayLength);
}

// Move backward 10 inches through one maze segment
void tenInBackward() {
  Serial.println("Backward 10in");
  moveForward(forwardTenIn);
  ddelay(delayLength);
}

// Functions that can be called to easily read the ultrasonic sensors

// Front ultrasonic
float frontUltrasonic() {
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  durationF = pulseIn(pingPin, HIGH);
  distanceF = microsecondsToInches(durationF);
  Serial.print("DisF: ");
  Serial.println(distanceF, 2);
  return distanceF;
}

// Right ultrasonic
float rightUltrasonic() {
  digitalWrite(trigPinR, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);
  durationR = pulseIn(echoPinR, HIGH);
  distanceR = durationR * 0.034 / 2.0 / 2.54;
  Serial.print("DisR: ");
  Serial.println(distanceR, 2);
  return distanceR;
}

// Ping conversion
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

// Functions related to hugging aka staying a certain distance away from the right wall

// PD controller to move motors accordingly
void hugWall() {
  // Read and print the output of the right sensor
  Serial.println(" ");
  float distFromRightWall = rightUltrasonic();
  finalPoint = distFromRightWall; 

  // Just move straight if distance is greater than 200
  if (distFromRightWall > 200) {
    Serial.println("Going straight");
    motorSpeed(speedMed);
    moveForward(15);
    ddelay(10);
    return;
  }

  // Calculate and print the proportional and derivative errors
  errorP = distFromRightWall - goalR;  // Positive if too far and negative if too close to wall (should ideally be 0, with max of 2 and min of -2)
  errorD = finalPoint - initialPoint;  // Positive if heading away and negative if going towards wall (should be 0, with very slight variances)

  // Print the errors to the serial monitor
  Serial.print("ErrP: ");
  Serial.println(errorP);
  Serial.print("ErrD: ");
  Serial.println(errorD);

  // Print finalPoint and initialPoint to check if updating correctly
  Serial.print("Final: ");
  Serial.println(finalPoint, 2);
  Serial.print("Initl: ");
  Serial.println(initialPoint, 2);

  // Use proper math to calculate the wheel speed and print it
  leftPWM = (rightPWM + errorP * Gp + errorD * Gd);
  leftPWM = constrain(leftPWM, speedSlow, speedFast);
  Serial.print("lPWM: ");
  Serial.println(leftPWM);
  Serial.print("rPWM: ");
  Serial.println(rightPWM);

  // Actually move the wheels at that speed
  motorASpeed(rightPWM);
  motorBSpeed(leftPWM);
  moveForward(15);

  // Update the memory value
  initialPoint = finalPoint;

  ddelay(10);
}

// Hug the wall for a set amount of times
void hugWallfor(int count) {
  for (int i = 0; i < count; i++) {
    hugWall();
  }
  motorSpeed(speedSlow);
  ddelay(delayLength);
}

// Hug the wall until a certain distance from front wall
void hugWallUntil(int inches) {
  float distFromFrontWall;
  do {
    distFromFrontWall = frontUltrasonic();
    hugWall();
  } while (distFromFrontWall > inches);
  motorSpeed(speedSlow);
  ddelay(delayLength);
}

// Functions related to the MPU6050 gyroscope

// Print the MPU data to the serial monitor
void mpuAngles() {
  // Get angle variables
  mpu.update();
  float angX = mpu.getAngleX();
  float angY = mpu.getAngleY();
  float angZ = mpu.getAngleZ();

  // Print them
  Serial.print("X : ");
	Serial.print(angX);
	Serial.print("\tY : ");
	Serial.print(angY);
	Serial.print("\tZ : ");
	Serial.println(angZ);
}

// Check if the robot has turned the full 90 degrees and finish turning as needed
void checkAccurateTurn(float originalAngle, bool leftTurn, bool rightTurn) {
  // Calculate the angles
  float targetAngle = 0;

  if (leftTurn && !rightTurn) {
    targetAngle = originalAngle + 90;
  } else if (rightTurn && !leftTurn) {
    targetAngle = originalAngle - 90;
  }

  float tolerance = 1;
  float goalMore = targetAngle + tolerance;
  float goalLess = targetAngle - tolerance;

  // Print the angles
  Serial.print("Goal: ");
  Serial.println(targetAngle);
  Serial.print("Less: ");
  Serial.println(goalLess);
  Serial.print("More: ");
  Serial.println(goalMore);
  Serial.print("CurZ: ");
  Serial.println(angZ);

  // Maximum correction attempts
  int attempt = 0;
  const int maxAttempts = 30; 

  // Determine if the robot is basically facing the right direction
  if (angZ < goalMore) {
    // Too far left
    while ((angZ > goalMore || angZ < goalLess) && attempt < maxAttempts) {
      moveLeft(10);
      Serial.print("Slight Left: ");
      Serial.println(angZ);
      ddelay(50);
      attempt++;
    }
  } else if (angZ > goalLess) {
    // Too far right
    while ((angZ > goalMore || angZ < goalLess) && attempt < maxAttempts) {
      moveRight(10);
      Serial.print("Slight Right: ");
      Serial.println(angZ);
      ddelay(50);
      attempt++;
    }
  }

  // Break if reached the maximum attempt count
  if (attempt >= maxAttempts) {
    Serial.println("Max correction attempts reached.");
  } else {
    Serial.println("Angle Fine");
  }
}