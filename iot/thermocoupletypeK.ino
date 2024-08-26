#include <SPI.h>
#include <max6675.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Pin definitions for each thermocouple
int thermoSO[] = {14, 25, 35, 4, 5, 23};
int thermoCS[] = {12, 26, 32, 15, 17, 19};
int thermoSCK[] = {13, 27, 33, 22, 16, 18};

// Temperature control limits
float heat_temp1;
float heat_temp2;
float upperTempLimit;
float lowerTempLimit;
bool relay = false;
bool heat_on = false;

// MAX6675 sensor objects
MAX6675 thermocouple[] = {
  MAX6675(thermoSCK[0], thermoCS[0], thermoSO[0]),
  MAX6675(thermoSCK[1], thermoCS[1], thermoSO[1]),
  MAX6675(thermoSCK[2], thermoCS[2], thermoSO[2]),
  MAX6675(thermoSCK[3], thermoCS[3], thermoSO[3]),
  MAX6675(thermoSCK[4], thermoCS[4], thermoSO[4]),
  MAX6675(thermoSCK[5], thermoCS[5], thermoSO[5])
};

TaskHandle_t Task1;
TaskHandle_t Task2;

QueueHandle_t xQueue;

// Structure to hold sensor data
struct sensorData {
  float c[6];
  float f[6];
};

// WiFi and MQTT settings
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connected to WiFi");
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("temperature/control"); // Subscribe to topic for control messages
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
  pinMode(21, OUTPUT);
  digitalWrite(21, LOW);

  delay(500);

  xQueue = xQueueCreate(10, sizeof(sensorData));

  if (xQueue == NULL) {
    Serial.println("Failed to create queue");
    while (true);
  }

  // Setup WiFi and MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  // Create tasks
  xTaskCreatePinnedToCore(
    taskReadSensor, /* Task function. */
    "ReadSensor",   /* name of task. */
    2048,           /* Stack size. */
    NULL,           /* parameter of the task */
    1,              /* priority of the task. */
    &Task1,         /* Task handle. */
    1);             /* Core where the task should run */

  xTaskCreatePinnedToCore(
    taskSendSerial, /* Task function. */
    "SendSerial",   /* name of task. */
    2048,           /* Stack size. */
    NULL,           /* parameter of the task */
    1,              /* priority of the task. */
    &Task2,         /* Task handle. */
    0);             /* Core where the task should run */
}

void taskReadSensor(void *parameter) {
  sensorData sensorData;

  for (;;) {
    // Read Celsius and Fahrenheit temperatures from all sensors
    for (int i = 0; i < 6; i++) {
      sensorData.c[i] = thermocouple[i].readCelsius();
      sensorData.f[i] = thermocouple[i].readFahrenheit();
    }
    handleSerial();

    // Relay control logic
    heat_temp1 = sensorData.c[1]; // Use sensor 2 for temperature control
    heat_temp2 = sensorData.c[4]; // Use sensor 5 for temperature control
    if (relay) {
      if (heat_temp1 >= upperTempLimit) {
        digitalWrite(21, LOW);
        heat_on = false;
      } else if (heat_temp1 <= lowerTempLimit && !heat_on) {
        digitalWrite(21, HIGH);
        heat_on = true;
      }
    } else {
      digitalWrite(21, LOW);
      heat_on = false;
    }

    // Send data to queue
    if (xQueueSend(xQueue, &sensorData, portMAX_DELAY) != pdPASS) {
      Serial.println("Failed to send data to queue");
    }

    // Send data to MQTT broker
    String mqttMessage = String("0,") + sensorData.c[0] + "," + sensorData.f[0] + "," +
                         sensorData.c[1] + "," + sensorData.f[1] + "," +
                         sensorData.c[2] + "," + sensorData.f[2] + "," +
                         sensorData.c[3] + "," + sensorData.f[3] + "," +
                         sensorData.c[4] + "," + sensorData.f[4] + "," +
                         sensorData.c[5] + "," + sensorData.f[5];
    client.publish("temperature/data", mqttMessage.c_str());

    // Delay to simulate sampling rate
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void taskSendSerial(void *parameter) {
  sensorData sensorData;

  for (;;) {
    // Receive data from queue
    if (xQueueReceive(xQueue, &sensorData, portMAX_DELAY) == pdPASS) {
      // Print Celsius temperatures to serial
      Serial.print("0");
      Serial.print(",");
      Serial.print(sensorData.c[0]);
      Serial.print(",");
      Serial.print(sensorData.f[0]);
      Serial.print(",");
      Serial.print(sensorData.c[1]);
      Serial.print(",");
      Serial.print(sensorData.f[1]);
      Serial.print(",");
      Serial.print(sensorData.c[2]);
      Serial.print(",");
      Serial.print(sensorData.f[2]);
      Serial.print(",");
      Serial.print(sensorData.c[3]);
      Serial.print(",");
      Serial.print(sensorData.f[3]);
      Serial.print(",");
      Serial.print(sensorData.c[4]);
      Serial.print(",");
      Serial.print(sensorData.f[4]);
      Serial.print(",");
      Serial.print(sensorData.c[5]);
      Serial.print(",");
      Serial.println(sensorData.f[5]);
    }

    // Delay to manage serial output rate
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void handleSerial() {
  String inputString = "";
  float inputValue;
  bool validInput = false;

  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      if (inputString.equalsIgnoreCase("start")) {
        relay = true;
        validInput = true;
      } else if (inputString.equalsIgnoreCase("stop")) {
        relay = false;
        validInput = true;
      } else if (inputString.length() > 0 && isDigit(inputString.charAt(0))) {
        inputValue = inputString.toFloat();
        upperTempLimit = inputValue + 2;
        lowerTempLimit = inputValue - 2;

        Serial.println(" ");
        Serial.print("Temperature limit set to: ");
        Serial.println(inputValue);
        Serial.println(" ");
        validInput = true;
      }
      if (!validInput) {
        Serial.println("Invalid input, ignoring...");
      }

      inputString = "";
      validInput = false;
    } else {
      inputString += c;

      // Optional: limit string length to prevent overflow
      if (inputString.length() > 20) {
        Serial.println("Input too long, ignoring...");
        inputString = "";
      }
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "temperature/control") {
    if (message.equalsIgnoreCase("start")) {
      relay = true;
    } else if (message.equalsIgnoreCase("stop")) {
      relay = false;
    } else if (message.length() > 0 && isDigit(message.charAt(0))) {
      float inputValue = message.toFloat();
      upperTempLimit = inputValue + 2;
      lowerTempLimit = inputValue - 2;

      Serial.println(" ");
      Serial.print("Temperature limit set to: ");
      Serial.println(inputValue);
      Serial.println(" ");
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
