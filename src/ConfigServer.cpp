#include "ConfigServer.h"
#include "SettingsStorage.h"

WebServer ConfigServer::server;
DeviceSettings* ConfigServer::settingsPtr = nullptr;

void ConfigServer::start(DeviceSettings &settings){
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    WiFi.softAP("ESP32-Setup");

    Serial.println("Setup AP started");
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, ConfigServer::handleRoot);
    server.on("/save", HTTP_POST, ConfigServer::handleSave);

    server.begin();

    while(true){
        server.handleClient();
        delay(1);
    }
}

void ConfigServer::handleRoot(){
    String html =
            "<html>"
            "<body>"
            "<h2>WiFi Setup</h2>"
            "<form action='/save' method='POST'>"
            "SSID:<br>"
            "<input name='ssid'><br><br>"
            "Password:<br>"
            "<input name='password' type='password'><br><br>"
            "<input type='submit' value='Save'>"
            "</form>"
            "</body>"
            "</html>";

    server.send(200, "text/html", html);
}

void ConfigServer::handleSave(){
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    strncpy(settingsPtr->wifiSSID, ssid.c_str(), sizeof(ssid));
    strncpy(settingsPtr->wifiPassword, password.c_str(), sizeof(password));
    
    SettingsStorage::save(*settingsPtr);

    server.send(200,
            "text/html",
            "<h2>Saved. Rebooting...</h2>");

    delay(2000);

    ESP.restart();
}