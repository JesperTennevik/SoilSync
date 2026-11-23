#pragma once
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include "aws_secrets_E8F60AC04A24.h"
#include "DeviceSettings.h"

#define AWS_IOT_PUBLISH_TOPIC "/telemetry"
#define AWS_IOT_SUBSCRIBE_TOPIC "/downlink"

class AWSConnection{
    static WiFiClientSecure wifiClient; 
    static MQTTClient mqttClient;
    static DeviceSettings* settingsPtr;

    static void setupShadow();
    
    static bool connectWifi(const char* wifiSSID, const char* wifiPassword);
    static void messageHandler(String &topic, String &payload);
    static void reportMeasurementInterval(unsigned long interval);

    public:
        static void connectAWS(DeviceSettings &settings);
        static bool publishAWS(String payload, const char* topic);
        static bool publishTelemetry(String payload);
        static bool loop();

};