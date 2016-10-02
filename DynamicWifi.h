#ifndef DynamicWifi_h
#define DynamicWifi_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

// #define dynamic_wifi_debug

#ifdef dynamic_wifi_debug
  #define debug_print(...)    Serial.print(__VA_ARGS__)
  #define debug_println(...)  Serial.println(__VA_ARGS__)
  #define debug_write(...)    Serial.write(__VA_ARGS__)
#else
  #define debug_print(...)
  #define debug_println(...)
  #define debug_write(...)
#endif

class DynamicWifi {
public:
  DynamicWifi(char*);
  ~DynamicWifi();
  bool tryConfigure();
  String getSsid();
  String getPassword();
private:
  void handleGet(WiFiClient);
  void handlePost(WiFiClient, String);
  void status(WiFiClient, short);
  String ssid;
  String password;
  WiFiServer* server;
};

#endif
