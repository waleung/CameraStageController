#include "Controller.h"

Controller controller;

void setup() {
  Serial.begin(115200);
  controller.init();
}

void loop() {
  controller.available();
  if (controller.commandReceived())
  {
    controller.processCommand();
  }
}
