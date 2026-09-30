# Lesson 3: Fan and pump

Open **GreenHouse_03_BME680_Relay_BME688_Fan_Pump.ino** from its matching folder. This is a standalone project for the **Arduino MKR WiFi 1010** with **MKR IoT Carrier Rev2**. Comments, serial messages, and documentation are in English.

Keep the temperature/humidity readings, optional second sensor, and light relay. Add manual fan and pump control through the Grove motor driver.

There are **no automatic control rules**. Sensor readings never change actuator states. Cloud connectivity is available but disabled by default for the first local hardware test.

## Learning sequence

| Lesson | Project | Added components / readings | Cloud variables | Callbacks |
| --- | --- | --- | --- | --- |
| 1 | [GreenHouse_01_BME680_Temperature_Humidity](../GreenHouse_01_BME680_Temperature_Humidity/README.md) | External BME680: temperature and humidity | 2 | 0 |
| 2 | [GreenHouse_02_BME680_Relay_OptionalBME688](../GreenHouse_02_BME680_Relay_OptionalBME688/README.md) | Add light relay; optional onboard BME688 | 3 (+2 optional) | 1 |
| 3 | [GreenHouse_03_BME680_Relay_BME688_Fan_Pump](../GreenHouse_03_BME680_Relay_BME688_Fan_Pump/README.md) | Add fan and pump | 5 (+2 optional) | 3 |
| 4 | [GreenHouse_04_BME680_Relay_BME688_Fan_Pump_SGP30_Soil](../GreenHouse_04_BME680_Relay_BME688_Fan_Pump_SGP30_Soil/README.md) | Add SGP30 and analog soil moisture | 8 (+2 optional) | 3 |
| 5 | [GreenHouse_05_BME680_Relay_BME688_Fan_Pump_SGP30_Soil_Light](../GreenHouse_05_BME680_Relay_BME688_Fan_Pump_SGP30_Soil_Light/README.md) | Add light sensor, BME pressure/gas resistance, I2C scan | 11 (+2 optional) | 3 |

Each directory has its own sketch, `thingProperties.h`, `arduino_secrets.h`, `sketch.json`, and README. No files or libraries are loaded from another lesson's directory.

## Hardware and constants

| Component | Connection / address | Reading or control |
| --- | --- | --- |
| External BME680 | I2C **0x77** | Temperature in C and relative humidity in % |
| Light relay | **D2**, Carrier Rev2 relay 2 | Manual on/off; use COM/NO contacts |
| Optional onboard BME688 | I2C **0x76**, fixed | Second temperature/humidity reading |
| Grove I2C Motor Driver | I2C **0x0F** | Address must match its switches |
| Fan | **MOTOR2** | Manual on/off |
| Water pump | **MOTOR1** | Manual on/off |

Settings are at the top of the sketch: addresses, timing, serial baud rate, relay pin/levels and initial state, motor channels/speeds and initial states. `Wire` uses the board's fixed **D11/SDA** and **D12/SCL** pins, available on the I2C Grove connector.

All actuators start **off**. Change `INITIAL_LIGHT_ON`, `INITIAL_FAN_ON`, or `INITIAL_PUMP_ON` for startup tests. D2 controls the relay; it does not power the lamp. Connect the lamp through the relay's COM/NO contacts and provide the kit's load supply. The Rev2 pin assignment is not interchangeable with Carrier Rev1.

Motor speeds default to **100** in the driver's **-100 to +100** range, where magnitude is PWM duty cycle in percent. Zero stops the motor; the sign determines direction. If a motor does not start, check its power supply, polarity, and required starting speed. Test the pump with water.

## BME addresses and solder pads

BME sensor boards can use **0x76 or 0x77**, selected through their address solder pads/SDO connection. The physical pad setting must match `BME_I2C_ADDRESS`. Changing the constant does not change the hardware address. The [Grove BME680 documentation](https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/) shows the pads; disconnect power before modifying them.

Two BME sensors on one bus must have **different addresses**. The **Carrier Rev2 already has its onboard BME688 at 0x76**, even when optional readings are disabled. Therefore, use **0x77 for the external BME680** when a Rev2 Carrier is attached. A separate external sensor without that Carrier may use either address. Two external BMEs plus the onboard BME688 would require an I2C multiplexer or a separate bus. BME280 and BME680 require different libraries.

