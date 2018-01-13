#include <SimpleDHT.h>

/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>
#include <PubSubClient.h>

int pinDHT22 = 4;
SimpleDHT22 dht22;

WiFiMulti WiFiMulti;

char mqttServer[] = "192.168.0.160";
const int mqttPort = 1883;
char mqttUser[] = "";
char clientName[] = "ESP32Clientdht22";
char mqttPassword[] = "";
char channelNameTemp[] = "esp32/temp";
char channelNameHumidity[] = "esp32/humid";

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("SSDID", "PASSWORD");
    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);

    client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientName, mqttUser, mqttPassword )) {
      Serial.println("connected");
 
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}


char charBufTemp[50];
char charBufHumid[50];
int count = 0;
void loop() {

  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.println(err);delay(2000);
    return;
  }
  sprintf(charBufTemp , "%.1f", temperature);
  sprintf(charBufHumid , "%.1f", humidity);
  if (temperature) {
    client.publish(channelNameTemp, charBufTemp, true);
  }
   
   if (humidity) {
    client.publish(channelNameHumidity,charBufHumid, true);
  }

  if (!client.connected()) {
    ESP.restart();
  }
  client.loop();
  
  
  if (!WiFi.isConnected()) {
    Serial.println("Reboot");
    ESP.restart();
  } 
  delay(2500);
}