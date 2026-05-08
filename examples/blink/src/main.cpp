/**
 * @file blink/main.cpp
 * @author Aldem Pido
 * @date 5/8/26
 * @brief Basic implementation of threaded subsystem
 */
#include <Arduino.h>

#include "ThreadedSubsystem.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

class BlinkSubsystem : public Subsystem::ThreadedSubsystem {
 public:
  using Subsystem::ThreadedSubsystem::ThreadedSubsystem;

 protected:
  bool initRaw() override {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    return true;
  }

  void updateRaw() override {
    _state = !_state;
    digitalWrite(LED_BUILTIN, _state ? HIGH : LOW);
  }

 private:
  bool _state = false;
};

BlinkSubsystem blink(Subsystem::BaseSetup{"blink", 4096, 1, 1, 500});

void setup() {
  Serial.begin(115200);
  blink.startTask();
}

void loop() { delay(1000); }
