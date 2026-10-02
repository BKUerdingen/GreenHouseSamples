# Lesson 6: Display pages and touch control

Open **GreenHouse_06_BME680_BME688_SGP30_Soil_Fan_Pump_Light_TFT_Touch.ino** from its matching folder. This standalone project extends [lesson 5](../GreenHouse_05_BME680_Relay_BME688_Fan_Pump_SGP30_Soil_Light/README.md) with the **MKR IoT Carrier Rev2's onboard 240 x 240 round display and five capacitive touch buttons**. It uses an **Arduino MKR WiFi 1010** and the same Greenhouse Kit sensors and actuators.

All comments, documentation, display labels, and serial messages are in English. There are no automatic greenhouse control rules. All actuator changes are manual: touch buttons, serial commands, initial settings, or Cloud switches.

Cloud is **disabled by default**, all actuators start **off**, and the onboard second BME688 readings are **enabled by default** in this lesson. Wi-Fi credential fields are blank. No extra Cloud variables or callbacks are introduced for the display or touch buttons.

[Back to the sample overview](../Readme.md)

## Project files

- `GreenHouse_06_BME680_BME688_SGP30_Soil_Fan_Pump_Light_TFT_Touch.ino`: sensor readings, actuator commands, touch handling, and display pages.
- `thingProperties.h`: 11 Cloud variables, plus the optional 2 onboard BME688 variables, and 3 actuator callbacks.
- `arduino_secrets.h`: Wi-Fi credential placeholders.
- `sketch.json`: board and library version metadata.
- `README.md`: setup, controls, and hardware checks.

## Touch controls

The Carrier buttons are labelled **00 through 04**; there is no button 05. The agreed mapping is:

| Touch button | Action |
| --- | --- |
| 00 | Toggle fan on/off |
| 01 | Toggle pump on/off |
| 02 | Toggle light relay on/off |
| 03 | Previous page (page up) |
| 04 | Next page (page down) |

Every button works on every page. Holding a button produces one action; release and touch it again to repeat. Page navigation wraps from the first page to the last and vice versa. `FAN_TOUCH`, `PUMP_TOUCH`, `LIGHT_TOUCH`, `PAGE_UP_TOUCH`, and `PAGE_DOWN_TOUCH` are constants near the top of the sketch. Keep assignments distinct when changing them.

Set `CARRIER_HAS_CASE` to match whether the Carrier is inside its plastic enclosure. Keep fingers away from the touch pads during startup calibration. The Carrier library maps the touch pads to hardware; their numbers are touch indices, not Arduino GPIO numbers.

Touch actions and page changes are logged to Serial. Touch, Cloud, and serial commands update the same actuator variables, so touch changes are also available to Cloud when connected. Displayed actuator states are **requested states**, not physical feedback. A disconnected motor driver cannot confirm whether the pump or fan is actually running.

## Display pages

All pages use **text size 2**: characters are twice as wide and twice as tall as before (12 x 16 pixels). Each detail page has up to seven rows; long information is split across pages.

| Page | Contents |
| --- | --- |
| 1: Overview | Only Cloud variable values, with the abbreviations below. |
| 2: BME680 T/H | Address, temperature/humidity, full Cloud names, type and units. |
| 3: BME680 P/G | Address, pressure/gas resistance, full Cloud names, type and units. |
| 4: BME688 T/H | Address, optional second temperature/humidity, full Cloud names, type and units. |
| 5: SGP30 | Address, eCO2/TVOC, full Cloud names, type and units. |
| 6: Soil | Analog pin, raw ADC value/range, Cloud name and type; no I2C address. |
| 7: Light | Address, raw clear count, Cloud name and type. |
| 8: Light / RGB | Address and local RGB counts (not Cloud variables). |
| 9: Cloud | Cloud/Wi-Fi status and Cloud variable count. |
| 10: Wi-Fi | Full configured SSID, IP and RSSI. |
| 11: Thing ID | Full runtime Thing ID, available when connected. |
| 12: Device ID | Full Device ID from the secure element. |

