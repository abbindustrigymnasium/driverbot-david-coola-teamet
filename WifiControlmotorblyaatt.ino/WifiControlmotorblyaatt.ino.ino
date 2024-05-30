#define motorPinLeftSpeed 4  // D? !0
#define motorPinLeftDir 2
#define distanceTriggerPin D5
#define distanceEchoPin D6
long duration;

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID "ABBgym_2.4"
#define WIFI_PASSWORD "mittwifiarsabra"

Servo SteeringServo;  // create servo object to control a servo
Servo SensorServo;  // create servo object to control a servo

int WifiUpdateDelay = 200;

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Broker
const char *mqtt_broker = "maqiatto.com";
const char *topicX = "catidifat@gmail.com/Xmove";
const char *topicY = "catidifat@gmail.com/Ymove";
const char *AutonomousTopic = "catidifat@gmail.com/AutonomousActivation";

const char *mqtt_username = "catidifat@gmail.com";
const char *mqtt_password = "Mybroker";
const int mqtt_port = 1883;
int MaxPower = 1024;
bool IsSelfDriving = false;
int SensorDirection = 0;  // variable to store the current position of the servo

void setup() {
  pinMode(distanceTriggerPin, OUTPUT);
  pinMode(distanceEchoPin, INPUT);
  digitalWrite(distanceTriggerPin, LOW);
  SteeringServo.attach(D7, 500, 2400);//attach servo with fix for more than 180°
  SensorServo.attach(D8, 500, 2400);
  Serial.begin(9600);
  //delay so the serial monitor can keep up 🙏
  delay(500);
  // connecting to a WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi🛜");
  while (WiFi.status() != WL_CONNECTED) {
    delay(WifiUpdateDelay);
    Serial.print(".");
  }
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  Serial.println("Connecting to broker client🛜");
  while(!client.connected())
  {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.print(".");
    if (!client.connect(client_id.c_str(), mqtt_username, mqtt_password)) 
    {
      Serial.println("failed with state 🥺🥺");
      Serial.print(client.state());
      delay(2000);
      Serial.println("");
      Serial.println("Trying again😤😤");
    } 
    
  }
  

  Serial.println("");
  Serial.println("Connection established to the broker 🤑🤑" );
  Serial.print("Subscribing to topic🤠🤠: " );
  Serial.print(topicX);
  Serial.print(", ");
  Serial.print(topicY);
  Serial.print(" and ");
  Serial.print(AutonomousTopic);
  // publish and subscribe
  client.subscribe(topicX);
  client.subscribe(topicY);
  client.subscribe(AutonomousTopic);
  // put your setup code here, to run once:
  pinMode(motorPinLeftSpeed, OUTPUT);
  pinMode(motorPinLeftDir, OUTPUT);
  analogWrite(motorPinLeftSpeed, 0);
  digitalWrite(motorPinLeftDir, HIGH);

}

void callback(char *topic, byte *payload, unsigned int length) {
    // Convert the payload to a string
    char payloadStr[length + 1]; // +1 for null terminator
    for (int i = 0; i < length; i++) {
        payloadStr[i] = (char) payload[i];
    }
    payloadStr[length] = '\0'; // Null terminate the string

    // Convert the string to a float
    float floatValue = atof(payloadStr);
    float angle;
    float MotorPower;
    if (strcmp(topic, AutonomousTopic) == 0) {
      IsSelfDriving = !IsSelfDriving;
      Serial.println("");
      Serial.print("Set Self driving status to: ");
      Serial.print(IsSelfDriving);
    }
    if(!IsSelfDriving)
    {
      if (strcmp(topic, topicX) == 0) {
        // Map the payload value to servo angles using linear interpolation

        // Linear interpolation between -1 and 1
        angle = ((floatValue) + 1) * 90; // Scale the value to range from 0 to 180
        // Process the mapped angle value here
        SteeringServo.write(angle); // Write the angle to the servo
      }
      if (strcmp(topic, topicY) == 0) {
      
        // Process the mapped angle value here
        MotorPower = floatValue * MaxPower;
        
        if (MotorPower > 0)
        {
          digitalWrite(motorPinLeftDir, HIGH);
        }
        else if (MotorPower < 0)
        {
          digitalWrite(motorPinLeftDir, LOW);
          MotorPower *= -1;
        }
        analogWrite(motorPinLeftSpeed, int(MotorPower));       
      }
    }
    
}


void loop() {
  client.loop();
  SensorServo.write(90);
  
  if(IsSelfDriving)
  {
    for (int pos = 0; pos <= 360; pos += 2) {
      if (pos <= 180) {
        SensorDirection = pos;  // update direction variable
        SensorServo.write(SensorDirection);  // tell servo to go to position in variable 'direction'
      } 
      else {
        SensorDirection = 360 - pos;  // update direction variable
        SensorServo.write(SensorDirection);  // tell servo to go to position in variable 'direction'
      }
      digitalWrite(distanceTriggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(distanceTriggerPin, LOW);
      duration = pulseIn(distanceEchoPin, HIGH);
      Serial.println(duration / 29 / 2);
      delay(100);  
    }
  }
  
}
