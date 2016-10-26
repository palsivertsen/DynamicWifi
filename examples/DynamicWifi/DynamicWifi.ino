#include <DynamicWifi.h>

void setup() {
  // Setup
  Serial.begin(115200);
  Serial.println("Booting...");

  char ssid[15] = {'\0'};
  sprintf(ssid, "ESP8266_%06X", ESP.getChipId());

  Serial.print("Setting up AP with SSID: ");
  Serial.println(ssid);
  // Initiate config object
  DynamicWifi config(ssid);

  unsigned long configStart = millis();
  // Try config until success or timeout
  while(!config.tryConfigure()) {
    if (millis() - configStart > (5 * 60 * 1000)) { // Abort after +-5 minutes
      Serial.println("Config took too long!");
      return;
    }
  }
  Serial.printf("Connected to WiFi '%s'\n", WiFi.SSID().c_str());
}

void loop() {
}
