#define motorPinLeftSpeed 4  // D? !0
#define motorPinLeftDir 0

void setup() {
  // put your setup code here, to run once:
  pinMode(motorPinLeftSpeed, OUTPUT);
  pinMode(motorPinLeftDir, OUTPUT);
  Serial.begin(9600);
  analogWrite(motorPinLeftSpeed, 1024);
  digitalWrite(motorPinLeftDir, HIGH);

}

void loop() {
  digitalWrite(motorPinLeftDir, HIGH);
  delay(1000);
  analogWrite(motorPinLeftSpeed, 0);
  delay(1000);
  analogWrite(motorPinLeftSpeed, 1024);
  digitalWrite(motorPinLeftDir, LOW);
  delay(1000);


}


