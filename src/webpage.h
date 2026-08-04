#pragma once

#include <queue>
#include <map>
#include <ArduinoJson.h>
#include <WebServer.h> // Include for the WebServer type

extern std::map<int, String> names;

namespace WebPage
{

    // Size of the XML buffer
    constexpr size_t XmlBufferSize = 2048;

    // Declare the XML buffer (optional: extern if you want to access it outside)
    extern char XML[XmlBufferSize];

    extern int webSubmittedValue;

    void update_XML(int val);
    void newKeyInput(int val);

    // Declare the WebServer instance accessible externally (optional)
    extern WebServer server;

    // Function to send the main HTML page
    void SendHtml();

    // Initialize HTTP server and handlers
    void init_html_pages();

}
