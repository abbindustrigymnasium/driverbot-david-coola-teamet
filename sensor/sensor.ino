#include <SharpIR.h>

SharpIR sharpSens(3, A0);

void setup() 
{
}

void loop() {
  
  int dis = sharpSens.getDistance();
  Serial.print(dis);
}