## Optional second temperature/humidity sensor

The second sensor is the **onboard BME688**, so no additional external module is needed. The option is available in lessons 2-5 and defaults to off.

1. Change `#define ENABLE_SECOND_TH 0` near the top of the sketch to `1`.
2. Install Arduino_MKRIoTCarrier and its dependencies.
3. If using Cloud, add `temperature2` and `humidity2` as `float`, Read Only, updated every 3 seconds.
4. Upload again. Second-sensor readings appear every 3 seconds.

With the option set to 0, these two properties are not registered with Cloud and need not exist in the Thing. Remove them from the Thing if you previously added them and want the minimal configuration again. The optional code is guarded by `#if ENABLE_SECOND_TH` so the generated Cloud header can omit these variables.

The Carrier library handles onboard measurement timing and returns cached values. Allow a few measurement cycles after startup; do not use initial values to calibrate the external sensor. The onboard and external sensors are in different physical locations and may read differently. Enabling this option calls `carrier.begin()`, which also initializes other Carrier hardware internally. It does not add light-sensor readings to this lesson.

## Install the board package and libraries

1. In Arduino IDE, open Boards Manager and install **Arduino SAMD Boards (32-bits ARM Cortex-M0+)**. Select **Arduino MKR WiFi 1010** and its USB port. The FQBN is `arduino:samd:mkrwifi1010`; the build environment uses core **1.8.14**.
2. Open Library Manager, search for each required library below, and choose **Install all dependencies** when offered.
3. Use **BME680 by Zanduino** with `Zanshin_BME680.h`; similarly named BME libraries are not interchangeable.
4. `Wire.h` is supplied by the board package. Libraries are referenced with `#include`; their source files are not bundled in the project.

| Library Manager name | Header | Build environment version | Required when |
| --- | --- | --- | --- |
| BME680 by Zanduino | `Zanshin_BME680.h` | 1.0.10 | Required |
| Arduino_MKRIoTCarrier | `Arduino_MKRIoTCarrier.h` | 2.1.0 | Only required when ENABLE_SECOND_TH is 1 |
| Grove I2C Motor Driver v1.3 | `Grove_I2C_Motor_Driver.h` | 1.0.1 | Required |
| ArduinoIoTCloud | `ArduinoIoTCloud.h` | 2.10.0 | Required, even with Cloud disabled |
| Arduino_ConnectionHandler | `Arduino_ConnectionHandler.h` | 1.3.0 | Required, even with Cloud disabled |

Arduino_MKRIoTCarrier's dependencies include the BSEC Software Library and the other sensor/display libraries selected by Library Manager. Cloud dependencies include WiFiNINA and the security/network libraries selected by Library Manager. Cloud libraries are included even when `CLOUD_ENABLED` is false.

### Required motor library version

Install **Grove I2C Motor Driver v1.3, library version 1.0.1**. The "v1.3" in the library name is not the software version. In Library Manager, choose **1.0.1** from the version selector; do not update this library for these classroom boards.

For Arduino CLI:

```text
arduino-cli lib install "Grove I2C Motor Driver v1.3@1.0.1"
```

The project's `sketch.json` records this version for Arduino Cloud Editor. When copying only the sketch text into a Thing, select library version **1.0.1** in the Cloud Editor as well. Local Arduino IDE/CLI builds use the installed library; a plain `#include` does not enforce a version. Check the compile output if multiple library copies are installed.

Version 1.0.1 accepts **-100 to +100** in `Motor.speed()`: magnitude is PWM duty cycle in percent and the sign selects direction. The default value **100 means 100% duty cycle**. The sketch uses the compatible `Motor.begin(address)` and `Motor.speed(channel, speed)` calls, with no newer firmware commands. Compile-time range checks reject motor speed constants outside -100 to +100.

## First hardware test

1. Check wiring, addresses, and initial settings. Leave `CLOUD_ENABLED = false`.
2. Open the matching `.ino` file, compile, and upload.
3. Open Serial Monitor at **9600 baud**. Press Reset if the startup messages have already passed; the sketch waits at most 3 seconds for the monitor.
4. Gently warm the external BME680 and observe temperature/humidity readings, updated approximately once per second.
5. Send `l`, then `L`, and compare the relay messages with the lamp's response.
6. Send `f`, then `F`; send `p`, then `P`. End with `0` to switch every actuator off.