| Overview label | Cloud variable | Display unit |
| --- | --- | --- |
| T / H | `temperature` / `humidity` | C / % RH, rounded |
| T2 / H2 | `temperature2` / `humidity2` | C / % RH, rounded; omitted when disabled |
| Pr | `pressure` | hPa, rounded |
| G | `gasResistance` | kOhm, rounded; `k` suffix (Cloud retains Ohm) |
| C / V | `eCO2` / `tvoc` | ppm / ppb |
| S / L | `soilRaw` / `lightLevel` | Raw ADC / raw clear count |
| F / P / R | `fan` / `pump` / `light` | Requested state: 0 = off, 1 = on |

The overview uses seven compact rows. It excludes local RGB readings, connection status and diagnostic messages. `--` means no valid reading, not zero. Detail pages retain more precision. Soil values are not percentages, light counts are not lux, and eCO2 is an estimate rather than a direct CO2 measurement. Allow about 15 seconds for the SGP30's initial warm-up values to settle.

The central text area fits within the round display. Full IDs and a 32-character SSID wrap without losing characters. Passwords are never displayed. Only changed rows are redrawn; changing pages clears the screen. No full-screen framebuffer is allocated. `INITIAL_PAGE` (zero based), `DISPLAY_ROTATION`, `DISPLAY_TEXT_SIZE`, colors and `DISPLAY_REFRESH_MS` are constants near the top. The geometry is designed for text size 2; increasing the font further also requires adjusting rows, columns and positions.

Display code uses cached measurements. It does not read sensors again or add deliberate delays to the loop. The existing approximately 1 Hz sensor schedule, 3-second onboard BME schedule, and regular `ArduinoCloud.update()` calls remain active. Actual timing depends on sensor and network operations.

The screen uses SPI through the Carrier library, not an I2C address. Rev2 uses D13 for TFT chip select, D14 for data/command, and D3 for backlight. `carrier.begin()` initializes the screen and touch pads even when it reports a sensor failure; the sketch explicitly enables the Rev2 backlight afterward.

## Hardware and constants

| Component | Connection / address | Reading or control |
| --- | --- | --- |
| External BME680 | I2C **0x77** | Temperature in C and relative humidity in % |
| Light relay | **D2**, Carrier Rev2 relay 2 | Manual on/off; use COM/NO contacts |
| Optional onboard BME688 | I2C **0x76**, fixed | Second temperature/humidity reading |
| Grove I2C Motor Driver | I2C **0x0F** | Address must match its switches |
| Fan | **MOTOR2** | Manual on/off |
| Water pump | **MOTOR1** | Manual on/off |
| SGP30 | I2C **0x58**, fixed | eCO2 in ppm and TVOC in ppb |
| Analog soil moisture sensor | **A0** | Raw ADC value, 0-1023 |
| APDS9960 light sensor | I2C **0x39**, fixed | Raw RGB and clear values; clear value also sent to Cloud |
| External BME680 (additional channels) | Same sensor at **0x77** | Pressure in hPa, gas resistance in ohms |

Settings are at the top of the sketch: addresses, timing, serial baud rate, relay pin/levels and initial state, motor channels/speeds and initial states, and the soil input pin/ADC resolution. `Wire` uses the board's fixed **D11/SDA** and **D12/SCL** pins, available on the I2C Grove connector.

All actuators start **off**. Change `INITIAL_LIGHT_ON`, `INITIAL_FAN_ON`, or `INITIAL_PUMP_ON` for startup tests. D2 controls the relay; it does not power the lamp. Connect the lamp through the relay's COM/NO contacts and provide the kit's load supply. The Rev2 pin assignment is not interchangeable with Carrier Rev1.

Motor speeds default to **100** in the driver's **-100 to +100** range, where magnitude is PWM duty cycle in percent. Zero stops the motor; the sign determines direction. If a motor does not start, check its power supply, polarity, and required starting speed. Test the pump with water.

## BME addresses and solder pads

BME sensor boards can use **0x76 or 0x77**, selected through their address solder pads/SDO connection. The physical pad setting must match `BME_I2C_ADDRESS`. Changing the constant does not change the hardware address. The [Grove BME680 documentation](https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/) shows the pads; disconnect power before modifying them.

