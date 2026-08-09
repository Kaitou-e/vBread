#include <Arduino.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "vtuber.hpp"
#include "screen.hpp"
#include <vector>
#include "storage.hpp"

const char *pluginName = "vBread";
const char *pluginDeveloper = "Kaitou e";
unsigned long lastAuthAttempt = 0;
const unsigned long authRetryMs = 5000;

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

void startAuthentication()
{
    if (!webSocket.isConnected())
        return;

    authenticated = false;
    lastAuthAttempt = millis();

    if (authToken.isEmpty())
    {
        Serial.println("No saved token; requesting new authorization token");
        apiStatus = "API: requesting token";
        drawScreen();

        requestAuthToken();
    }
    else
    {
        Serial.println("Using saved token to authenticate");
        apiStatus = "API: authenticating";
        drawScreen();

        sendAuthenticate();
    }
}

void checkVTubeConnection()
{
    if (!webSocket.isConnected())
        return;

    // Connected but no successful authentication response arrived.
    if (!authenticated && millis() - lastAuthAttempt >= authRetryMs)
    {
        Serial.println("Auth timed out; retrying authentication");
        startAuthentication();
    }
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

        authenticated = false;
        requestedToken = false;

        apiStatus = "API: connected";
        drawScreen();

        if (authToken.isEmpty())
        {
            Serial.println("No saved token; requesting a new token");

            requestedToken = true;
            requestAuthToken();
        }
        else
        {
            Serial.println("Saved token found; authenticating with it");

            sendAuthenticate();
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

                Serial.println("Got new auth token");
                saveAuthToken(authToken);

                apiStatus = "API: got token";
                drawScreen();

                sendAuthenticate();
            }
        }
        else if (strcmp(msgType, "AuthenticationResponse") == 0)
        {
            bool authenticatedOk = doc["data"]["authenticated"] | false;
            authenticated = authenticatedOk;

            if (authenticated)
            {
                apiStatus = "API: authed";
                drawScreen();

                Serial.println("VTube Studio authentication successful");
                sendApiStateRequest();
            }
            else
            {
                Serial.println("Saved token rejected; requesting a new one");

                authToken = "";
                deleteAuthToken();

                apiStatus = "API: token rejected";
                drawScreen();

                requestAuthToken();
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
