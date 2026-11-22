#pragma once

#include <WiFi.h>

typedef struct {
    char deviceId[37];
    char deviceLocation[33];
    uint16_t measurementInterval;
    
    char wifiSSID[33];
    char wifiPassword[65];
} DeviceSettings;

template <size_t N>
bool hasTerminator(const char (&value)[N]) {
    return memchr(value, '\0', N) != nullptr;
}

inline bool validSettings(const DeviceSettings& settings){
    if (!hasTerminator(settings.deviceId) ||
        !hasTerminator(settings.deviceLocation) ||
        !hasTerminator(settings.wifiSSID) ||
        !hasTerminator(settings.wifiPassword)
    ){
        return false;
    }

    if (settings.deviceId[0] == '\0' ||
        settings.deviceLocation[0] == '\0' ||
        settings.wifiSSID[0] == '\0' ||
        settings.wifiPassword[0] == '\0'
    ){
        return false;
    }

    if(settings.measurementInterval < 1000){
        return false;
    }

    return true;
}