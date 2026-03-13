.. zephyr:code-sample:: dht22_stm32disco
   :name: DHT22 Temperature and Humidity Sensor Test for STM32F3 Discovery

   This sample demonstrates DHT22 sensor integration and data acquisition on the STM32F3 Discovery board.

Overview
********

This sample application tests the DHT22 temperature and humidity sensor connected to the STM32F3 Discovery board:

- Reads temperature and humidity data from the DHT22 sensor
- DHT22 sensor is connected to GPIO PA1 with internal pull-up resistor enabled
- Displays temperature and humidity values over UART serial output
- Periodically polls the sensor and logs readings to the console
- Demonstrates Zephyr RTOS features like GPIO control and sensor APIs

The application continuously reads sensor data and displays temperature (in Celsius) and humidity (in percentage) to the serial terminal.

Building and Running
********************

This application can be built and executed on the STM32F3 Discovery board as follows:

.. zephyr-app-commands::
   :zephyr-app: samples/led4_stm32disco
   :host-os: unix
   :board: stm32f3_disco
   :goals: build flash
   :compact:

Sample Output
=============

.. code-block:: console

    ================================================
    DHT22 Sensor - Temperature & Humidity Monitor
    STM32F3 Discovery Board
    ================================================

    Initializing DHT22 sensor on GPIO PA1 (with internal pull-up)...
    Waiting for sensor to stabilize (2 seconds)...
    DHT22 sensor initialized successfully!

    Reading sensor every 2 seconds...
    ================================================

    ===========================
    Temperature: 24.5°C
    Humidity: 65.3%
    ===========================
    ===========================
    Temperature: 24.6°C
    Humidity: 65.1%
    ===========================
    ===========================
    Temperature: 24.5°C
    Humidity: 65.4%
    ===========================

Hardware Setup
==============

**DHT22 Sensor Connection:**

- **Sensor Pin (Data):** Connected to GPIO PA1
- **Internal Pull-up:** Enabled (no external pull-up resistor required)
- **Power Supply:** 3.3V to 5V
- **Ground:** Connected to GND
- **Protocol:** Single-wire digital communication

**DHT22 Sensor Specifications:**

- Temperature Range: -40°C to 80°C
- Temperature Accuracy: ±0.5°C
- Humidity Range: 0% to 100% RH
- Humidity Accuracy: ±2%
- Sampling Rate: Once every 2 seconds minimum

**Wiring Diagram:**

.. code-block:: text

    DHT22          STM32F3 Discovery
    ----           -----------------
    VCC     ----> 3.3V
    DATA    ----> PA1 (GPIO with internal pull-up)
    GND     ----> GND

Code Implementation
*******************

The ``src/main.c`` file contains the main application logic:

**Key Components:**

1. **GPIO Initialization:** Configures GPIO PA1 with internal pull-up for DHT22 data communication
2. **Sensor Reading Function:** Implements the DHT22 communication protocol to read raw sensor data
3. **Data Parsing:** Extracts temperature and humidity values from the sensor response
4. **Serial Output:** Sends formatted temperature and humidity readings to the UART terminal
5. **Polling Loop:** Continuously reads the sensor at fixed intervals

**Main Function Flow:**

- Initialize UART for serial communication
- Configure GPIO PA1 with internal pull-up for DHT22 sensor control
- Wait 2 seconds for sensor to stabilize
- Enter main loop to periodically read sensor data
- Display temperature and humidity readings
- Wait for minimum 2-second interval before next read

Includes
========

.. code-block:: c

    #include <zephyr/kernel.h>
    #include <zephyr/drivers/gpio.h>
    #include <zephyr/drivers/sensor.h>

- ``<zephyr/kernel.h>``: Provides core Zephyr kernel APIs including ``k_sleep()`` for delays and ``printk()`` for console output.
- ``<zephyr/drivers/gpio.h>``: Contains GPIO driver APIs for pin configuration and control.
- ``<zephyr/drivers/sensor.h>``: Includes sensor driver APIs for reading accelerometer and magnetometer data.

Benefits: These headers enable hardware abstraction, allowing the code to work across different Zephyr-supported boards without modification.

Macros and Constants
====================

.. code-block:: c

    #define LED_NODE(n) DT_PATH(leds, led_##n)

- ``LED_NODE(n)``: A macro that constructs a device tree path to access LED nodes. For example, ``LED_NODE(3)`` becomes ``DT_PATH(leds, led_3)``.
- Benefits: Simplifies device tree access, makes the code more readable, and allows easy addition/removal of LEDs by changing the macro.

.. code-block:: c

    #define NUM_LEDS 8

- ``NUM_LEDS``: Defines the number of LEDs available on the board (8 for STM32F3 Discovery).
- Benefits: Centralizes configuration, making it easy to adapt the code for boards with different numbers of LEDs.

Global Variables
================

