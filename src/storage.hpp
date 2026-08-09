#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <map>

extern std::map<int, String> names;
extern const char *NAMES_FILE;

bool saveNames();
bool loadNames();
bool initStorage();
bool saveAuthToken(const String &token);
bool loadAuthToken(String &token);
bool deleteAuthToken();

#endif