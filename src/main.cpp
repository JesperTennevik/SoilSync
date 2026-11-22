#include <Arduino.h>
#include "DeviceSettings.h"
#include "SettingsStorage.h"

DeviceSettings settings;

void printSettings(){
    Serial.print("DeviceId: ");
    Serial.println(settings.deviceId);

    Serial.print("DeviceLocation: ");
    Serial.println(settings.deviceLocation);
    
    Serial.print("MeasurementInterval: ");
    Serial.println(settings.measurementInterval);

    Serial.print("WifiSSID: ");
    Serial.println(settings.wifiSSID);

    Serial.print("WifiPassword: ");
    Serial.println(settings.wifiPassword);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    SettingsStorage::load(settings);
}

void loop() {
    static unsigned long lastSettingsPrint = 0;
    if(millis() - lastSettingsPrint >= 5000){
        printSettings();

        Serial.print(F("------------------"));

        lastSettingsPrint = millis();
    }
}