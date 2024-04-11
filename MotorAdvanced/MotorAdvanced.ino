#define motorPinRightSpeed 5 // D1
#define motorPinLeftSpeed 4  // D? !0

int InputMaxTurningAngle = 90;

// Function prototype
void Move(float TurningAngle, int MaxMotorPower, bool IsReversing);

void setup() {
  // put your setup code here, to run once:
  pinMode(motorPinLeftSpeed, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int x = 1;
  for (int i = -90; i <= 90; i += 10) {
    Serial.println(i); // Print current value of i for debugging
    Move(90, 1024, true); // Example arguments, adjust as needed
    delay(400); // 50ms delay
  }

  delay(1000);
}

void Move(float TurningAngle, int MaxMotorPower, bool IsReversing)
{
  float TurningVector = 0;

  //checking to see if turning angle is out of bounds
  if(TurningAngle > 90 || TurningAngle < -90) {
    Serial.println("Turning angle was outside the bounds of " + String(InputMaxTurningAngle) + " degrees and -" + String(InputMaxTurningAngle) + " degrees");
    return; // Exit function if turning angle is out of bounds
  }

  // Calculate turning vector
  if(TurningAngle != 0) {
    // Vector from -1 to 1
    TurningVector = TurningAngle / InputMaxTurningAngle;
  }
  else {
    TurningVector = 0;
    Serial.println("Turning vector is straight");
  }

  // Calculate motor speeds based on the turning vector
  int leftMotorSpeed = MaxMotorPower + (TurningVector * MaxMotorPower);
  int rightMotorSpeed = MaxMotorPower - (TurningVector * MaxMotorPower);
  //Constrain to motorPower
  rightMotorSpeed = constrain(rightMotorSpeed, 0, MaxMotorPower);
  leftMotorSpeed = constrain(leftMotorSpeed, 0, MaxMotorPower);
  //Activate Motor
  analogWrite(motorPinLeftSpeed, leftMotorSpeed);
  analogWrite(motorPinRightSpeed, rightMotorSpeed);

  //debug
  Serial.println("-------------------");
  Serial.print("Turning Angle: ");
  Serial.println(TurningAngle);
  Serial.print("Turning Vector: ");
  Serial.println(TurningVector);
  Serial.print("Left Motor Speed: ");
  Serial.println(leftMotorSpeed);
  Serial.print("Right Motor Speed: ");
  Serial.println(rightMotorSpeed);

}
