
# IoT Dashboard with NodeMCU and MQTT

This project is an IoT Dashboard that displays sensor data and allows control over parameters like temperature, time, and RPM fan speed. The dashboard is connected to a NodeMCU (ESP8266) microcontroller, which communicates with the dashboard using MQTT protocol.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Setup Instructions](#setup-instructions)
- [Usage](#usage)
- [Code Overview](#code-overview)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project provides an IoT solution where a NodeMCU microcontroller reads sensor data and sends it to an MQTT broker. The data is displayed on a web-based dashboard that allows users to monitor and control various parameters. The dashboard updates in real-time and can also send commands back to the NodeMCU.

## Features

- **Real-Time Monitoring:** View sensor data such as temperature directly on the dashboard.
- **Control Parameters:** Adjust temperature, time, and RPM fan speed from the dashboard.
- **Power Control:** Turn the power ON/OFF via the dashboard.
- **MQTT Communication:** Reliable communication using the MQTT protocol.

## Hardware Requirements

- NodeMCU (ESP8266)
- Sensors (e.g., DHT11, DHT22 for temperature)
- Fan (for RPM control)
- Resistors, breadboard, and connecting wires
- Power supply (e.g., USB cable)

## Software Requirements

- Arduino IDE
- NodeMCU/ESP8266 Board Package installed in Arduino IDE
- MQTT Broker (e.g., HiveMQ, Mosquitto)
- Web browser for the dashboard

### Arduino Libraries

Make sure to install the following libraries in the Arduino IDE:

- [ESP8266WiFi](https://github.com/esp8266/Arduino)
- [PubSubClient](https://github.com/knolleary/pubsubclient)

## Circuit Diagram

The NodeMCU should be connected to your sensors and actuators (like a fan) based on the specific components you're using. Below is a general connection overview:

```
+-------------------+
|     NodeMCU       |
|                   |
| GPIOx -> Sensor 1 |
| GPIOy -> Sensor 2 |
| GPIOz -> Fan PWM  |
+-------------------+
```

Refer to the specific datasheets for your sensors and actuators to connect them to the appropriate GPIO pins.

## Setup Instructions

1. **Clone the Repository:**
   ```
   git clone https://github.com/yourusername/IoT-Dashboard.git
   cd IoT-Dashboard
   ```

2. **Configure the Arduino Code:**
   - Open the `NodeMCU_MQTT.ino` file in the Arduino IDE.
   - Replace `your_SSID` and `your_PASSWORD` with your Wi-Fi credentials.
   - Replace `broker.hivemq.com` with your MQTT broker address, if different.
   - Modify the MQTT topics as needed.

3. **Upload the Code to NodeMCU:**
   - Connect the NodeMCU to your computer via USB.
   - Select the correct board and port in the Arduino IDE.
   - Upload the code to the NodeMCU.

4. **Host the Web Dashboard:**
   - Open the `index.html` file in a web browser.
   - Ensure the MQTT broker details in the JavaScript (`mqtt.connect(...)`) are correct.

## Usage

- **Monitoring:** View the sensor data on the web dashboard in real-time.
- **Control:** Use the buttons on the dashboard to adjust temperature, time, and RPM.
- **Power:** Toggle the power button to turn the connected device on or off.

### Real-Time Updates

The dashboard connects to the MQTT broker via WebSockets and updates in real-time as data is received from the NodeMCU.

## Code Overview

### Arduino Code (`NodeMCU_MQTT.ino`)

- **WiFi Connection:** Connects to the specified Wi-Fi network.
- **MQTT Client:** Publishes sensor data and subscribes to control topics.
- **Callback Function:** Handles incoming messages (e.g., turning the power on/off).

### Web Dashboard (`index.html`)

- **HTML Structure:** Defines the layout of the sensor displays and control buttons.
- **JavaScript:** Handles MQTT connections, subscribes to topics, and updates the UI.
- **CSS:** Styles the dashboard for a clean and modern look.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
