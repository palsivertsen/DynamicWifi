#include <DynamicWifi.h>

void generateUniqueSsid(char*);

void setup() {
  // Setup
  Serial.begin(115200);
  Serial.println("Booting...");

  char uniqueSsid[15] = {'\0'};
  generateUniqueSsid(uniqueSsid);

  Serial.print("Setting up AP with SSID: ");
  Serial.println(uniqueSsid);
  // Initiate config object
  DynamicWifi config(uniqueSsid);

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

// Generates SSID nased on mac address
void generateUniqueSsid(char* result) {
  uint8_t macAddr[6];
  WiFi.softAPmacAddress(macAddr);
  String apSsid = String("ESP8266_");
  apSsid.concat(String(macAddr[3], HEX));
  apSsid.concat(String(macAddr[4], HEX));
  apSsid.concat(String(macAddr[5], HEX));
  apSsid.toUpperCase();
  apSsid.toCharArray(result, 15);
}
