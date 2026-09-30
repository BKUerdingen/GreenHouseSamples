# Greenhouse Samples

A five-step learning sequence for the **Arduino Greenhouse Kit**, using an **MKR WiFi 1010** and, from lesson 2, the **MKR IoT Carrier Rev2**. Start with one sensor and gradually add manual actuator control and further measurements.

Each lesson is a standalone Arduino project with its own sketch, Cloud configuration, and README. Comments, serial output, and documentation are in English. There are no automatic greenhouse control rules: sensor readings never switch actuators.

## Learning sequence

Each lesson builds on the previous one. Follow the links for wiring, library installation, serial commands, and Cloud setup.

| Step | Project | What you learn |
| --- | --- | --- |
| 1 | [Temperature and humidity](GreenHouse_01_BME680_Temperature_Humidity/README.md) | Read temperature and relative humidity from one external BME680 and display both values in the Serial Monitor or Arduino Cloud. |
| 2 | [Light relay and optional second T/H sensor](GreenHouse_02_BME680_Relay_OptionalBME688/README.md) | Add manual lighting control through a relay. Optionally compare the external sensor with the Carrier's onboard BME688. The light sensor is not used yet. |
| 3 | [Fan and pump](GreenHouse_03_BME680_Relay_BME688_Fan_Pump/README.md) | Add manual fan and water pump control through the Grove I2C Motor Driver. |
| 4 | [Gas and soil moisture](GreenHouse_04_BME680_Relay_BME688_Fan_Pump_SGP30_Soil/README.md) | Add SGP30 eCO2/TVOC readings and raw analog soil moisture measurements. |
| 5 | [Complete hardware test](GreenHouse_05_BME680_Relay_BME688_Fan_Pump_SGP30_Soil_Light/README.md) | Add the onboard light sensor, external BME680 pressure and gas resistance, I2C scanning, and additional serial diagnostics. This is the final project in the learning sequence. |

The optional second temperature/humidity sensor is available in **steps 2-5**. Set `ENABLE_SECOND_TH` to `1` to enable it; its two additional Cloud variables are only registered when enabled.

## Getting started

1. Choose a lesson and open the `.ino` file whose name matches its project folder.
2. Select **Arduino MKR WiFi 1010** and install the board package and libraries listed in that project's README.
3. Check the hardware constants at the top of the sketch. Start with Cloud disabled and all actuators off.
4. Upload and open the Serial Monitor at **9600 baud**.
5. Once the local hardware test works, create the required Arduino Cloud Device, Thing, variables, and Dashboard, then enable `CLOUD_ENABLED`.

Every project includes Cloud connection code and only the variables needed for its scope. Cloud is disabled by default. The individual READMEs specify exact variable names, types, permissions, and callbacks.

## Connecting a sketch to Arduino Cloud

Entering a Thing ID is only one part of the setup. The board must already be registered as a Cloud Device, associated with that Thing, and configured to join your Wi-Fi network. The Thing must also contain the variables expected by the selected lesson. The sketch does not create these Cloud objects automatically.

### Prepare the Device and Thing

1. Register your **Arduino MKR WiFi 1010** as a Device in Arduino Cloud using the device setup wizard.
2. Create a Thing and associate the registered board with it.
3. Create the variables listed in the selected project's README. Match their **names, types, permissions, and update settings** exactly. Use the variables for that lesson, rather than adding all variables from the final project.
4. For lessons 2-5, add `temperature2` and `humidity2` only when using the optional second sensor with `ENABLE_SECOND_TH` set to `1`.

When changing lessons, update the Thing's variables to match the new sketch. If you use a separate Thing for each lesson, associate the board with the Thing you are currently using.

### Option A: Upload from the local Arduino IDE

Keep the project's supplied `thingProperties.h` and `arduino_secrets.h` files alongside its `.ino` file.

1. Copy the **Thing ID** from Arduino Cloud into `thingProperties.h`. This must be the Thing ID, not the Device ID:

   ```cpp
   const char THING_ID[] = "your-thing-id";
   ```

   The template already passes this value to `ArduinoCloud.setThingId(THING_ID)` inside `initProperties()`.

2. Enter your Wi-Fi credentials in `arduino_secrets.h`:

   ```cpp
   #define SECRET_SSID "your-wifi-name"
   #define SECRET_OPTIONAL_PASS "your-wifi-password"
   ```

   Keep populated credentials private. The MKR WiFi 1010 must have completed Cloud device setup; these templates use its secure element and do not require an ESP-style device secret key.

