#include "Arduino.h"
#include <ESP8266WiFi.h>

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
