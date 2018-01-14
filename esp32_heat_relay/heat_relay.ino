#include <WiFi.h>
#include <PubSubClient.h>

int switchPin = 4;
char *channel = "heat/switch";
int isOn = 1;
char *CLIENT_NAME = "ESP32_heat";

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char msg_buffer[length];
  if (strcmp(topic, channel) == 0) {
    char *cstring = (char *) payload;
    cstring[length] = '\0';
    int status = atoi(cstring);
    if (status == 1) {
      digitalWrite(switchPin, HIGH);
      isOn = 1;
    } else if (status == 0) {
      digitalWrite(switchPin, LOW);
      isOn = 0;
    }  
  }
}

 
const char* ssid = "SSID_HERE";
const char* password =  "PASSWORD";
const char* mqttServer = "MQTT_SERVER_IP";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
bool isConnectingToWifi = false;
 
WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
  if (isConnectingToWifi) {
    return;
  }
  
  WiFi.begin(ssid, password);
  isConnectingToWifi = true;
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  isConnectingToWifi = false;
}
 
void setup() {
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, HIGH); 
 
  Serial.begin(115200);
  setupWifi();
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqtt_callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(CLIENT_NAME, mqttUser, mqttPassword )) {
      Serial.println("connected");
      client.subscribe(channel);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_NAME)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe(channel);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int count = 0;
int rebootCount = 0;
char buf[1];
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop(); 
  delay(1000);
  
  if (!WiFi.isConnected()) {
    ESP.restart();
  } 
  if (rebootCount == 3600) {
    ESP.restart();
  }
  
  // Publish status every count times
  if (count == 15) {
    client.publish(channel,itoa(isOn,buf, 10), true);
    count = 0;
  }
  count++;
  rebootCount++;
  
}