.. code-block:: c

    static const struct gpio_dt_spec leds[] = {
        GPIO_DT_SPEC_GET(LED_NODE(3), gpios),    // LD3 Red
        GPIO_DT_SPEC_GET(LED_NODE(4), gpios),    // LD4 Blue
        GPIO_DT_SPEC_GET(LED_NODE(5), gpios),    // LD5 Orange
        GPIO_DT_SPEC_GET(LED_NODE(6), gpios),    // LD6 Green
        GPIO_DT_SPEC_GET(LED_NODE(7), gpios),    // LD7 Green
        GPIO_DT_SPEC_GET(LED_NODE(8), gpios),    // LD8 Orange
        GPIO_DT_SPEC_GET(LED_NODE(9), gpios),    // LD9 Blue
        GPIO_DT_SPEC_GET(LED_NODE(10), gpios),   // LD10 Red
    };

- ``leds[]``: An array of GPIO device tree specifications for each LED. Each element contains pin information extracted from the device tree.
- Benefits: Device tree-based configuration ensures portability across different board revisions and allows the OS to manage pin multiplexing automatically.

.. code-block:: c

    static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

- ``button``: GPIO specification for the user button (SW0).
- Benefits: Uses device tree aliases for board-agnostic code.

.. code-block:: c

    static const struct device *accel = DEVICE_DT_GET(DT_ALIAS(accel0));
    static const struct device *magn = DEVICE_DT_GET(DT_ALIAS(magn0));

- ``accel`` and ``magn``: Device pointers for accelerometer and magnetometer sensors.
- Benefits: Device tree-based device discovery allows the code to work with different sensor configurations without recompilation.

Functions
=========

test_all_functions()
--------------------

This function performs a comprehensive test of all board peripherals.

UART Testing:

.. code-block:: c

    printk("Testing UART: Hello from STM32F3 Discovery Board!\n");

- Uses ``printk()`` to send a test message over UART0.
- Benefits: Verifies UART communication setup, useful for debugging and user feedback.

LED Testing:

.. code-block:: c

    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_pin_set_dt(&leds[i], 1);
        k_sleep(K_MSEC(200));
        gpio_pin_set_dt(&leds[i], 0);
    }

- Iterates through all LEDs, turning each on for 200ms then off.
- ``gpio_pin_set_dt()``: Sets the GPIO pin state using device tree information.
- Benefits: Tests GPIO functionality, provides visual feedback, and demonstrates sequential control.

Button Testing:

.. code-block:: c

    int btn_state = gpio_pin_get_dt(&button);

- Reads the current state of the button pin.
- Benefits: Demonstrates digital input reading, useful for user interaction.

Sensor Testing:

Accelerometer:

.. code-block:: c

    sensor_sample_fetch(accel);
    struct sensor_value val;
    sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &val);

- Fetches a new sample from the accelerometer and reads X, Y, Z acceleration values.
- Benefits: Shows how to interface with I2C sensors, provides motion sensing capabilities.

Magnetometer:

.. code-block:: c

    sensor_sample_fetch(magn);
    sensor_channel_get(magn, SENSOR_CHAN_MAGN_X, &val);

- Similar to accelerometer, reads magnetic field strength in X, Y, Z directions.
- Benefits: Enables compass functionality and magnetic field sensing.

Timer Testing:

.. code-block:: c

    k_sleep(K_SECONDS(1));

- Uses Zephyr's kernel sleep function for precise timing.
- Benefits: Demonstrates real-time scheduling capabilities, essential for embedded systems.

main() Function
===============

Device Initialization:

.. code-block:: c

    for (int i = 0; i < NUM_LEDS; i++) {
        if (!gpio_is_ready_dt(&leds[i])) {
            return -1;
        }
        gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
    }

- Checks if each LED GPIO is ready, then configures it as an output pin, initially inactive (off).
- Benefits: Ensures hardware is available before use, prevents runtime errors.

.. code-block:: c

    if (!gpio_is_ready_dt(&button)) {
        return -1;
    }
    gpio_pin_configure_dt(&button, GPIO_INPUT);

- Configures the button pin as input.
- Benefits: Sets up user input capability.

Test Execution:

.. code-block:: c

    test_all_functions();

- Runs the comprehensive test suite once at startup.
- Benefits: Verifies all peripherals are working before entering main operation.

LED Chase Loop:

.. code-block:: c

    int current_led = 0;
    while (1) {
        gpio_pin_set_dt(&leds[current_led], 1);
        k_sleep(K_MSEC(1000));
        gpio_pin_set_dt(&leds[current_led], 0);
        current_led = (current_led + 1) % NUM_LEDS;
    }

- Implements an infinite loop that cycles through LEDs, turning each on for 1 second then off.
- Benefits: Provides continuous visual indication of system operation, demonstrates real-time task execution.

Overall Benefits
================

- **Portability**: Device tree usage makes the code board-agnostic.
- **Modularity**: Separate functions for different tests allow easy expansion.
- **Real-time Operation**: Zephyr RTOS ensures predictable timing and scheduling.
- **Hardware Abstraction**: Driver APIs hide low-level details, improving maintainability.
- **Comprehensive Testing**: Covers GPIO, UART, sensors, and timing - useful for board bring-up and debugging.