Missing modules are reported without an intentional infinite wait. Fix the wiring and restart: initialization is not automatically retried. Temperature/humidity use **-999** as the unavailable/uninitialized sentinel. Sentinel values sent to Cloud are not physical measurements.

### Serial control

Commands are case-sensitive; line endings are ignored.

| Command | Action |
| --- | --- |
| `l` / `L` | Light relay on / off |
| `f` / `F` | Fan on / off |
| `p` / `P` | Pump on / off |
| `0` | All actuators off |

Actuators retain their requested state until another command or restart. There is no automatic shutoff timer. Messages confirm requested commands, not actual motion or illumination; the actuators have no feedback sensors.

## Arduino Cloud

This project includes the connection code and exactly the properties for this lesson. The sketch does not create a Device, Thing, or Dashboard in your account.

| Variable | Type | Permission / interval | Unit or widget |
| --- | --- | --- | --- |
| `temperature` | `float` | Read Only / 1 second | C |
| `humidity` | `float` | Read Only / 1 second | % RH |
| `light` | `bool` | Read & Write / On Change | Switch |
| `pump` | `bool` | Read & Write / On Change | Switch |
| `fan` | `bool` | Read & Write / On Change | Switch |
| `temperature2` (optional) | `float` | Read Only / 3 seconds | C |
| `humidity2` (optional) | `float` | Read Only / 3 seconds | % RH |

1. Provision the MKR WiFi 1010 as an Arduino Cloud Device and associate it with a Thing for this lesson. Use a separate Thing per lesson or adjust an existing Thing to match the table.
2. Create the listed variables with exactly these names and types. Add the optional pair only if `ENABLE_SECOND_TH` is 1.
3. For local IDE use, enter the Thing ID in `thingProperties.h` and Wi-Fi credentials in `arduino_secrets.h`. Do not distribute populated credentials. The MKR WiFi 1010 uses its secure element.
4. For Cloud Editor use, copy the main sketch into the Thing's sketch and use its generated `thingProperties.h`. Configure the network credentials in Cloud. Keep only one definition of each actuator callback; the main sketch already supplies them.
5. Set `CLOUD_ENABLED = true`, compile, and upload.
6. Create a Dashboard with value displays for the sensor variables and switches for the actuators. Link each widget to the corresponding Thing variable.

There are 3 actuator callbacks: `onLightChange()`, `onPumpChange()`, `onFanChange()`. Sensor values are read-only and require no callbacks. Cloud and serial commands update the same state variables; the last processed command wins. Cloud synchronization can apply stored switch states and override startup settings. Set Dashboard switches to off before connecting. A connection loss leaves actuators in their last requested state.

`ArduinoCloud.update()` is serviced frequently; the sketch does not deliberately wait for a Cloud connection before operating the hardware. Available Cloud variable quotas must accommodate this lesson's variable count.

## Documentation and validation

- [Arduino Greenhouse Kit](https://www.arduino.cc/education/greenhouse)
- [Zanduino BME680 library](https://github.com/Zanduino/BME680)
- [Grove BME680 address pads](https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/)
- [Arduino MKR IoT Carrier library](https://github.com/arduino-libraries/Arduino_MKRIoTCarrier)
- [Grove motor driver library](https://github.com/Seeed-Studio/Grove_I2C_Motor_Driver_v1_3)
- [Arduino Cloud library](https://github.com/arduino-libraries/ArduinoIoTCloud)

Validated with Arduino CLI for `arduino:samd:mkrwifi1010` (Arduino SAMD Boards 1.8.14): the default configuration and a test copy with Cloud enabled both compile successfully. The Cloud-enabled test also enables the optional second T/H sensor. Both build profiles confirm that Grove I2C Motor Driver v1.3 **1.0.1** was actually used. No sketch was uploaded to hardware and no live Cloud connection was tested.

## Authorship and project background

**Author (concept and project direction): David Voss.** David defined the educational goals, requirements, lesson structure, and instructions. The code and documentation were generated by **OpenAI Codex using GPT-6 Astra**, following his guidance; David's contribution was directing the work rather than writing the generated implementation himself.

These educational materials were developed as an outcome of a **2026 Erasmus+ project** between:

- **Berufskolleg Uerdingen**, Krefeld, Germany.
- **Colegiul Tehnic "Costin D. Nenițescu" Pitești**, Romania.
