#include <Arduino.h>
#include <vector>
#include <map>
//////////////////////
#include "vtuber.hpp"
#include "screen.hpp"
#include "wifi.hpp"
#include "storage.hpp"
#include "webpage.h"

const char *ssid = "DUPC 2889";
const char *password = "34K22m/1";

const char *host = "192.168.137.1";
const uint16_t port = 8001;
const char *path = "/";

std::vector<int> buttonPins = {13, 4, 27, 26, 25, 33};
unsigned int max_code = 0, btn_code = 0, last_code = 0;
std::map<int, String> names = {
    {1, "Angry"},
    {2, "Crying"},
    {4, "Dark-face"},
    {8, ""},
    {16, ""},
    {32, ""},
    {3, ""}};

String wifiStatus = "WiFi: starting";
String apiStatus = "API: disconnected";
String pressStatus = "Press: none";

const char *NAMES_FILE = "/names.json";

void setup()
{
  Serial.begin(115200);
  delay(1000);

  for (int i : buttonPins)
    pinMode(i, INPUT_PULLUP);

  initStorage();
  initScreen();
  initWiFi(ssid, password);
  WebPage::init_html_pages();
  initWebSocket();
}

void loop()
{
  webSocket.loop();
  WebPage::server.handleClient();

  for (int i = 0; i < 9; i++)
  {
    unsigned int mask = 1 << i;
    if (digitalRead(buttonPins[i]) == HIGH)
    {
      btn_code |= mask;
    }
    else
    {
      mask = ~mask;
      btn_code &= mask;
    }
  }
  if (max_code < btn_code)
    max_code = btn_code;
  if (btn_code == 0 and last_code > 0)
  {
    auto it = names.find(max_code);
    if (it != names.end())
    {
      triggerHotkey(it->second);
      Serial.println("Button pressed");
      pressStatus = String("Press: ") + it->second.c_str();
      drawScreen();
    }
    max_code = 0;
  }
  last_code = btn_code;
  delay(20);
}