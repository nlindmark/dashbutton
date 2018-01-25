// NN
#define DEBUG_ESP_HTTP_UPDATE
#define DEBUG_ESP_PORT Serial

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


#define update_server "192.168.1.100"
#define update_uri "/esp/update/espupdater.php"

// Forward declations
void printMacAddress();


void iotUpdater(bool debug) {
  if (debug) {
    printMacAddress();
    Serial.println("start flashing......");
    Serial.println(update_server);
    Serial.println(update_uri);

  }

  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      if (debug) Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      if (debug) Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      if (debug) Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void printMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  Serial.println(mac[5], HEX);
}
