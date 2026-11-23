#include "AWSConnection.h"
#include "ConfigServer.h"
#include "SettingsStorage.h"
#include <ArduinoJson.h>

WiFiClientSecure AWSConnection::wifiClient = WiFiClientSecure();
MQTTClient AWSConnection::mqttClient = MQTTClient(1024);
DeviceSettings* AWSConnection::settingsPtr = nullptr;

bool AWSConnection::connectWifi(const char* wifiSSID, const char* wifiPassword){    
    if(!(strlen(wifiSSID) > 0 && strlen(wifiPassword) > 0)){
        return false;
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPassword);

    unsigned long start = millis();
    unsigned long timeout = 10 * 1000;

    while(WiFi.status() != WL_CONNECTED){
        if(millis() - start > timeout){
            return false;
        }

        delay(500);
        Serial.print(".");
    }

    return true;
}

void AWSConnection::connectAWS(DeviceSettings &settings){
    settingsPtr = &settings;
    
    if(!connectWifi(settings.wifiSSID, settings.wifiPassword)){
        ConfigServer::start(settings);
    }
    
    wifiClient.setCACert(AWS_CERT_CA);
    wifiClient.setCertificate(AWS_CERT_CRT);
    wifiClient.setPrivateKey(AWS_CERT_PRIVATE);

    mqttClient.begin(AWS_IOT_ENDPOINT, 8883, wifiClient);

    char tlsError[100];
    if(wifiClient.connect(AWS_IOT_ENDPOINT, 8883)){
        Serial.println("TLS verification succeeded");
    }
    else {
        int code = wifiClient.lastError(tlsError, sizeof(tlsError));
        Serial.printf("TLS verification failed (%d): %s\n", code, tlsError);
        time_t now = time(nullptr);
        Serial.print("Device time: ");
        Serial.println(ctime(&now));
        return;
    }

    mqttClient.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while(!mqttClient.connect(settings.deviceId)){
        Serial.print(".");
        delay(100);
    }

    if (!mqttClient.connected()) {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    char topic[64];
    snprintf(topic, sizeof(topic), "%s%s", settings.deviceId, AWS_IOT_SUBSCRIBE_TOPIC);

    bool res = mqttClient.subscribe(topic);
    if(!res){
        Serial.println("\n\rSubscribe Failed. Restarting...");
        delay(5);
        ESP.restart();
    }
    setupShadow();

    Serial.println("AWS IoT Connected!");
}

void AWSConnection::setupShadow(){
    char topic[64];

    snprintf(topic, sizeof(topic), "$aws/things/%s/shadow/get/accepted", settingsPtr->deviceId); 
    mqttClient.subscribe(topic);
    snprintf(topic, sizeof(topic), "$aws/things/%s/shadow/get/rejected", settingsPtr->deviceId); 
    mqttClient.subscribe(topic);
    snprintf(topic, sizeof(topic), "$aws/things/%s/shadow/update/delta", settingsPtr->deviceId); 
    mqttClient.subscribe(topic);

    snprintf(topic, sizeof(topic), "$aws/things/%s/shadow/get", settingsPtr->deviceId);
    mqttClient.publish(topic, "");
}

void AWSConnection::messageHandler(String &topic, String &payload){
    Serial.println("incoming: " + topic + " - " + payload);

    JsonDocument doc;
    deserializeJson(doc, payload);

    if(topic.endsWith("/get/accepted")){
        JsonObject desired = doc["state"]["desired"];

        if(!desired.isNull()){
            Serial.println("desired state:");

            if(desired["measurementInterval"].is<unsigned long>()){
                settingsPtr->measurementInterval = desired["measurementInterval"].as<unsigned long>();
                SettingsStorage::save(*settingsPtr);
            }

            serializeJsonPretty(desired, Serial);
            Serial.println();
        }
    }

    if(topic.endsWith("/update/delta")){

        Serial.println("New desired-state changes:");

        serializeJsonPretty(doc["state"], Serial);

        JsonObject state = doc["state"];

        if(state["measurementInterval"].is<long>()){
            settingsPtr->measurementInterval = state["measurementInterval"].as<long>();
            reportMeasurementInterval(settingsPtr->measurementInterval);
            Serial.print("\r\nNew measurement interval: ");
            Serial.println(settingsPtr->measurementInterval);
        }

        Serial.println();
    }
}

void AWSConnection::reportMeasurementInterval(unsigned long interval){
    char topic[64];
    snprintf(topic, sizeof(topic), "$aws/things/%s/shadow/update", settingsPtr->deviceId);

    String payload = "{\"state\":{\"reported\":{\"measurementInterval\":" + String(interval) + "}}}";
    mqttClient.publish(topic, payload);
}

bool AWSConnection::publishAWS(String payload, const char* topic){
    Serial.print("Publishing to '");
    Serial.print(topic);
    Serial.print("': ");
    Serial.println(payload);
    return mqttClient.publish(topic, payload);
}

bool AWSConnection::publishTelemetry(String payload){
    char topic[64];
    snprintf(topic, sizeof(topic), "%s%s", settingsPtr->deviceId, AWS_IOT_PUBLISH_TOPIC);
    return publishAWS(payload, topic);
}

bool AWSConnection::loop(){
    return mqttClient.loop();
}