#include <Arduino.h>
#include <WiFi.h>
#include "wifi.hpp"
#include "screen.hpp"

void initWiFi(const char *ssid, const char *password)
{
    wifiStatus = "WiFi: connecting";
    drawScreen();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    wifiStatus = "WiFi: connected";
    apiStatus = "API: connecting";
    pressStatus = "Press: none";
    drawScreen();
}