#include "Dashbutton.h"


// FORWARD DECLARATIONS
void eventListener();

// VARIABLES
Dashbutton *dashbutton;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  dashbutton = Dashbutton::getInstance();
  dashbutton->setCallback(eventListener);

}

  void loop() {
    // put your main code here, to run repeatedly:
    dashbutton->update();
    yield();
  }

  // CALLBACKS
  //

  void eventListener() {
    // Something wonderful has happened
  }
