#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "vtuber.hpp"
#include "screen.hpp"
#include <vector>

const char *pluginName = "vBread";
const char *pluginDeveloper = "Kaitou e";

WebSocketsClient webSocket;
String authToken = "";
bool requestedToken = false;
bool authenticated = false;
int id_num_hotkey = 0;

void initWebSocket()
{
    webSocket.begin(host, port, path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void sendJson(const JsonDocument &doc)
{
    String msg;
    serializeJson(doc, msg);
    webSocket.sendTXT(msg);
    Serial.println("Sent:");
    Serial.println(msg);
}

void requestAuthToken()
{
    JsonDocument doc;
    doc["apiName"] = "VTubeStudioPublicAPI";
    doc["apiVersion"] = "1.0";
    doc["requestID"] = "auth-token-001";
    doc["messageType"] = "AuthenticationTokenRequest";
    JsonObject data = doc.createNestedObject("data");
    data["pluginName"] = pluginName;
    data["pluginDeveloper"] = pluginDeveloper;
    sendJson(doc);
}

void sendAuthenticate()
{
    if (authToken.isEmpty())
        return;

    JsonDocument doc;
    doc["apiName"] = "VTubeStudioPublicAPI";
    doc["apiVersion"] = "1.0";
    doc["requestID"] = "auth-001";
    doc["messageType"] = "AuthenticationRequest";
    JsonObject data = doc.createNestedObject("data");
    data["pluginName"] = pluginName;
    data["pluginDeveloper"] = pluginDeveloper;
    data["authenticationToken"] = authToken;
    sendJson(doc);
}

void sendApiStateRequest()
{
    JsonDocument doc;
    doc["apiName"] = "VTubeStudioPublicAPI";
    doc["apiVersion"] = "1.0";
    doc["requestID"] = "state-001";
    doc["messageType"] = "APIStateRequest";
    sendJson(doc);
}

void triggerHotkey(String name)
{
    std::vector<int> ind = {-1};
    for (int i = 0; i < name.length(); i++)
    {
        if (name[i] == ',')
            ind.push_back(i);
    }
    if (ind.size() == 1)
    {
        DynamicJsonDocument doc(512);
        doc["apiName"] = "VTubeStudioPublicAPI";
        doc["apiVersion"] = "1.0";
        doc["requestID"] = String(millis());
        doc["messageType"] = "HotkeyTriggerRequest";

        JsonObject data = doc.createNestedObject("data");
        data["hotkeyID"] = name; // can be hotkey name or hotkey ID

        String msg;
        serializeJson(doc, msg);
        webSocket.sendTXT(msg);

        Serial.println("Sent hotkey trigger:");
        Serial.println(msg);
    }
    else
    {
        for (int i = 1; i < ind.size(); i++)
        {
            String tname = name.substring(ind[i - 1] + 1, ind[i]);
            DynamicJsonDocument doc(512);
            doc["apiName"] = "VTubeStudioPublicAPI";
            doc["apiVersion"] = "1.0";
            doc["requestID"] = String(millis());
            doc["messageType"] = "HotkeyTriggerRequest";

            JsonObject data = doc.createNestedObject("data");
            data["hotkeyID"] = tname; // can be hotkey name or hotkey ID

            String msg;
            serializeJson(doc, msg);
            webSocket.sendTXT(msg);

            Serial.println("Sent hotkey trigger:");
            Serial.println(msg);
        }
    }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_CONNECTED)
    {
        Serial.println("WebSocket connected");
        // requestedToken = false;
        // authenticated = false;
        // requestAuthToken();
        // requestedToken = true;
        apiStatus = "API: connected";
        drawScreen();
        if (!authenticated && !requestedToken)
        {
            requestAuthToken();
            requestedToken = true;
        }
        return;
    }

    if (type == WStype_TEXT)
    {
        String text;
        text.reserve(length);
        for (size_t i = 0; i < length; i++)
            text += (char)payload[i];

        Serial.println("Received:");
        Serial.println(text);

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, text);
        if (err)
        {
            Serial.println("JSON parse failed");
            return;
        }

        const char *msgType = doc["messageType"] | "";
        if (strcmp(msgType, "AuthenticationTokenResponse") == 0)
        {
            if (doc["data"]["authenticationToken"].is<const char *>())
            {
                authToken = doc["data"]["authenticationToken"].as<const char *>();
                Serial.println("Got auth token");
                apiStatus = "API: got token";
                drawScreen();
                sendAuthenticate();
            }
        }
        else if (strcmp(msgType, "AuthenticationResponse") == 0)
        {
            bool authenticatedOk = doc["data"]["authenticated"] | false;
            authenticated = authenticatedOk;
            apiStatus = authenticated ? "API: authed" : "API: auth failed";
            drawScreen();
            Serial.print("Authenticated: ");
            Serial.println(authenticated ? "true" : "false");
            if (authenticated)
            {
                sendApiStateRequest();
            }
        }
        else if (strcmp(msgType, "APIStateResponse") == 0)
        {
            bool active = doc["data"]["active"] | false;
            bool sessionAuth = doc["data"]["currentSessionAuthenticated"] | false;
            apiStatus = String("API: ") + (active ? "active" : "inactive");
            if (sessionAuth)
                apiStatus += " authed";
            drawScreen();
        }
    }

    if (type == WStype_DISCONNECTED)
    {
        Serial.println("WebSocket disconnected");
        apiStatus = "API: disconnected";
        drawScreen();
    }
}
