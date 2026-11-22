#pragma once

#include "DeviceSettings.h"

class SettingsStorage{
    public:
        static void load(DeviceSettings &settings);
        static bool save(const DeviceSettings &settings);
};