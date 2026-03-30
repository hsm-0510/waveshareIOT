# waveshareIOT

ESP32-S3 firmware for a Waveshare-based industrial I/O module that:

1. brings up a **W5500 Ethernet interface**,
2. controls **relay / digital outputs** through a **TCA9554 I2C I/O expander**,
3. reads **8 local digital inputs** from ESP32 GPIOs, and
4. exchanges basic I/O state over **JSON messages on a TCP socket**.

This repository is currently best understood as an **early working prototype** for weighbridge / lane-barrier style automation. The code is already structured into separate modules for Ethernet, TCP/JSON handling, digital inputs, and extended I/O, but a few parts are still unfinished or inconsistent with the checked-in config file.

---

## What the firmware currently does

On boot, the firmware:

- starts the serial console at `115200`,
- initializes I2C on ESP32-S3 pins `SDA=42` and `SCL=41`,
- configures the TCA9554 output expander,
- configures 8 GPIO digital inputs on pins `4` to `11`,
- initializes the W5500 Ethernet controller, and
- starts a TCP server on the configured Ethernet interface.

After a client connects, the main loop:

- reads an incoming JSON line from the TCP connection,
- updates internal **output** and **status** arrays from the received JSON,
- applies output states to the TCA9554,
- samples the 8 digital inputs,
- sends the current input states back to the client as JSON.

---

## Implemented hardware mapping

### MCU / platform

- **Board:** `esp32-s3-devkitc-1`
- **Framework:** Arduino via PlatformIO
- **Platform:** `espressif32@6.7.0`

### W5500 Ethernet SPI pins

| Signal | ESP32-S3 Pin |
|---|---:|
| SCK  | 15 |
| CS   | 16 |
| MOSI | 13 |
| MISO | 14 |

### I2C pins

| Signal | ESP32-S3 Pin |
|---|---:|
| SDA | 42 |
| SCL | 41 |

### Local digital inputs

| Input | ESP32-S3 Pin |
|---|---:|
| DI1 | 4 |
| DI2 | 5 |
| DI3 | 6 |
| DI4 | 7 |
| DI5 | 8 |
| DI6 | 9 |
| DI7 | 10 |
| DI8 | 11 |

### TCA9554 output expander

The code maps 8 expandable outputs to TCA9554 pins 1 through 8:

`{1, 2, 3, 4, 5, 6, 7, 8}`

The device address is fixed to:

- **TCA9554 I2C address:** `0x20`

---

## Network behavior

Despite the example `config/protocol.json`, the firmware **currently behaves as a TCP server**, not a TCP client.

### Active behavior in code

- Static IP: `192.168.10.60`
- TCP port: `80`
- Ethernet MAC: `DE:AD:BE:EF:FE:ED`

A remote host can connect to the module over TCP and exchange newline-terminated JSON messages.

---

## JSON model currently implemented in firmware

The code maintains three internal key/value groups.

### 1) Inputs sent by the device

These are sampled from the ESP32 GPIO pins and sent back under the `inputs` object:

```json
{
  "inputs": {
    "entranceLB_status": 0,
    "exitLB_status": 0,
    "irSens01_entranceLB": 0,
    "irSens02_entranceLB": 0,
    "irSens03_entranceLB": 0,
    "irSens01_exitLB": 0,
    "irSens02_exitLB": 0,
    "irSens03_exitLB": 0
  }
}
```

### 2) Outputs received from the client

These are expected under the `outputs` object in incoming JSON:

```json
{
  "outputs": {
    "control_entranceLB": 1,
    "control_exitLB": 0
  }
}
```

### 3) Status values received from the client

These are expected under the `status` object in incoming JSON:

```json
{
  "status": {
    "vehicle_alignment_status": 1,
    "driver_absence_status": 0,
    "entrance_weight": 12345,
    "exit_weight": 0,
    "entrance_weight_status": 1,
    "exit_weight_status": 0,
    "rfid_scan_status": 1,
    "entrance_print_status": 0,
    "exit_print_status": 0
  }
}
```

### Example full message to send to the device

```json
{
  "outputs": {
    "control_entranceLB": 1,
    "control_exitLB": 0
  },
  "status": {
    "vehicle_alignment_status": 1,
    "driver_absence_status": 0,
    "entrance_weight": 15230,
    "exit_weight": 0,
    "entrance_weight_status": 1,
    "exit_weight_status": 0,
    "rfid_scan_status": 1,
    "entrance_print_status": 0,
    "exit_print_status": 0
  }
}
```

> Important: the receiver logic expects a **newline (`\n`) terminated JSON payload**.

---

## Repository structure

