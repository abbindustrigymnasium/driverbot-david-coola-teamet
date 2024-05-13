#define motorPinLeftSpeed 4  // D? !0
#define motorPinLeftDir 0

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID "ABBgym_2.4"
#define WIFI_PASSWORD "mittwifiarsabra"

Servo s1;  // create servo object to control a servo

int WifiUpdateDelay = 200;

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Broker
const char *mqtt_broker = "maqiatto.com";
const char *topicX = "catidifat@gmail.com/Xmove";
const char *topicY = "catidifat@gmail.com/Ymove";

const char *mqtt_username = "catidifat@gmail.com";
const char *mqtt_password = "Mybroker";
const int mqtt_port = 1883;

  void setup() {
  s1.attach(D2, 500, 2400);
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
  Serial.println("Subscribing to topic🤠🤠: " );
  Serial.print(topicX);
  Serial.print(" and ");
  Serial.print(topicY);
  // publish and subscribe
  client.subscribe(topicX);
  client.subscribe(topicY);
  // put your setup code here, to run once:
  pinMode(motorPinLeftSpeed, OUTPUT);
  pinMode(motorPinLeftDir, OUTPUT);
  analogWrite(motorPinLeftSpeed, 1024);
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

    // Map the payload value to servo angles using linear interpolation
    float angle;

    // Linear interpolation between -1 and 1
    angle = ((floatValue) + 1) * 90; // Scale the value to range from 0 to 180

    if (strcmp(topic, topicX) == 0) {
        // Process the mapped angle value here
        s1.write(angle); // Write the angle to the servo
    }
}


void loop() {
  client.loop();
  //digitalWrite(motorPinLeftDir, HIGH);
  
}


