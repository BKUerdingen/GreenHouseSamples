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




void initProperties() {
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(temperature, READ, 1 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity, READ, 1 * SECONDS, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
