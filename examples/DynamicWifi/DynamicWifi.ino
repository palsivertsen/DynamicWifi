#include "DynamicWifi.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Boot boot!");
  DynamicWifi config("My Config network name");
  while(!config.configure()) {
    delay(100);
  }
  Serial.print("SSID: ");
  Serial.println(config.getSsid());
  Serial.print("Password: ");
  Serial.println(config.getPassword());
}

void loop() {
}
