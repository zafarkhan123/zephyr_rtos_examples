# Zephyr RTOS Examples — STM32F3 Discovery

This repository contains example programs built and verified on the **STM32F3 Discovery** board using Zephyr RTOS.

**Board:** STM32F3 Discovery (stm32f3_disco)
**MCU:** STM32F303VCT6
**RTOS:** Zephyr

---

## Examples

| # | Folder | Description |
|---|--------|-------------|
| 1 | [`1_LedBlink`](#1-1_ledblink) | Sequential LED color blink across 8 onboard LEDs |
| 2 | [`2_ledblink_and_buttonread`](#2-2_ledblink_and_buttonread) | LED chase, button read, onboard sensors, and interactive shell CLI |
| 3 | [`3_DHT22`](#3-3_dht22) | DHT22 temperature and humidity sensor via bit-bang protocol |

---

## 1. `1_LedBlink`

### Overview

A simple Zephyr RTOS application that blinks 8 onboard LEDs on the STM32F3 Discovery board in a sequential color pattern using the GPIO device tree API.

- 8 LEDs controlled in 4 color pairs: Red, Blue, Green, Orange
- Each color pair turns ON for **1500 ms** while all others remain OFF
- Cycles continuously: Red → Blue → Green → Orange → repeat

### LED Mapping

| Color  | LED 1 Node Label | LED 2 Node Label |
|--------|-----------------|-----------------|
| Red    | red_led_3       | red_led_10      |
| Orange | orange_led_5    | orange_led_8    |
| Green  | green_led_7     | green_led_6     |
| Blue   | blue_led_9      | blue_led_4      |

### Build & Flash

```bash
cd 1_LedBlink
west build -b stm32f3_disco
west flash
```

### Sample Output

No UART output. Observable behavior on the board:

```
[Red LEDs ON]    ... 1500 ms
[Blue LEDs ON]   ... 1500 ms
[Green LEDs ON]  ... 1500 ms
[Orange LEDs ON] ... 1500 ms
(repeats)
```

---

## 2. `2_ledblink_and_buttonread`

### Overview

Demonstrates core STM32F3 Discovery peripherals using Zephyr RTOS:

- Sequential LED chase pattern across 8 onboard LEDs (LD3–LD10)
- User button state reading (SW0)
- Onboard accelerometer (LSM303DLHC) and magnetometer readings
- UART serial output via `printk`
- Interactive **Zephyr Shell CLI** for real-time hardware control

On startup, the application runs a full peripheral test, then enters shell mode where all hardware can be controlled interactively via typed commands over UART.

### CLI Implementation

This example uses the **Zephyr Shell subsystem** (`zephyr/shell/shell.h`) to expose a command-line interface over UART. Commands are registered with `SHELL_CMD_REGISTER` and become available at the `uart:~$` prompt once the startup test sequence completes.

To use the CLI, open a serial terminal connected to the ST-Link virtual COM port at **115200 baud**, then type any of the commands listed below.

### Hardware

| Peripheral | Detail |
|------------|--------|
| LEDs | LD3 (Red), LD4 (Blue), LD5 (Orange), LD6 (Green), LD7 (Green), LD8 (Orange), LD9 (Blue), LD10 (Red) |
| Button | SW0 — user button |
| Accelerometer | LSM303DLHC via I2C (alias: `accel0`) |
| Magnetometer | LSM303DLHC via I2C (alias: `magn0`) |

### CLI Commands

After startup, the Zephyr shell is available over UART (`uart:~$`):

**LED Control:**

| Command | Description |
|---------|-------------|
| `led_on <num>` | Turn on LED 0–7 |
| `led_off <num>` | Turn off LED 0–7 |
| `led_blink <num> <times>` | Blink LED N times (500 ms on/off) |
| `led_all_on` | Turn all 8 LEDs on |
| `led_all_off` | Turn all 8 LEDs off |

**Sensors & Input:**

| Command | Description |
|---------|-------------|
| `button_status` | Check if user button is pressed |
| `sensor_accel` | Read accelerometer X/Y/Z (m/s²) |
| `sensor_magn` | Read magnetometer X/Y/Z (Gauss) |
| `test_all` | Re-run the full peripheral test sequence |

### Build & Flash

```bash
cd 2_ledblink_and_buttonread
west build -b stm32f3_disco
west flash
```

### Sample Output

```
STM32F3 Discovery - Testing Functions!
Testing UART: Hello from STM32F3 Discovery Board!
Testing LEDs...
LED test completed.
Testing button...
Button is not pressed.
Testing accelerometer...
Accel X: 0.000000 m/s²
Accel Y: 0.000000 m/s²
Accel Z: 9.810000 m/s²
Testing magnetometer...
Magn X: 0.000000 Gauss
Magn Y: 0.000000 Gauss
Magn Z: 0.000000 Gauss
Testing timer delay...
Delay test passed.
All tests completed.
CLI is now available. Use shell commands to control LEDs and sensors.

uart:~$ led_on 0
LED 0 turned on
uart:~$ sensor_accel
Accel X: 0.000000 m/s²
Accel Y: 0.000000 m/s²
Accel Z: 9.810000 m/s²
```

---

## 3. `3_DHT22`

### Overview

Reads temperature and humidity from a **DHT22** sensor connected to GPIO PA1 using a manual **bit-bang** implementation of the DHT22 single-wire protocol. Readings are printed to UART every 10 seconds from a dedicated high-priority Zephyr thread.

Key implementation details:
- Manual bit-bang: pulls data line LOW for 20 ms to start, then reads 40-bit response
- Interrupts are disabled during the read for precise microsecond timing
- Checksum validation on every read
- Dedicated Zephyr thread (priority 5) to prevent timing disruption

### Hardware Setup

**Wiring:**

| DHT22 Pin | STM32F3 Discovery |
|-----------|-------------------|
| VCC       | 3.3V              |
| DATA      | PA1 (internal pull-up enabled) |
| GND       | GND               |

No external pull-up resistor is needed — the internal pull-up is configured in the device tree overlay (`stm32f3_disco.overlay`).

**DHT22 Specs:**

| Parameter | Value |
|-----------|-------|
| Temperature range | -40°C to 80°C |
| Temperature accuracy | ±0.5°C |
| Humidity range | 0% to 100% RH |
| Humidity accuracy | ±2% RH |
| Min sampling interval | 2 seconds |

### Build & Flash

```bash
cd 3_DHT22
west build -b stm32f3_disco
west flash
```

### Sample Output

```
================================================
DHT22 Sensor - Temperature & Humidity Monitor
STM32F3 Discovery Board (PA1)
================================================

DHT22 sensor initialized on GPIO PA1
Starting DHT22 reading thread (every 10 seconds)...

Temperature: 24.5°C, Humidity: 65.3%
Temperature: 24.6°C, Humidity: 65.1%
Temperature: 24.5°C, Humidity: 65.4%
```

---

## Development Environment

- [Zephyr RTOS](https://zephyrproject.org/) with `west` build tool
- ST-Link (onboard) for flashing and UART output
- Serial terminal at **115200 baud** for UART output — options include:
  - [PuTTY](https://www.putty.org/) (Windows/Linux)
  - [Tera Term](https://teratermproject.github.io/) (Windows)
  - minicom (Linux/macOS)

## Getting Started

```bash
# Clone the repo
git clone https://github.com/zafarkhan123/zephyr_rtos_examples.git

# Navigate to an example and build
cd zephyr_rtos_examples/1_LedBlink
west build -b stm32f3_disco
west flash
```
