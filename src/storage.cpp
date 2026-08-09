#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "storage.hpp"

bool saveNames()
{
    JsonDocument doc;

    for (auto &kv : names)
    {
        doc[String(kv.first)] = kv.second;
    }

    File file = LittleFS.open(NAMES_FILE, "w");
    if (!file)
    {
        Serial.println("Failed to open names file for writing");
        return false;
    }

    if (serializeJsonPretty(doc, file) == 0)
    {
        Serial.println("Failed to write JSON");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Succeed to write JSON");
    return true;
}

bool saveAuthToken(const String &token)
{
    File file = LittleFS.open("/vtube_token.txt", "w");

    if (!file)
    {
        Serial.println("Could not open token file for writing");
        return false;
    }

    file.print(token);
    file.close();

    Serial.println("Saved VTube Studio auth token");
    return true;
}

bool loadAuthToken(String &token)
{
    if (!LittleFS.exists("/vtube_token.txt"))
    {
        Serial.println("No saved VTube Studio token");
        return false;
    }

    File file = LittleFS.open("/vtube_token.txt", "r");

    if (!file)
    {
        Serial.println("Could not open token file for reading");
        return false;
    }

    token = file.readString();
    token.trim();
    file.close();

    if (token.isEmpty())
    {
        Serial.println("Saved VTube Studio token was empty");
        return false;
    }

    Serial.println("Loaded saved VTube Studio auth token");
    return true;
}

bool deleteAuthToken()
{
    if (!LittleFS.exists("/vtube_token.txt"))
        return true;

    bool deleted = LittleFS.remove("/vtube_token.txt");

    Serial.println(deleted
                       ? "Deleted invalid VTube Studio token"
                       : "Could not delete VTube Studio token");

    return deleted;
}

bool loadNames()
{
    if (!LittleFS.exists(NAMES_FILE))
    {
        Serial.println("names.json not found, using defaults");
        return false;
    }

    File file = LittleFS.open(NAMES_FILE, "r");
    if (!file)
    {
        Serial.println("Failed to open names file for reading");
        return false;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(err.c_str());
        return false;
    }

    names.clear();
    for (JsonPair kv : doc.as<JsonObject>())
    {
        int key = atoi(kv.key().c_str());
        names[key] = kv.value().as<String>();
    }

    Serial.println("Succeed to read JSON");
    return true;
}

bool initStorage()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed");
        return false;
    }

    Serial.println("LittleFS mounted");
    loadNames();
    return true;
}