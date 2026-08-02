#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_WIDTH 128 // OLED display width, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "DUPC 2889";
const char* password = "34K22m/1";

const char* host = "192.168.137.1";
const uint16_t port = 8001;
const char* path = "/";

const char* pluginName = "vBread";
const char* pluginDeveloper = "Kaitou e";

WebSocketsClient webSocket;
String authToken = "";
bool requestedToken = false;
bool authenticated = false;
int id_num_hotkey = 0;

// BUTTONS
const int angryButtonPin = 4;
bool lastAngryButtonState = HIGH;
const int sadButtonPin = 13;
bool lastSadButtonState = HIGH;
const int darkButtonPin = 27;
bool lastDarkButtonState = HIGH;

String wifiStatus = "WiFi: starting";
String apiStatus = "API: disconnected";
String pressStatus = "Press: none";

void drawScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

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

void sendJson(const JsonDocument& doc) {
  String msg;
  serializeJson(doc, msg);
  webSocket.sendTXT(msg);
  Serial.println("Sent:");
  Serial.println(msg);
}

void requestAuthToken() {
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

void sendAuthenticate() {
  if (authToken.isEmpty()) return;

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

void sendApiStateRequest() {
  JsonDocument doc;
  doc["apiName"] = "VTubeStudioPublicAPI";
  doc["apiVersion"] = "1.0";
  doc["requestID"] = "state-001";
  doc["messageType"] = "APIStateRequest";
  sendJson(doc);
}

void triggerHotkey(String name){
  DynamicJsonDocument doc(512);
  doc["apiName"] = "VTubeStudioPublicAPI";
  doc["apiVersion"] = "1.0";
  doc["requestID"] = String(millis());
  doc["messageType"] = "HotkeyTriggerRequest";

  JsonObject data = doc.createNestedObject("data");
  data["hotkeyID"] = name;   // can be hotkey name or hotkey ID

  String msg;
  serializeJson(doc, msg);
  webSocket.sendTXT(msg);

  Serial.println("Sent hotkey trigger:");
  Serial.println(msg);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket connected");
    // requestedToken = false;
    // authenticated = false;
    // requestAuthToken();
    // requestedToken = true;
    apiStatus = "API: connected";
    drawScreen();
    if (!authenticated && !requestedToken){
      requestAuthToken();
      requestedToken = true;
    }
    return;
  }

  if (type == WStype_TEXT) {
    String text;
    text.reserve(length);
    for (size_t i = 0; i < length; i++) text += (char)payload[i];

    Serial.println("Received:");
    Serial.println(text);

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, text);
    if (err) {
      Serial.println("JSON parse failed");
      return;
    }

    const char* msgType = doc["messageType"] | "";
    if (strcmp(msgType, "AuthenticationTokenResponse") == 0) {
      if (doc["data"]["authenticationToken"].is<const char*>()) {
        authToken = doc["data"]["authenticationToken"].as<const char*>();
        Serial.println("Got auth token");
        apiStatus = "API: got token";
        drawScreen();
        sendAuthenticate();
      }
    } else if (strcmp(msgType, "AuthenticationResponse") == 0) {
      bool authenticatedOk = doc["data"]["authenticated"] | false;
      authenticated = authenticatedOk;
      apiStatus = authenticated ? "API: authed" : "API: auth failed";
      drawScreen();
      Serial.print("Authenticated: ");
      Serial.println(authenticated ? "true" : "false");
      if (authenticated) {
        sendApiStateRequest();
      }
    } else if (strcmp(msgType, "APIStateResponse") == 0) {
      bool active = doc["data"]["active"] | false;
      bool sessionAuth = doc["data"]["currentSessionAuthenticated"] | false;
      apiStatus = String("API: ") + (active ? "active" : "inactive");
      if (sessionAuth) apiStatus += " authed";
      drawScreen();
    }
  }

  if (type == WStype_DISCONNECTED) {
    Serial.println("WebSocket disconnected");
    apiStatus = "API: disconnected";
    drawScreen();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(angryButtonPin, INPUT_PULLUP);
  pinMode(sadButtonPin, INPUT_PULLUP);
  pinMode(darkButtonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  drawScreen();

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
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

  webSocket.begin(host, port, path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();

  bool currentStateAngry = digitalRead(angryButtonPin);
  bool currentStateSad = digitalRead(sadButtonPin);
  bool currentStateDark = digitalRead(darkButtonPin);

  if (lastAngryButtonState == HIGH && currentStateAngry == LOW) {
    triggerHotkey("Angry");
    Serial.println("Button pressed");
    pressStatus = "Press: Angry";
    drawScreen();
    delay(200);
  } if (lastSadButtonState == HIGH && currentStateSad == LOW) {
    triggerHotkey("Crying");
    Serial.println("Button pressed");
    pressStatus = "Press: Crying";
    drawScreen();
    delay(200);
  } if (lastDarkButtonState == HIGH && currentStateDark == LOW) {
    triggerHotkey("Dark-face");
    Serial.println("Button pressed");
    pressStatus = "Press: Dark face";
    drawScreen();
    delay(200);
  }

  lastAngryButtonState = currentStateAngry;
  lastSadButtonState = currentStateSad;
  lastDarkButtonState = currentStateDark;
}