#pragma once
// Create matching variables in your Thing. See README.md.
// In the Cloud Editor, this file is generated from the Thing configuration.
#include "arduino_secrets.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char THING_ID[] = ""; // Enter your own Thing ID for local Cloud use.
const char SSID[] = SECRET_SSID;
const char PASS[] = SECRET_OPTIONAL_PASS;

float temperature;
float humidity;
int eCO2;
int tvoc;
int soilRaw;
int lightLevel;
float pressure;
float gasResistance;
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
  ArduinoCloud.addProperty(eCO2, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(tvoc, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(soilRaw, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(lightLevel, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(pressure, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(gasResistance, READ, 1 * SECONDS, NULL);
#if ENABLE_SECOND_TH
  ArduinoCloud.addProperty(temperature2, READ, 3 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity2, READ, 3 * SECONDS, NULL);
#endif
  ArduinoCloud.addProperty(light, READWRITE, ON_CHANGE, onLightChange);
  ArduinoCloud.addProperty(pump, READWRITE, ON_CHANGE, onPumpChange);
  ArduinoCloud.addProperty(fan, READWRITE, ON_CHANGE, onFanChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
