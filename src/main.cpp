#include <Arduino.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "AWSConnection.h"

DeviceSettings settings;
void printSettings(){
    Serial.print("DeviceId: ");
    Serial.println(settings.deviceId);
    
    Serial.print("MeasurementInterval: ");
    Serial.println(settings.measurementInterval);

    Serial.print("WifiSSID: ");
    Serial.println(settings.wifiSSID);

    Serial.print("WifiPassword: ");
    Serial.println(settings.wifiPassword);
}

bool sendTelemetry() {
  Serial.println("Sending telemetry to AWS");
  bool sendResult =
      AWSConnection::publishTelemetry(
        "{\"temperature\":" + String(random(15.0, 30.0)) +
        ",\"humidity\":" + String(random(50, 90)) + "}"
      );

  return sendResult;
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    SettingsStorage::load(settings);
    
    AWSConnection::connectAWS(settings);
}

void loop() {
    AWSConnection::loop();

    static unsigned long lastSettingsPrint = 0;
    if(millis() - lastSettingsPrint >= 5000){
        printSettings();

        Serial.println(F("------------------"));

        lastSettingsPrint = millis();
    }

    if(!validSettings(settings)){
        return;
    }

    static unsigned long previousMillis = millis() - settings.measurementInterval;

    if (millis() - previousMillis >= settings.measurementInterval) {
        previousMillis = millis();

        bool sendResult = sendTelemetry();
        if (sendResult == false) {
        Serial.println("Loop failed to send telemetry to AWS");
        Serial.println("Restarting ESP32");
        ESP.restart();
        }
    }
}