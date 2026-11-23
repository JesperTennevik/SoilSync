#pragma once
#include <Webserver.h>

#include "DeviceSettings.h"


class ConfigServer {
    static WebServer server;
    static DeviceSettings* settingsPtr;
    
    static void handleRoot();
    static void handleSave();

    public:
        static void start(DeviceSettings &settings);

};