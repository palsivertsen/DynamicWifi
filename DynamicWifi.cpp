#include "DynamicWifi.h"

DynamicWifi::DynamicWifi(char* ssid)
    :server(new WiFiServer(80)) {
  WiFi.disconnect(true);
  WiFi.softAP(ssid);
  debug_dw_printf("Created network '%s'\n", ssid);
  debug_dw_print("IP: ");
  debug_dw_println(WiFi.softAPIP());
  server->begin();
  debug_dw_println("Server ready");
}

DynamicWifi::~DynamicWifi() {
  delete server;
  WiFi.softAPdisconnect(true);
  debug_dw_println("Wifi off");
}

bool DynamicWifi::tryConfigure() {
  WiFiClient client = server->available();
  if (client) {
    debug_dw_println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        String verb = client.readStringUntil(' ');
        String path = client.readStringUntil(' ');
        String protocol = client.readStringUntil('\r');
        client.read();
        {
          String header;
          debug_dw_println("Headers:");
          do {
            header = client.readStringUntil('\r');
            client.read();
            debug_dw_println(header);
          } while (header.length() > 0);
        }
        String body = client.readString();

        debug_dw_print("Verb: ");
        debug_dw_println(verb);
        debug_dw_print("Path: ");
        debug_dw_println(path);
        debug_dw_print("Protocol: ");
        debug_dw_println(protocol);
        debug_dw_print("Body: ");
        debug_dw_println(body);

        if (protocol != "HTTP/1.1") {
          status(client, 505); // HTTP Version Not Supported
          break;
        }

        // Only accept requests on root path
        if (path != "/") {
          status(client, 404);
          debug_dw_print("Path '");
          debug_dw_print(path);
          debug_dw_println("' not allowed'");
          break;
        }

        if (verb.equals("GET")) {
          handleGet(client);
        } else if (verb.equals("POST")) {
          handlePost(client, body);
        } else {
          status(client, 405); // Method not allowed
        }
        break;
      }
    }
    client.flush();
    client.stop();
    debug_dw_println("\nClient disconnected");
  }
  return WiFi.status() == WL_CONNECTED;
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
      debug_dw_println("PreKey");
    } else {
      keyVal = body.substring(i, body.length());
      debug_dw_println("PostKey");
      i = body.length();
    }
    debug_dw_print("KeyVal: ");
    debug_dw_println(keyVal);
    if (keyVal.startsWith("ssid=")) {
      ssid = keyVal.substring(5, keyVal.length());
    } else if (keyVal.startsWith("password=")) {
      password = keyVal.substring(9, keyVal.length());
    } else {
      debug_dw_print("Ignoring ");
      debug_dw_println(keyVal);
    }
  } while(i < body.length());
  decodeFields();
  WiFi.begin(ssid.c_str(), password.c_str());
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    status(client, 200);
    client.print("Connected to '");
    client.print(ssid);
    client.write('\'');
  } else {
    status(client, 400);
    client.print("Could not connect to '");
  }
}

void DynamicWifi::status(WiFiClient client, short status) {
  client.print("HTTP/1.1 ");
  client.print(status);
  client.print(" \r\n\r\n");
}

void DynamicWifi::decodeFields() {
  ssid.replace('+', ' ');
  password.replace('+', ' ');
}
