#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi settings
const char* ssid = "your_SSID";          // Your Wi-Fi SSID
const char* password = "your_PASSWORD";  // Your Wi-Fi Password

// MQTT Broker settings
const char* mqtt_server = "broker.hivemq.com"; // Replace with your MQTT broker address
const int mqtt_port = 1883;
const char* mqtt_user = "";  // If you have a username for MQTT broker
const char* mqtt_password = "";

// MQTT topics
const char* topic_suhu = "iot/suhu";
const char* topic_waktu = "iot/waktu";
const char* topic_kecepatan = "iot/kecepatan";
const char* topic_power = "iot/power";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(message);

  if (String(topic) == topic_power) {
    if (message == "ON") {
      // Implement your logic when power is ON
      Serial.println("Power ON");
    } else if (message == "OFF") {
      // Implement your logic when power is OFF
      Serial.println("Power OFF");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_power);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Example: Publish sensor data (replace with actual sensor readings)
  int suhu = 25; // replace with actual sensor value
  int waktu = 10; // replace with actual value
  int kecepatan = 1500; // replace with actual value

  client.publish(topic_suhu, String(suhu).c_str());
  client.publish(topic_waktu, String(waktu).c_str());
  client.publish(topic_kecepatan, String(kecepatan).c_str());

  delay(5000); // Adjust delay as needed
}