Two BME sensors on one bus must have **different addresses**. The **Carrier Rev2 already has its onboard BME688 at 0x76**, even when optional readings are disabled. Therefore, use **0x77 for the external BME680** when a Rev2 Carrier is attached. A separate external sensor without that Carrier may use either address. Two external BMEs plus the onboard BME688 would require an I2C multiplexer or a separate bus. BME280 and BME680 require different libraries.

## Optional second temperature/humidity sensor

The second sensor is the **onboard BME688**, so no additional external module is needed. The option is available in lessons 2-6. In lesson 6, it defaults to **on** so its display page shows measurements.

1. Leave `ENABLE_SECOND_TH` at `1` for onboard readings, or set it to `0` to disable them.
2. Install Arduino_MKRIoTCarrier and its dependencies (already required in this lesson).
3. If using Cloud, add `temperature2` and `humidity2` as `float`, Read Only, updated every 3 seconds.
4. Upload again. Second-sensor values update every 3 seconds; the latest values appear in every serial measurement block.

With the option set to 0, these two properties are not registered with Cloud and need not exist in the Thing. Remove them from the Thing if you previously added them and want the minimal configuration again. The optional code is guarded by `#if ENABLE_SECOND_TH` so the generated Cloud header can omit these variables.

The Carrier library handles onboard measurement timing and returns cached values. Allow a few measurement cycles after startup; do not use initial values to calibrate the external sensor. The onboard and external sensors are in different physical locations and may read differently. The Carrier is initialized in this lesson regardless of the optional second T/H readings.

## Install the board package and libraries

1. In Arduino IDE, open Boards Manager and install **Arduino SAMD Boards (32-bits ARM Cortex-M0+)**. Select **Arduino MKR WiFi 1010** and its USB port. The FQBN is `arduino:samd:mkrwifi1010`; the build environment uses core **1.8.14**.
2. Open Library Manager, search for each required library below, and choose **Install all dependencies** when offered.
3. Use **BME680 by Zanduino** with `Zanshin_BME680.h`; similarly named BME libraries are not interchangeable.
4. `Wire.h` is supplied by the board package. Libraries are referenced with `#include`; their source files are not bundled in the project.

| Library Manager name | Header | Build environment version | Required when |
| --- | --- | --- | --- |
| BME680 by Zanduino | `Zanshin_BME680.h` | 1.0.10 | Required |
| Arduino_MKRIoTCarrier | `Arduino_MKRIoTCarrier.h` | 2.1.0 | Required for display, touch, and onboard sensors |
| Grove I2C Motor Driver v1.3 | `Grove_I2C_Motor_Driver.h` | 1.0.1 | Required |
| Adafruit SGP30 Sensor | `Adafruit_SGP30.h` | 2.0.3 | Required |
| ArduinoIoTCloud | `ArduinoIoTCloud.h` | 2.10.0 | Required, even with Cloud disabled |
| WiFiNINA | `WiFiNINA.h` | 2.1.1 | Required for network status, IP and RSSI |
| Arduino_ConnectionHandler | `Arduino_ConnectionHandler.h` | 1.3.0 | Required, even with Cloud disabled |

Arduino_MKRIoTCarrier's dependencies include the BSEC Software Library and the other sensor/display libraries selected by Library Manager. Adafruit SGP30 Sensor also requires Adafruit BusIO. Cloud dependencies include WiFiNINA and the security/network libraries selected by Library Manager. Display and touch support are included through Arduino_MKRIoTCarrier. Install its Adafruit GFX, Adafruit ST7735 and ST7789, Adafruit BusIO, and Arduino_MCHPTouch dependencies when prompted. Cloud libraries are included even when `CLOUD_ENABLED` is false.

### Required motor library version

Install **Grove I2C Motor Driver v1.3, library version 1.0.1**. The "v1.3" in the library name is not the software version. In Library Manager, choose **1.0.1** from the version selector; do not update this library for these classroom boards.

For Arduino CLI:

```text
arduino-cli lib install "Grove I2C Motor Driver v1.3@1.0.1"
```