```text
waveshareIOT/
├── config/
│   └── protocol.json              # Example protocol / configuration document (not yet used by firmware)
├── include/                       # Default PlatformIO include folder placeholder
├── lib/                           # Default PlatformIO library folder placeholder
├── src/
│   ├── main.cpp                   # Application entry point and main loop
│   ├── CMakeLists.txt             # Auto-generated ESP-IDF component registration
│   ├── extended_io/
│   │   ├── digital_inputs.*       # Local GPIO input initialization and sampling
│   │   ├── i2c_driver.*           # Wire-based I2C helpers
│   │   ├── tca9554.*              # TCA9554 read/write and output control
│   │   └── readme.txt             # TCA9554 notes copied from module/device info
│   ├── host_interface/
│   │   └── w5500_eth.*            # W5500 SPI + Ethernet server startup
│   ├── serial_interface/
│   │   └── serial_interface.h     # Commented-out UART example / placeholder
│   └── tcp_server/
│       ├── tcp_server.*           # TCP JSON send/receive and key/value storage
├── test/                          # Default PlatformIO test folder placeholder
├── platformio.ini                 # Main PlatformIO project configuration
├── sdkconfig.esp32-s3-devkitc-1   # ESP-IDF sdkconfig generated for this environment
└── CMakeLists.txt                 # Root ESP-IDF CMake project file
```

---

## Build environment

`platformio.ini` currently defines:

- environment: `esp32-s3-devkitc-1`
- framework: `arduino`
- upload port: `COM11`
- dependencies:
  - `arduino-libraries/NTPClient`
  - `arduino-libraries/Ethernet`
  - `bblanchon/ArduinoJson`

### Build with PlatformIO

```bash
pio run
```

### Upload to the board

```bash
pio run -t upload
```

### Open serial monitor

```bash
pio device monitor -b 115200
```

If your board is not on `COM11`, update `upload_port` in `platformio.ini` first.

---

## Serial output you can expect

The firmware prints useful startup and runtime diagnostics such as:

- Ethernet wiring / SPI pin mapping
- assigned local IP address
- client connection events
- incoming JSON processing
- current output array values
- current input array values

This makes the serial console the easiest first step when bringing the hardware up.

---

## Current limitations / code-review notes

These points are worth knowing before extending the project:

### 1) `protocol.json` is not used by the firmware

The repository contains `config/protocol.json`, but the firmware does **not** load or parse it. It is currently acting more like a reference document than a live configuration source.

### 2) Config JSON and implemented JSON do not match exactly

There are naming differences between `protocol.json` and the actual keys in `tcp_server.cpp`.

Examples:

- config: `irSens_01_entranceWB`
- code: `irSens01_entranceLB`

- config: `entranceLB`
- code: `control_entranceLB`

So client software should follow the **code**, not the config file.

### 3) The project is a TCP server, not a TCP client

`protocol.json` says:

```json
"mode": "tcp_client"
```

But the actual firmware opens `EthernetServerCompat server(80)` and waits for incoming connections.

### 4) Output reset behavior is incomplete

`update_outputs()` only writes a channel when its value is `1`.

That means the code currently does **not explicitly drive outputs low** when a command changes to `0`.

### 5) JSON receive buffering needs cleanup

`jsonRecvBuffer()` works like an early prototype and should be refactored:

- it shadows the variable `c`,
- it takes `EthernetClient` and `String` by value,
- it returns a constant string instead of the updated buffer,
- `main.cpp` ignores the returned value.

### 6) `serial_interface` is not integrated

The serial interface folder is currently just a commented example and is not used by the firmware.

### 7) There are a few code-style / portability issues

Examples include:

- `#include "host_interface\w5500_eth.h"` using a Windows-style backslash in `main.cpp`
- `#include "I2C_Driver.h"` while the actual file name is `i2c_driver.h`

Depending on toolchain / filesystem case sensitivity, these may need cleanup.

---

## Suggested next improvements

Good next steps for this project would be:

1. parse runtime configuration from `config/protocol.json` or move all config into constants,
2. unify JSON key naming across code and config,
3. fix newline buffering and JSON parsing flow,
4. add explicit output OFF handling,
5. document the expected external TCP client behavior,
6. add unit / integration tests for JSON parsing and I/O mapping,
7. add a wiring diagram and hardware photos for deployment.

---

## Development status

At the moment this repo is best described as:

- **working Ethernet + I/O prototype**
- **modularized enough to extend**
- **not yet production-hardened**

If you are using this project as the base for a field device, start by validating:

- SPI wiring to the W5500,
- I2C communication to the TCA9554,
- digital input polarity,
- client-side JSON message format,
- required relay ON/OFF behavior.

---

## License

No license file is currently included in this repository.

If you plan to share or reuse this project publicly, consider adding a license such as MIT, Apache-2.0, or GPL depending on your intended use.
