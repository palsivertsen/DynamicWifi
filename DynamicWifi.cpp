#include "DynamicWifi.h"

DynamicWifi::DynamicWifi(char* ssid)
    :server(new WiFiServer(80)) {
  WiFi.softAP(ssid);
  Serial.println("Creating AP with ssid: ");
  Serial.println(ssid);
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  server->begin();
}

DynamicWifi::~DynamicWifi() {
  delete server;
  WiFi.softAPdisconnect(true);
  Serial.println("Wifi off");
}

String httpProtocolNotSupported() {
  String response = String("HTTP/1.1 505");
}

bool DynamicWifi::configure() {
  WiFiClient client = server->available();
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        String verb = client.readStringUntil(' ');
        String path = client.readStringUntil(' ');
        String protocol = client.readStringUntil('\r');
        client.read();
        {
          String header;
          do {
            header = client.readStringUntil('\r');
            client.read();
            // Get headers here
          } while (header.length() > 0);
        }
        String body = client.readString();

        Serial.print("Verb: ");
        Serial.println(verb);
        Serial.print("Path: ");
        Serial.println(path);
        Serial.print("Protocol: ");
        Serial.println(protocol);
        Serial.print("Body: ");
        Serial.println(body);

        if (protocol != "HTTP/1.1") {
          error(client, 505); // HTTP Version Not Supported
          break;
        }

        if (verb.equals("GET")) {
          handleGet(client);
        } else if (verb.equals("POST")) {
          handlePost(client, body);
        } else {
          error(client, 405); // Method not allowed
        }
        break;
      }
    }
    delay(1);
    client.stop();
    Serial.println("\nClient disconnected");
  }
  return false;
}

void DynamicWifi::handleGet(WiFiClient client) {
  String htmlPage =
   String("HTTP/1.1 200 OK\r\n") +
          "Content-Type: text/html\r\n" +
          "Connection: close\r\n" +  // the connection will be closed after completion of the response
          "\r\n" +
          "<!DOCTYPE HTML>" +
          "<html>" +
          "<body>"
          "<form method=\"post\">"
          "SSID:<br>"
          "<input type=\"text\" name=\"ssid\">"
          "<br>"
          "Password:<br>"
          "<input type=\"password\" name=\"password\">"
          "<br>"
          "<input type=\"submit\" value=\"Submit\">"
          "</form>"
          "</body>"
          "</html>" +
          "\r\n";
  client.print(htmlPage);
}

void DynamicWifi::handlePost(WiFiClient client, String body) {
  int i = 0;
  do {
    int indexOfAmp = body.indexOf('&', i);
    String keyVal = "";
    if (indexOfAmp > 0) {
      keyVal = body.substring(i, indexOfAmp);
      i = indexOfAmp + 1;
      Serial.println("PreKey");
    } else {
      keyVal = body.substring(i, body.length());
      Serial.println("PostKey");
      i = body.length();
    }
    Serial.print("KeyVal: ");
    Serial.println(keyVal);
    if (keyVal.startsWith("ssid=")) {
      ssid = keyVal.substring(5, keyVal.length());
    } else if (keyVal.startsWith("password=")) {
      password = keyVal.substring(9, keyVal.length());
    } else {
      Serial.print("Ignoring ");
      Serial.println(keyVal);
    }
  } while(i < body.length());
  client.print("HTTP/1.1 ");
  if (ssid.length() <= 0 || password.length() <= 0) {
    client.print(400);
  } else {
    client.print(200);
  }
  client.print(" \r\n\r\n");
  client.print("Network '");
  client.print(ssid);
  client.print("' saved!");
}

void DynamicWifi::error(WiFiClient client, short status) {
  client.print("HTTP/1.1 ");
  client.print(status);
  client.print(" \r\n\r\n");
}

String DynamicWifi::getSsid() {
  return ssid;
}

String DynamicWifi::getPassword() {
  return password;
}
