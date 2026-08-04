#include <Arduino.h>
#include <queue>
#include <map>
#include <ArduinoJson.h>
#include <WebServer.h>
#include "webpage.h"
#include "index_src.h"
#include "storage.hpp"

namespace WebPage
{
    // constexpr size_t XmlBufferSize = 2048;
    char XML[XmlBufferSize];
    int webSubmittedValue = -1;
    std::queue<int> key_buffer;

    WebServer server(80);

    void update_XML(int val)
    {
        XML[0] = 0;
        snprintf(XML, XmlBufferSize,
                 "<data>"
                 "<v>%d</v>"
                 "</data>",
                 val);
    }

    void newKeyInput(int val)
    {
        key_buffer.push(val);
    }

    void SendHtml()
    {
        server.send(200, "text/html", (const char *)PAGE_MAIN);
    }

    void SendXML()
    {
        if (key_buffer.empty())
            update_XML(0);
        else
        {
            update_XML(key_buffer.front());
            key_buffer.pop();
        }
        // Serial.println("Sending updated XML");
        server.send(200, "text/xml", XML);
    }

    void SendNames()
    {
        Serial.println("SendNames() called");

        JsonDocument doc;

        Serial.print("names size: ");
        Serial.println(names.size());

        for (auto &kv : names)
        {
            Serial.print("  ");
            Serial.print(kv.first);
            Serial.print(" => ");
            Serial.println(kv.second);

            doc[String(kv.first)] = kv.second;
        }

        String out;
        serializeJsonPretty(doc, out);

        Serial.print("JSON output size: ");
        Serial.println(out.length());
        Serial.println("JSON output:");
        Serial.println(out);

        if (out.isEmpty())
        {
            Serial.println("Warning: JSON output is empty");
            server.send(500, "text/plain", "empty json");
            return;
        }

        server.send(200, "application/json", out);
        Serial.println("Sent JSON to client");
    }

    void getSubmit()
    {
        if (server.hasArg("VALUE"))
        {
            String value = server.arg("VALUE");
            Serial.print("Received VALUE: ");
            Serial.println(value);

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, value);

            if (err)
            {
                Serial.print("JSON parse failed: ");
                Serial.println(err.c_str());
                server.send(400, "text/plain", "Invalid JSON");
                return;
            }

            if (!doc.is<JsonObject>())
            {
                server.send(400, "text/plain", "JSON must be an object");
                return;
            }

            names.clear();
            for (JsonPair kv : doc.as<JsonObject>())
            {
                int key = atoi(kv.key().c_str());
                names[key] = kv.value().as<String>();
            }

            Serial.println("Updated names map:");
            for (auto &kv : names)
            {
                Serial.print(kv.first);
                Serial.print(" => ");
                Serial.println(kv.second);
            }
            if (saveNames())
                Serial.println("Saved to littleFS.");

            server.send(200, "text/plain", "OK");
        }
        else
        {
            Serial.println("No VALUE argument received");
            server.send(400, "text/plain", "Missing VALUE");
        }
    }

    void init_html_pages()
    {
        server.on("/", SendHtml);
        server.on("/xml", SendXML);
        server.on("/getnames", HTTP_GET, SendNames);
        server.on("/submit", HTTP_PUT, getSubmit);

        Serial.println("Starting HTTP server...");
        server.begin();
        Serial.println("HTTP server started (http://localhost:8180)");
    }
}