#ifndef VTUBER_HPP
#define VTUBER_HPP

#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <vector>

extern WebSocketsClient webSocket;

extern const char *pluginName;
extern const char *pluginDeveloper;

extern String authToken;
extern bool requestedToken;
extern bool authenticated;

extern const char *host;    // = "192.168.137.1";
extern const uint16_t port; // = 8001;
extern const char *path;    // = "/";

extern String apiStatus;

void sendJson(const JsonDocument &doc);
void requestAuthToken();
void checkVTubeConnection();
void sendAuthenticate();
void sendApiStateRequest();
void triggerHotkey(String name);
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void initWebSocket();

#endif
