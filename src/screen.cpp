#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "screen.hpp"
#include <vector>

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initScreen()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    display.display();
}

// std::vector<int> buttonPinstest = {6, 7, 10, 4, 2, 20, 0, 1, 3};
void drawScreen()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // for (int i = 0; i < 9; i++)
    // {
    //     int pinState = digitalRead(buttonPins[i]);
    //     display.setCursor(i * 5, 0);
    //     display.print(pinState);
    // }
    display.setCursor(0, 0);
    display.println(wifiStatus);

    display.setCursor(0, 16);
    display.println(apiStatus);

    display.setCursor(0, 32);
    display.println(pressStatus);

    display.setCursor(0, 48);
    display.print("IP: ");
    display.println(WiFi.localIP());

    display.display();
}