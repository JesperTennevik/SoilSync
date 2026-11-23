#include <Arduino.h>

#include "DeviceSettings.h"
#include "SettingsStorage.h"
#include "AWSConnection.h"

#define SENSOR_PIN A0
#define DRY_VALUE 3090
#define WET_VALUE 1100

DeviceSettings settings;
void printSettings(){
    Serial.println();
    Serial.print("DeviceId: ");
    Serial.println(settings.deviceId);
    
    Serial.print("MeasurementInterval: ");
    Serial.println(settings.measurementInterval);

    Serial.print("WifiSSID: ");
    Serial.println(settings.wifiSSID);

    Serial.print("WifiPassword: ");
    Serial.println(settings.wifiPassword);
}

int readMoisture(){
    long sum = 0;

    for(int i = 0; i < 20; i++){
        sum += analogRead(SENSOR_PIN);
    }

    int avg = sum / 20;

    int moisturePercent = map(
        avg,
        DRY_VALUE,
        WET_VALUE,
        0,
        100
    );

    return constrain(
        moisturePercent,
        0,
        100
    );
}

void setup() {
    Serial.begin(115200);
    delay(5000);

    SettingsStorage::load(settings);
    printSettings();
    AWSConnection::connectAWS(settings);
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

void loop() {
    AWSConnection::loop();

    // static unsigned long lastSettingsPrint = 0;
    // if(millis() - lastSettingsPrint >= 5000){
    //     printSettings();

    //     Serial.println(F("------------------"));

    //     lastSettingsPrint = millis();
    // }

    if(!validSettings(settings)){
        return;
    }

    static unsigned long previousMillis = millis() - settings.measurementInterval;
    if (millis() - previousMillis >= settings.measurementInterval) {
        previousMillis = millis();

        int moisture = readMoisture();

        Serial.println("--------------------------------");
        Serial.print("Moisture: ");
        Serial.print(moisture);
        Serial.println("%");

        bool sendResult = AWSConnection::publishTelemetry(
            "{\"moisture\":" + String(moisture) + "}"
        );

        if(!sendResult){
            Serial.println("Loop failed to send telemetry to AWS");
            Serial.println("Restarting ESP32");
            ESP.restart();
        }
    }
}