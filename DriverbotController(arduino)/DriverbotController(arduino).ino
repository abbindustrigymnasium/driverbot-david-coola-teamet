#define motorPinSpeed 4  //Pin definition for motor speed
#define motorPinDir 2 //Pin definition to reverse the motor

#include <SharpIR.h> //custom sharp ir library to recive inputs from the sensor
#include <Servo.h>//Servo library for the steering and rotating the sensor
#include <ESP8266WiFi.h>//WIFI library to connect the esp to a local network
#include <PubSubClient.h>//Library to then connect connect the esp to a broker through the WIFI

#define WIFI_SSID "ABBgym_2.4"//Static variable for WIFI SSID
#define WIFI_PASSWORD "mittwifiarsabra"//Static variable for WIFI password

Servo SteeringServo;  //Object to control servo steering
Servo SensorServo;  //Object to control servo sensor control

SharpIR sensor( SharpIR::GP2Y0A41SK0F, A0 ); //Object and pin definition for SHARP IR sensor

int WifiUpdateDelay = 200; //Delay to wait in between trying again when connection has failed.

WiFiClient espClient; //Object to create a client for WIFI
PubSubClient client(espClient); //Object to create a client for broker connection

// MQTT Broker
const char *mqtt_broker = "maqiatto.com";//Broker URL
const char *topicX = "catidifat@gmail.com/Xmove";//Topic for messages on the X axis
const char *topicY = "catidifat@gmail.com/Ymove";//Topic for messages on the Y axis
const char *AutonomousTopic = "catidifat@gmail.com/AutonomousActivation";//Topic for messages to toggle self driving

const char *mqtt_username = "catidifat@gmail.com";//Username when logging in on the broker
const char *mqtt_password = "Mybroker";//Password when logging in on the broker(Very secure)
const int mqtt_port = 1883;//Port for the broker (TCP Port since esp cant handle websockets)

int MaxPower = 1024;//The max power that can be outputed from the website
bool IsSelfDriving = false;//Dynamic variable for the state of Self driving
int SensorDirection = 0;  //Variable to store the current position of the steering servo
int consecutiveHits;//Variable to confirm that the sensor data is legitamate by testing the value multiple times

