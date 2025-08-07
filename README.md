# Aarogya Kit

This project utilizes an ESP32 to monitor heart rate (BPM), SpO2 (blood oxygen levels), and body temperature using a MAX30100 Pulse Oximeter sensor and DS18B20 temperature sensor. The data is served over a simple web server with an endpoint to get the sensor readings in JSON format.

## Components
- **MAX30100 Pulse Oximeter**: Measures heart rate (BPM) and blood oxygen levels (SpO2).
- **DS18B20 Temperature Sensor**: Measures body temperature.
- **ESP32**: A microcontroller with WiFi capabilities for data communication.
- **Web Server**: Allows the user to access sensor data over a network.

## Features
- Web server running on the ESP32 to serve sensor data as JSON.
- Displays real-time sensor readings every second.
- Displays heart rate, SpO2, and body temperature values.
- Retries MAX30100 initialization if it fails.
- Configurable WiFi credentials for network access.

## Configuration
- **WiFi**: Configure WiFi credentials (SSID and password) in the `ssid` and `password` variables in the code.
- **Sensor Pin**: The DS18B20 temperature sensor is connected to GPIO 5 (modifiable).

## Wiring
- **MAX30100 Pulse Oximeter**: Use I2C pins (SDA, SCL) of ESP32 for communication.
- **DS18B20 Temperature Sensor**: Connect to GPIO 5 with a 4.7kΩ pull-up resistor on the data line.

## How to Use
1. Upload the code to your ESP32 using the Arduino IDE.
2. Connect your ESP32 to your WiFi network by updating the `ssid` and `password` variables.
3. Open the Serial Monitor to view debugging information.
4. Access the sensor data by navigating to `http://<ESP32_IP_ADDRESS>/data` in a web browser.

## Dependencies

- **MAX30100_PulseOximeter**: Library for interfacing with the MAX30100 pulse oximeter sensor to measure heart rate (BPM) and SpO2.
- **WiFi**: Built-in ESP32 library for WiFi functionality, enabling the ESP32 to connect to a wireless network.
- **WebServer**: ESP32 library for setting up a web server to serve sensor data as JSON over HTTP.
- **DallasTemperature**: Library for reading temperature data from the DS18B20 temperature sensor via the OneWire protocol.


## Example Output (JSON)
```json
{
  "temperature": 98.75,
  "bpm": 72.34,
  "spo2": 98.12
}