The project's `sketch.json` records this version for Arduino Cloud Editor. When copying only the sketch text into a Thing, select library version **1.0.1** in the Cloud Editor as well. Local Arduino IDE/CLI builds use the installed library; a plain `#include` does not enforce a version. Check the compile output if multiple library copies are installed.

Version 1.0.1 accepts **-100 to +100** in `Motor.speed()`: magnitude is PWM duty cycle in percent and the sign selects direction. The default value **100 means 100% duty cycle**. The sketch uses the compatible `Motor.begin(address)` and `Motor.speed(channel, speed)` calls, with no newer firmware commands. Compile-time range checks reject motor speed constants outside -100 to +100.

## Arduino Cloud

Create the Thing variables below with exactly these names and types. There are **13 variables** with the default onboard BME option, or **11** with that option disabled. The three callbacks are `onPumpChange()`, `onFanChange()`, and `onLightChange()`.

| Variable | Type | Permission / interval | Unit or widget |
| --- | --- | --- | --- |
| `temperature` | `float` | Read Only / 1 second | C |
| `humidity` | `float` | Read Only / 1 second | % RH |
| `eCO2` | `int` | Read Only / 1 second | ppm |
| `tvoc` | `int` | Read Only / 1 second | ppb |
| `soilRaw` | `int` | Read Only / 1 second | ADC 0-1023 |
| `lightLevel` | `int` | Read Only / 1 second | raw clear |
| `pressure` | `float` | Read Only / 1 second | hPa |
| `gasResistance` | `float` | Read Only / 1 second | ohm |
| `light` | `bool` | Read & Write / On Change | Switch |
| `pump` | `bool` | Read & Write / On Change | Switch |
| `fan` | `bool` | Read & Write / On Change | Switch |
| `temperature2` (optional) | `float` | Read Only / 3 seconds | C |
| `humidity2` (optional) | `float` | Read Only / 3 seconds | % RH |

For a local upload, provision the MKR WiFi 1010 as a Cloud Device and associate it with the correct Thing. As in all other samples, enter your **Thing ID** in `thingProperties.h`:

```cpp
const char THING_ID[] = "your-thing-id";
```

The template passes it to `ArduinoCloud.setThingId(THING_ID)` inside `initProperties()`. Enter Wi-Fi credentials in `arduino_secrets.h` and set `CLOUD_ENABLED = true`.

**Thing ID and Device ID are different.** Do not put the Device ID in `THING_ID`. The MKR WiFi 1010 reads its Device ID from the provisioned secure element. ArduinoIoTCloud 2.10.0 also receives the Thing association from Cloud, so manually setting the Thing ID is not technically required for that workflow. All samples retain the same field and setup pattern for teaching consistency. The Device must still be associated with the intended Thing in Cloud.

For the Cloud Editor, create the matching Thing variables, replace the Thing's main sketch with this `.ino` code, keep its automatically generated `thingProperties.h`, configure network credentials, and enable Cloud. Keep only one copy of each actuator callback. Select Grove motor library **1.0.1** explicitly when copying sketch text without its metadata.

The Cloud page displays the runtime Thing ID while connected. Until then it shows `(not available)`. With Cloud disabled, no Cloud connection is started and device identity is not loaded by this sketch. The Device ID becomes available after successful Cloud initialization; the password is never shown. The SSID label explicitly identifies the configured network, including while disconnected.

Create Dashboard value displays and switches linked to the correct Thing variables. Stored Cloud switch values can override startup settings, so set switches off before connecting. The last processed manual command wins. Losing Cloud connectivity does not automatically stop actuators; local touch and serial control remain available.