void setup() {
  SteeringServo.attach(D7, 500, 2400);//attach steering servo pin with fix for more than 180°
  SensorServo.attach(D8, 500, 2400);//attach sensor servo pin with fix for more than 180°
  Serial.begin(9600);//Setting baud rate
  delay(500);//delay so the serial monitor can keep up 🙏
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);//Begin connecting to wifi with SSID/Pass
  Serial.print("Connecting to WiFi🛜");

  //while not connected to WIFI add progression dots in the serial monitor to show that a connection is being attempted
  while (WiFi.status() != WL_CONNECTED) {
    delay(WifiUpdateDelay);
    Serial.print(".");
  }

  //Pub sub client set broker URL and port
  client.setServer(mqtt_broker, mqtt_port);
  //Set function when recieving messages
  client.setCallback(callback);
  Serial.println("Connecting to broker client🛜");
  //While loop when not connected to broker show the user that a connection is being attempted
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
  
  //Serial prints to show info such as connection established and what topics are subscribed to
  Serial.println("");
  Serial.println("Connection established to the broker 🤑🤑" );
  Serial.print("Subscribing to topic🤠🤠: " );
  Serial.print(topicX);
  Serial.print(", ");
  Serial.print(topicY);
  Serial.print(" and ");
  Serial.print(AutonomousTopic);

  //Subscribe to the topics to receive messages from them
  client.subscribe(topicX);
  client.subscribe(topicY);
  client.subscribe(AutonomousTopic);

  //Setting the pin mode for the motor
  pinMode(motorPinSpeed, OUTPUT);
  pinMode(motorPinDir, OUTPUT);
  //Ensure that motor is stopped on startup
  analogWrite(motorPinSpeed, 0);
  digitalWrite(motorPinDir, HIGH);

}
//Function to recieve messages topic and payload
void callback(char *topic, byte *payload, unsigned int length) {
    // Convert the payload to a string
    char payloadStr[length + 1]; // +1 for null terminator
    for (int i = 0; i < length; i++) {
        payloadStr[i] = (char) payload[i];
    }
    payloadStr[length] = '\0'; // Null terminate the string

    
    float floatValue = atof(payloadStr);// Convert the string to a float for servo and motor calculation
    float angle;//Define a variable for the steering servo angle
    float MotorPower;//Define a variable for the Motor speed
    //Simple toggle function to toggle autonomous when recieving messages on that topic
    if (strcmp(topic, AutonomousTopic) == 0) {
      IsSelfDriving = !IsSelfDriving;//flip the self driving variable
      //Show self driving state with Serial.print
      Serial.println("");
      Serial.print("Set Self driving status to: ");
      Serial.print(IsSelfDriving);
      delay(100);
      analogWrite(motorPinSpeed, 0);//Ensure that the motor is stopped before activating/deactivating self driving
    }
    //Check if self driving to not recive double inputs from self driving/manual input
    if(!IsSelfDriving)
    {
      //Check if topic has been recieved  on the X axis
      if (strcmp(topic, topicX) == 0) {
        // Linear interpolation from between -1 and 1 to 0 to 180
        angle = ((floatValue) + 1) * 90; // Scale the value to range from 0 to 180
        SteeringServo.write(angle); // Process the mapped angle value to the steering servo
      }
      if (strcmp(topic, topicY) == 0) {
      
        //Use the max power and the message payload to calculate Motor power
        MotorPower = floatValue * MaxPower;
        
        //Check to see if motor power is in the forward direction
        if (MotorPower > 0)
        {
          digitalWrite(motorPinDir, HIGH);//Set the motor direction to forward
        }
        //Check to see if motor power is in the backward direction
        else if (MotorPower < 0)
        {
          digitalWrite(motorPinDir, LOW);//Set the motor direction to backward
          MotorPower *= -1;//invert the motorpower since negative values are not accepted in analogWrite
        }
        analogWrite(motorPinSpeed, int(MotorPower));//write motorpower to the motor  
      }
    }
    
}


void loop() {
  client.loop();//Loop client to check for new messages etc...

  //Here the self driving logic starts so we check if seldriving is true
  if(IsSelfDriving)
  {
    analogWrite(motorPinSpeed, 512);//Start the motor at half speed to drive forward  
    //Rotate the sensor in the range of +90° and -90 ° in each direction  
    for (int pos = 0; pos <= 180; pos += 10) {
      //if sensor position is less then 90° then use that to write to the servo
      if (pos <= 90) { 
        SensorDirection = pos;  // update direction variable
        SensorServo.write(SensorDirection);  // tell servo to go to position in variable 'direction'
      } 
      //if sensor position is more then 90° then flip the angle using 180-sensor_servo_pos
      else {
        SensorDirection = 180 - pos;  // update direction variable
        SensorServo.write(SensorDirection);  // tell servo to go to position in variable 'direction'
      }
      int distance = sensor.getDistance(); //Calculate the distance in centimeters and store the value in a variable from the SHARP IR sensor
      //If the object/wall is close then start detection
      if(distance > 5 && distance < 40)
      {
        consecutiveHits++;//Count how many times a wall has been detected
        if(consecutiveHits == 2)//if you get a object two times in a row then start steering
        {
          int SteeringDir = SensorDirection * 2; //Create a steering variable that is 2 times as large as the sensor dir since the range is larger
          //Serial.print distance to the wall as well as steering info
          Serial.print(distance);
          Serial.print(" steering with angle: ");
          Serial.print(SteeringDir);
          Serial.println("");
          SteeringServo.write(SteeringDir);//Write the information to the servo
          delay(200); //delay to let the sensor/servo catch up
        }
      }
      else
      {
        consecutiveHits = 0; //Reset the count if a wall isnt detected to ensure reliable values
      }
      delay(50);//delay to control the speed
    }
  }
  
}
