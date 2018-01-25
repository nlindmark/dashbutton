#ifndef DASHBUTTON_H
#define DASHBUTTON_H

#include "credentials.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
extern "C" {
#include "user_interface.h"
}



#define LED     2
//#define LED 5
typedef void (*fptr)();


class Dashbutton {

public:
  /**
   * Get the Dashbutton instance
   * @return Pointer to Dashbutton instance
   */
  static Dashbutton *getInstance();

  /**
   * Called periodically to update the timers
   */
  void update();

  /**
   * Set a callback function.
   * The callback will be called when a new door event is detected
   * @param callback [description]
   */
  void setCallback(fptr callback);

private:
  /* Private constructor to prevent instancing. */
  Dashbutton();
  void blink();


  /**
   * Static wrapper functions used in callbacks
   * http://www.newty.de/fpt/callback.html#static
   */
  static void Wrapper_To_Call_blink();
  static void Wrapper_To_Call_gotoSleep();
  static void Wrapper_To_Call_mqttCallback(char *topic, byte *payload,
                                           unsigned int length);


  void mqttCallback(char *topic, byte *payload, unsigned int length);
  boolean mqttReconnect();
  void gotoSleep();

  // Variables
  /* Here will be the instance stored. */
  static Dashbutton *instance;
  Ticker *pBlinkTimer = new Ticker(Wrapper_To_Call_blink, 500, 0, MILLIS);
  Ticker *pTimeout = new Ticker(Wrapper_To_Call_gotoSleep, 10000, 1, MILLIS);
  boolean lastLedState = true;
  fptr call;
  IPAddress mqttServer;
  PubSubClient mqttClient;

  WiFiClient espClient;
  long lastMqttReconnectAttempt = 0;
};

#endif
