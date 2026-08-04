#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Arduino.h>

extern String wifiStatus;
extern String apiStatus;
extern String pressStatus;

void initScreen();
void drawScreen();

#endif