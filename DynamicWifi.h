#ifndef DynamicWifi_h
#define DynamicWifi_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

// #define debug_dynamic_wifi

#ifdef debug_dynamic_wifi
  #define debug_dw_print(...)    Serial.print(__VA_ARGS__)
  #define debug_dw_println(...)  Serial.println(__VA_ARGS__)
  #define debug_dw_write(...)    Serial.write(__VA_ARGS__)
  #define debug_dw_printf(...)    Serial.printf(__VA_ARGS__)
#else
  #define debug_dw_print(...)
  #define debug_dw_println(...)
  #define debug_dw_write(...)
  #define debug_dw_printf(...)
#endif

class DynamicWifi {
public:
  DynamicWifi(char*);
  ~DynamicWifi();
  bool tryConfigure();
private:
  void handleGet(WiFiClient);
  void handlePost(WiFiClient, String);
  void status(WiFiClient, short);
  String ssid;
  String password;
  WiFiServer* server;
};

#endif
