#include "SettingsStorage.h"
#include <Preferences.h>
#include <WiFi.h>

void SettingsStorage::load(DeviceSettings &settings){
    Preferences preferences;
    if (!preferences.begin("device-config", true)) {
        Serial.println("No settings found.");
        return;
    }

    bool loaded =
        preferences.getBytesLength("settings") == sizeof(settings) &&
        preferences.getBytes("settings", &settings, sizeof(settings)) == sizeof(settings);

    preferences.end();

    if (settings.deviceId[0] == '\0'){
        uint8_t mac[6];
        WiFi.macAddress(mac);
        snprintf(settings.deviceId, sizeof(settings.deviceId), "%02X%02X%02X%02X%02X%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        SettingsStorage::save(settings);
    }
}

bool SettingsStorage::save(const DeviceSettings &settings){
    Preferences preferences;

    if (!preferences.begin("device-config", false)) {
    return false;
    }

    size_t written = preferences.putBytes("settings", &settings, sizeof(settings));
    preferences.end();

    return written == sizeof(settings);
}