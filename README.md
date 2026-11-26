# SoilSync

SoilSync is an IoT project for the Arduino Nano ESP32. It reads soil moisture, sends the measurement to AWS IoT over MQTT, and allows the measurement interval to be changed through the AWS IoT device shadow.


The data is visualized via a [dashboard website](https://github.com/JesperTennevik/SoilSyncDashboard).

## How it works

- Reads the soil-moisture sensor through analog pin `A0`.
    - Defined in `src/main.cpp` along with the clibration upper and lower bounds.
- Measures soil moisture on configured interval.
- Publishes JSON telemetry to `<device-id>/telemetry`.
- Subscribes to `<device-id>/downlink` for incoming messages.
- Syncs `measurementInterval` with the AWS IoT device shadow.
- Saves device settings in ESP32 NVS flash storage.
- Starts a Wi-Fi setup access point if no valid Wi-Fi credentials are saved.

## Hardware

- Arduino Nano ESP32
- Analog soil-moisture sensor

Connect the sensor signal output to `A0`, VCC to a power pin, and GND to GND.

## Requirements

- PlatformIO
- An AWS IoT thing and certificates
- Wi-Fi network credentials

## AWS IoT setup

1. Create an AWS IoT thing.
2. Create and download a device certificate, private key, and the Amazon Root CA certificate.
3. Attach an IoT policy that allows the required MQTT connect, subscribe, and publish actions.
4. Add the AWS IoT endpoint and certificates to `aws_secrets.h`.

## Wi-Fi setup

If the saved Wi-Fi connection fails:
1. Connect the ESP32 to the `ESP32-Setup` Wi-Fi access point.
2. Open `http://192.168.4.1`.
3. Enter the Wi-Fi SSID and password.
4. Save the settings. The device will restart and try to connect again.

## Build and upload

1. Open the project in a PlatformIO compatible environment.
2. Connect the Arduino Nano ESP32.
3. Run **PlatformIO: Build**.
4. Run **PlatformIO: Upload**.