3. Enable Cloud support near the top of the main `.ino` file:

   ```cpp
   const bool CLOUD_ENABLED = true;
   ```

4. Install the libraries listed in the project's README, select **Arduino MKR WiFi 1010**, and upload the sketch. Motor projects require **Grove I2C Motor Driver v1.3, software version 1.0.1**.
5. Open the Serial Monitor at **9600 baud** to inspect startup messages and Cloud connection status.

### Option B: Upload from the Arduino Cloud Editor

1. Open the sketch belonging to the configured Thing and replace its main sketch contents with the selected lesson's `.ino` code.
2. Keep the Thing's automatically generated `thingProperties.h`. Arduino Cloud generates its property declarations and configuration from the Thing. Do not overwrite it with the local template's file containing an empty Thing ID.
3. Configure the Wi-Fi credentials through the Cloud interface. Set `CLOUD_ENABLED` to `true` in the main sketch and enable `ENABLE_SECOND_TH` only if the optional sensor and its two Thing variables are included.
4. Keep only one definition of each actuator callback, such as `onLightChange()`. Our main sketches already provide the required callbacks, so do not append duplicate generated callback functions.
5. Select the libraries and versions listed in the project's README, then compile and upload. When copying only the sketch text, the project's `sketch.json` library selections are not transferred; explicitly select motor library **1.0.1** for lessons 3-5 and the standalone project.

### Dashboard and connection checks

Create a Dashboard and link value displays to the Thing's sensor variables and switches to its actuator variables. The Dashboard is configured separately from the sketch. Set actuator switches to off before connecting: Cloud synchronization may apply stored switch states and override the sketch's startup settings.

If the board stays offline, check `CLOUD_ENABLED`, the Wi-Fi credentials, Device-to-Thing association, and the Thing ID used by the uploaded sketch. If readings or controls do not work, compare the Thing's variables with the lesson's README and check that each Dashboard widget references the intended Thing and variable. Inspect the Serial Monitor for connection and sensor initialization messages.

Further guidance: [Add a device to Arduino Cloud](https://support.arduino.cc/hc/en-us/articles/360016495559-Add-a-device-to-Arduino-Cloud), [configure network credentials](https://support.arduino.cc/hc/en-us/articles/14416141314332-Configure-the-network-credentials-for-a-device), and [Arduino Cloud library documentation](https://github.com/arduino-libraries/ArduinoIoTCloud/blob/master/docs/readme.md).

## Important setup details

- **Motor library:** Steps 3-5 and `GreenHouseSample` require **Grove I2C Motor Driver v1.3, software version 1.0.1**. The library name's “v1.3” is not its software version. Select **1.0.1** in Library Manager or Cloud Editor; local IDE/CLI builds use the installed version. The projects' `sketch.json` files record the required Cloud library versions.
- **Motor speed:** Version 1.0.1 accepts **-100 to +100**. The magnitude is PWM duty cycle in percent, and the sign selects direction. The default value `100` means full duty cycle.
- **BME addresses:** BME boards can use **0x76 or 0x77**, selected through address solder pads. Carrier Rev2 already uses **0x76** for its onboard BME688, so the external BME680 uses **0x77**. Changing a constant does not change the physical address.
- **Manual control:** Actuators retain their last requested state until another command or a restart. There is no automatic shutoff timer. Test the pump with water.
- **Cloud synchronization:** Stored Cloud switch values may override startup settings. Set Dashboard switches to off before connecting.

## Validation

All five lessons and `GreenHouseSample` compile for `arduino:samd:mkrwifi1010` with Cloud disabled and enabled. The optional second sensor was also enabled in the Cloud test builds for steps 2-5. The motor-project build profiles confirm use of **Grove library 1.0.1**.

These are compilation checks. Wiring, actual actuator operation, and live Cloud connections still need to be tested on the kit.

## Authorship and project background

**Author (concept and project direction): David Voss.** David defined the educational goals, requirements, lesson structure, and instructions. The code and documentation were generated by **OpenAI Codex using GPT-6 Astra**, following his guidance; David's contribution was directing the work rather than writing the generated implementation himself.

These educational materials were developed as an outcome of a **2026 Erasmus+ project** between:

- **Berufskolleg Uerdingen**, Krefeld, Germany.
- **Colegiul Tehnic "Costin D. Nenițescu" Pitești**, Romania.
