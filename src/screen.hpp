#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Arduino.h>
#include <vector>

extern String wifiStatus;
extern String apiStatus;
extern String pressStatus;
extern std::vector<int> buttonPins;

void initScreen();
void drawScreen();

#endif