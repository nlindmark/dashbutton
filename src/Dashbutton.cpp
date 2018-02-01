
#include "Dashbutton.h"
#include <Arduino.h>
#include <Ticker.h>
#include "iotUpdater.h"

#define CODE_VERSION "V1.0.3"

char ssid[] = SSIDNAME;
char password[] = PASSWORD;
bool setup_wifi();

/* Null, because instance will be initialized on demand. */
Dashbutton *Dashbutton::instance = 0;

Dashbutton *Dashbutton::getInstance() {
  if (instance == 0) {
    instance = new Dashbutton();

    instance->call = NULL;

    pinMode(LED, OUTPUT);
    digitalWrite(LED, instance->lastLedState);
    instance->pBlinkTimer->start();
    instance->pTimeout->start();

    setup_wifi();
  }

  return instance;
}

void Dashbutton::setCallback(fptr callback) { call = callback; }

// static wrapper-function to be able to callback the member function
void Dashbutton::Wrapper_To_Call_blink() {
  // call member
  instance->blink();
}

void Dashbutton::Wrapper_To_Call_gotoSleep() {
  // call member
  instance->gotoSleep();
}

void Dashbutton::Wrapper_To_Call_mqttCallback(char *topic, byte *payload,
                                              unsigned int length) {
  // call member
  instance->mqttCallback(topic, payload, length);
}

void Dashbutton::update() {

  pBlinkTimer->update();
  pTimeout->update();

  if (!mqttClient.connected()) {
    mqttReconnect();
  } else {
    // Client connected
    mqttClient.loop();
  }
}

// PRIVATE
Dashbutton::Dashbutton()
    : mqttServer(192, 168, 1, 200),
      mqttClient(mqttServer, 1883, Wrapper_To_Call_mqttCallback, espClient) {}

void Dashbutton::blink() {
  instance->lastLedState = !instance->lastLedState;
  digitalWrite(LED, instance->lastLedState);
}

boolean Dashbutton::mqttReconnect() {

  char clientName[50];
  long now = millis();

  if (now - lastMqttReconnectAttempt > 3000) {
    lastMqttReconnectAttempt = now;
    // Attempt to reconnect

    snprintf(clientName, 50, "%ld", system_get_chip_id());
    if (mqttClient.connect(clientName, MQTTUSER, MQTTPASSWORD)) {
      char str[50];
      strcpy(str, "The Dashbutton is connected ");
      strcat(str, CODE_VERSION);
      mqttClient.publish("/home/dashbutton/hello", str);
      mqttClient.publish("/home/dashbutton/pushed", "pushed");
      mqttClient.subscribe("/home/dashbutton/status", 1);
      mqttClient.subscribe("/home/dashbutton/reprogram", 1);
    }
  }
  return mqttClient.connected();
}

void Dashbutton::gotoSleep() {
  // OK, ready. Go to sleep
  digitalWrite(LED, HIGH);
  ESP.deepSleep(0);
}

void Dashbutton::mqttCallback(char *topic, byte *payload, unsigned int length) {
  // handle message arrived
  // Internally, the mqtt client uses the same buffer for both inbound and
  // outbound
  // messages. The application should create its own copy of the values if they
  // are required beyond this.

  char t[50], p[50];
  snprintf(t, 50, "%s", topic);
  snprintf(p, length + 1, "%s", payload);
  Serial.println(t);
  Serial.println(p);

  if (strcmp(t, "/home/dashbutton/status") == 0) {
    if (strcmp(p, "Acknowledge") == 0) {
      gotoSleep();
    }
  } else if (strcmp(t, "/home/dashbutton/reprogram") == 0) {
    iotUpdater(true);
    Serial.println("Received a reprogram command");
  }
}

bool setup_wifi() {

  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSIDNAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  return false;
}
