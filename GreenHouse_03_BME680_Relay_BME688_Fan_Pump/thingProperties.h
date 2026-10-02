#pragma once
// Create matching variables in your Thing. See README.md.
// In the Cloud Editor, this file is generated from the Thing configuration.
#include "arduino_secrets.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char THING_ID[] = "83577944-d6e7-4b36-b70a-dd68e83aba04"; // Enter your own Thing ID for local Cloud use.
const char SSID[] = SECRET_SSID;
const char PASS[] = SECRET_OPTIONAL_PASS;

float temperature;
float humidity;
bool light;
bool pump;
bool fan;
#if ENABLE_SECOND_TH
float temperature2;
float humidity2;
#endif

void onLightChange();
void onPumpChange();
void onFanChange();

void initProperties() {
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity, READ, 1 * SECONDS, NULL);
#if ENABLE_SECOND_TH
  ArduinoCloud.addProperty(temperature2, READ, 3 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity2, READ, 3 * SECONDS, NULL);
#endif
  ArduinoCloud.addProperty(light, READWRITE, ON_CHANGE, onLightChange);
  ArduinoCloud.addProperty(pump, READWRITE, ON_CHANGE, onPumpChange);
  ArduinoCloud.addProperty(fan, READWRITE, ON_CHANGE, onFanChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