See the [central Cloud setup guide](../Readme.md#connecting-a-sketch-to-arduino-cloud) for both upload workflows.

## Serial commands

Open Serial Monitor at **9600 baud**. Commands are case-sensitive; line endings are ignored.

| Command | Action |
| --- | --- |
| `p` / `P` | Pump on / off |
| `f` / `F` | Fan on / off |
| `l` / `L` | Light relay on / off |
| `0` | All actuators off |
| `[` / `]` | Previous / next display page |
| `s` | Print requested actuator states |
| `i` | Scan I2C addresses |
| `d` | Reinitialize display and run the color test |
| `?` | Print command help |

Sensor readings, requested actuator states, initialization failures, touch actions, page changes, and Cloud/Wi-Fi status changes are reported through Serial.

## Light sensor readings

The onboard APDS9960 uses I2C address **0x39**. In the sketch, `readSensors()`
calls `readLightSensor()`, which assigns the sensor's clear channel to the
Cloud variable **`lightLevel`** (`int`, Read Only). This is a raw light count,
not lux. The separate Boolean variable `light` controls the light relay.

The Arduino_APDS9960 library starts a conversion in `colorAvailable()` and
disables it after `readColor()`. Checking availability only once immediately
after restarting a conversion can therefore report no reading. The sketch
polls briefly until the conversion is ready, with an upper waiting limit of
`LIGHT_READ_TIMEOUT_MS` (100 ms), and checks whether `readColor()` succeeds.
No new Cloud variables or callbacks are needed.

**Cloud and the shared I2C bus:** On the MKR WiFi 1010, the installed
ArduinoECCX08 library can leave the shared Wire bus at **1 MHz** after accessing
the security chip for Cloud authentication or random numbers. The APDS9960
supports at most **400 kHz**. An I2C address acknowledgement alone does not prove
that register reads work at that speed; the symptom can be a persistent
`measurement timeout` even though the sensor responds at address 0x39.

The sketch restores `I2C_CLOCK_HZ` (**100 kHz**) after `ArduinoCloud.begin()` and
every `ArduinoCloud.update()`. The `i2cPresent()` helper also restores this speed
before device access, including motor commands invoked by Cloud callbacks.
Keep these calls when copying the sketch into Arduino Cloud. Increasing the
light timeout does not correct an excessive I2C clock rate.

Hardware check (2026-10-02): a temporary Cloud-enabled sample 6 build using the
Teacher configuration was uploaded to the MKR WiFi 1010. Before this change,
the light sensor timed out every cycle. After the change, all 20 observed
cycles returned RGB/clear readings (clear counts 368 to 394), without a
timeout. Sample 5 and both the template and Teacher configurations of sample 6
compiled successfully. Sample 5 was not separately uploaded. The SGP30 was
disconnected during this check, so its missing readings were expected.

References: [ArduinoECCX08 source](https://github.com/arduino-libraries/ArduinoECCX08/blob/master/src/ECCX08.cpp)
and [APDS9960 datasheet](https://content.arduino.cc/assets/Nano_BLE_Sense_av02-4191en_ds_apds-9960.pdf).

The serial output keeps one light row per measurement cycle. A missing sensor,
conversion timeout, or failed I2C read is identified on that row and sets
`lightLevel` to `-1`. A valid zero is a real reading, not an error. To test,
cover and uncover the sensor and watch `clear` in Serial and `lightLevel` in
Cloud (and the light pages in sample 6). If unavailable, check the startup
message `Light sensor @ 0x39: OK` and use the `i` command to scan the I2C bus.

## Fixed serial measurement blocks

Each measurement cycle prints **nine lines in the same order**: a header, onboard BME688, SGP30, three external BME680 lines (temperature/humidity, pressure/gas resistance, gas status), soil, light, and requested actuator states.

The onboard BME688 is still read every three seconds. Its latest cached temperature and humidity are printed in every measurement cycle, including the intervening cycles. Before the first reading or after a failed read, missing values appear as `--`. With `ENABLE_SECOND_TH = 0`, its row says `disabled` instead of disappearing. Other unavailable sensors also retain their rows with placeholders.

The serial formatting update compiles successfully for the MKR WiFi 1010 with the current project settings. The shorter BME680 lines reduce terminal wrapping. Keep the Serial Monitor wide enough for at least 80 characters. Startup messages, explicit commands, actuator changes, page changes and Cloud/network events remain separate event messages and may add lines between measurement blocks.

## Dark-display diagnostics

The screen is explicitly reinitialized after Carrier, sensor and Cloud setup. Subsequent display transfers use a conservative **4 MHz SPI clock**. This is a recovery and diagnostic measure; compilation alone does not establish the cause of a dark screen.

The automatic color test is disabled by default now that a replacement Carrier has confirmed display operation. With `DISPLAY_STARTUP_TEST = true`, startup shows **red, green, blue and white**, each for 750 ms, then the selected information page. The color test bypasses the measurement-page renderer. It does not use delay loops: sensor readings, Cloud updates and touch controls continue. Page navigation cancels the test. Set the constant to `false` to skip the automatic test after the display has been verified.

Send `d` in the Serial Monitor at **9600 baud** to repeat initialization and the test. The one-time LCD initialization uses the display library's short initialization delays; the following color sequence is nonblocking. Serial output reports:

- Detected Carrier revision (this sample requires Rev2 wiring).
- Display CS/DC pins (Rev2: D13/D14).
- Backlight control pin and readback (Rev2: D3, expected HIGH).
- Each color command and return to the information pages.

These messages confirm which code ran and which commands were sent; they cannot confirm that the LCD or its backlight responded. If all four colors remain invisible, report these display messages. If the colors appear but the pages do not, report that distinction. If revision 1 is reported, verify the actual Carrier model before changing wiring or pin constants.

## Hardware acceptance checks

1. Install the board package and libraries above, check addresses and wiring, and upload with Cloud disabled.
2. Verify the overview appears and shows changing sensor values. Missing readings should show `--`.
3. Touch 00, 01, and 02 individually. Check the actuator, overview state, and serial log. Hold each button and confirm it does not toggle repeatedly; release and touch again to switch off. Test the pump with water.
4. Use 03 and 04 to visit all twelve pages, including wraparound. Try `[` and `]` in Serial as an alternative.
5. Compare each sensor page with the serial values and Cloud variable table. Cover the light sensor and compare RGB/clear readings; compare wet/dry soil; allow the SGP30 warm-up period.
6. Enable Cloud after configuring the Device, Thing, network, and variables. Verify the Cloud page's IDs, SSID, connection state, IP and RSSI, then test touch-to-Dashboard and Dashboard-to-actuator changes.
7. Temporarily disconnect Wi-Fi. Verify the status updates while touch and serial control remain usable.
8. Set `ENABLE_SECOND_TH` to `0`, adjust the Thing variables, and upload again. The onboard page should explain that readings are disabled; all other pages and controls should continue to work.
9. Finish by sending `0` to switch all actuators off.

## Validation

Compiled successfully for `arduino:samd:mkrwifi1010` with Arduino SAMD Boards **1.8.14**:

The previous display version worked after the user replaced the Carrier; the original dark-screen fault was therefore traced to the original Carrier hardware. Touch input also worked during the investigation.

The larger-text revision compiles with the current Cloud-enabled configuration: **206,368 bytes flash (78%)** and **10,604 bytes static RAM (32%)**. Layout checks passed for all twelve page titles/footers, seven size-2 body rows within the circular display, page wraparound, full 36-character IDs, a 32-character SSID, and boundary values in the overview. A software preview using the installed Adafruit font was also inspected. Those build sizes describe the display revision before the serial formatting update. Its visual readability must still be confirmed on the replacement Carrier. Actual actuator operation and complete live Cloud behavior require the acceptance checks above.

## References

- [MKR IoT Carrier Rev2](https://docs.arduino.cc/hardware/mkr-iot-carrier-rev2)
- [Carrier library source and examples](https://github.com/arduino-libraries/Arduino_MKRIoTCarrier)
- [Arduino Cloud library](https://github.com/arduino-libraries/ArduinoIoTCloud)
- [Grove BME680 address pads](https://wiki.seeedstudio.com/Grove-Temperature_Humidity_Pressure_Gas_Sensor_BME680/)

## Authorship and project background

**Author (concept and project direction): David Voss.** David defined the educational goals, requirements, lesson structure, and instructions. The code and documentation were generated by **OpenAI Codex using GPT-6 Astra**, following his guidance; David's contribution was directing the work rather than writing the generated implementation himself.

These educational materials were developed as an outcome of a **2026 Erasmus+ project** between:

- **Berufskolleg Uerdingen**, Krefeld, Germany.
- **Colegiul Tehnic "Costin D. Nenițescu" Pitești**, Romania.
