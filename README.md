# Zephyr RTOS Examples

This repository contains example programs built and verified on STM32 boards using Zephyr RTOS.

| Board | MCU |
|-------|-----|
| STM32F3 Discovery (`stm32f3_disco`) | STM32F303VCT6 |
| NUCLEO-F401RE (`nucleo_f401re`) | STM32F401RE |

---

## Examples

| # | Folder | Board | Description |
|---|--------|-------|-------------|
| 1 | [1_LedBlink](#1-1_ledblink) | STM32F3 Discovery | Sequential LED color chase across 8 onboard LEDs |
| 2 | [2_ledblink_and_buttonread](#2-2_ledblink_and_buttonread) | STM32F3 Discovery | LED chase, button read, onboard sensors, and interactive shell CLI |
| 3 | [3_DHT22](#3-3_dht22) | STM32F3 Discovery | DHT22 temperature and humidity sensor via bit-bang protocol |
| 4 | [4_OLED_SSD1306](#4-4_oled_ssd1306) | NUCLEO-F401RE | SSD1306 OLED display (128x64) over I2C using the CFB API |

---

## 1. 1_LedBlink

**Board:** STM32F3 Discovery

### Overview

Demonstrates GPIO output control using the Zephyr device tree API. Eight onboard LEDs are driven in color pairs, cycling through four colors sequentially with a 1500 ms interval between each step.

### LED Mapping

| Color | LED 1 | LED 2 |
|-------|-------|-------|
| Red | LD3 | LD10 |
| Orange | LD5 | LD8 |
| Green | LD7 | LD6 |
| Blue | LD9 | LD4 |

### Blink Sequence

Each step lasts **1500 ms**:

1. Red pair ON - all others OFF
2. Blue pair ON - all others OFF
3. Green pair ON - all others OFF
4. Orange pair ON - all others OFF
5. Repeat

### Build and Flash

```bash
cd 1_LedBlink
west build -b stm32f3_disco
west flash
```

### Sample Output

No console output - expected behavior is visible on the board:

```
[Red LEDs ON]    ... 1500 ms
[Blue LEDs ON]   ... 1500 ms
[Green LEDs ON]  ... 1500 ms
[Orange LEDs ON] ... 1500 ms
(repeats)
```

---

## 2. 2_ledblink_and_buttonread

**Board:** STM32F3 Discovery

### Overview

Demonstrates core STM32F3 Discovery peripherals using Zephyr RTOS:

- Sequential LED chase pattern across 8 onboard LEDs (LD3-LD10)
- User button state reading (SW0)
- Onboard accelerometer (LSM303DLHC) and magnetometer readings
- UART serial output via printk
- Interactive Zephyr Shell CLI for real-time hardware control

On startup, the application runs a full peripheral test, then enters shell mode where all hardware can be controlled interactively via typed commands over UART.

### CLI Implementation

Uses the Zephyr Shell subsystem to expose a command-line interface over UART. Commands are registered with SHELL_CMD_REGISTER and become available at the uart prompt once the startup test sequence completes.

Open a serial terminal connected to the ST-Link virtual COM port at **115200 baud** to use the CLI.

### Hardware

| Peripheral | Detail |
|------------|--------|
| LEDs | LD3 (Red), LD4 (Blue), LD5 (Orange), LD6 (Green), LD7 (Green), LD8 (Orange), LD9 (Blue), LD10 (Red) |
| Button | SW0 - user button |
| Accelerometer | LSM303DLHC via I2C |
| Magnetometer | LSM303DLHC via I2C |

### CLI Commands

**LED Control:**

| Command | Description |
|---------|-------------|
| led_on NUM | Turn on LED 0-7 |
| led_off NUM | Turn off LED 0-7 |
| led_blink NUM TIMES | Blink LED N times (500ms on/off) |
| led_all_on | Turn all 8 LEDs on |
| led_all_off | Turn all 8 LEDs off |

**Sensors and Input:**

| Command | Description |
|---------|-------------|
| button_status | Check if user button is pressed |
| sensor_accel | Read accelerometer X/Y/Z |
| sensor_magn | Read magnetometer X/Y/Z |
| test_all | Re-run the full peripheral test sequence |

### Build and Flash

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
Accel X: 0.000000 m/s2
Accel Y: 0.000000 m/s2
Accel Z: 9.810000 m/s2
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
Accel X: 0.000000 m/s2
Accel Y: 0.000000 m/s2
Accel Z: 9.810000 m/s2
```

---

## 3. 3_DHT22

**Board:** STM32F3 Discovery

### Overview

Reads temperature and humidity from a **DHT22** sensor connected to GPIO PA1 using a manual **bit-bang** implementation of the DHT22 single-wire protocol. Readings are printed to UART every 10 seconds from a dedicated high-priority Zephyr thread.

Key implementation details:
- Manual bit-bang: pulls data line LOW for 20ms to start, then reads 40-bit response
- Interrupts are disabled during the read for precise microsecond timing
- Checksum validation on every read
- Dedicated Zephyr thread (priority 5) to prevent timing disruption

### Hardware Setup

**Wiring:**

| DHT22 Pin | STM32F3 Discovery |
|-----------|-------------------|
| VCC | 3.3V |
| DATA | PA1 (internal pull-up enabled) |
| GND | GND |

No external pull-up resistor is needed - the internal pull-up is configured in the device tree overlay.

**DHT22 Specs:**

| Parameter | Value |
|-----------|-------|
| Temperature range | -40C to 80C |
| Temperature accuracy | +/-0.5C |
| Humidity range | 0% to 100% RH |
| Humidity accuracy | +/-2% RH |
| Min sampling interval | 2 seconds |

### Build and Flash

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

Temperature: 24.5C, Humidity: 65.3%
Temperature: 24.6C, Humidity: 65.1%
Temperature: 24.5C, Humidity: 65.4%
```

---

## 4. 4_OLED_SSD1306

**Board:** NUCLEO-F401RE

### Overview

Drives a **128x64 SSD1306 OLED display** over I2C using Zephyr's **Character Frame Buffer (CFB)** API. On startup, the display is initialized and three lines of text are rendered:

```
Hello, World!
Zephyr RTOS
STM32 F401RE
```

The application then idles - the content remains on screen indefinitely.

### Hardware Setup

**Wiring:**

| SSD1306 Pin | NUCLEO-F401RE |
|-------------|---------------|
| VCC | 3.3V |
| GND | GND |
| SCL | PB8 (I2C1 SCL) |
| SDA | PB9 (I2C1 SDA) |

The I2C lines are configured with open-drain output and pull-up bias in the device tree overlay. No external pull-up resistors are required.

SSD1306 I2C address: 0x3C

**Display Specs:**

| Parameter | Value |
|-----------|-------|
| Resolution | 128 x 64 pixels |
| Interface | I2C (standard mode, 100 kHz) |
| Driver IC | Solomon SSD1306 |
| Supply voltage | 3.3V |

### Key Zephyr Configs (prj.conf)

```
CONFIG_I2C=y
CONFIG_DISPLAY=y
CONFIG_SSD1306=y
CONFIG_CHARACTER_FRAMEBUFFER=y
CONFIG_HEAP_MEM_POOL_SIZE=16384
CONFIG_MAIN_STACK_SIZE=2048
```

### Build and Flash

```bash
cd 4_OLED_SSD1306
west build -b nucleo_f401re
west flash
```

### Sample Output (UART console)

```
Display initialized successfully!
```

Text displayed on the OLED screen:

```
Hello, World!
Zephyr RTOS
STM32 F401RE
```

---

## Development Environment

- Zephyr RTOS with west build tool
- ST-Link (onboard) for flashing and UART output
- Serial terminal at **115200 baud** for UART output:
  - PuTTY (Windows/Linux)
  - Tera Term (Windows)
  - minicom (Linux/macOS)

## Getting Started

```bash
git clone https://github.com/zafarkhan123/zephyr_rtos_examples.git

cd zephyr_rtos_examples/1_LedBlink
west build -b stm32f3_disco
west flash
```
