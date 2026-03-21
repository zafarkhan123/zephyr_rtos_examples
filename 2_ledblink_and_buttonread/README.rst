.. zephyr:code-sample:: led4_stm32disco
   :name: LED and Sensor Test for STM32F3 Discovery

   This sample demonstrates CLI and sequential LED blinking, UART communication, button input, and sensor readings (accelerometer and magnetometer) on the STM32F3 Discovery board.

Overview
********

This sample application showcases the capabilities of the STM32F3 Discovery board by:

- Blinking LEDs in a sequential chase pattern
- Printing status messages over UART0
- Reading button input
- Fetching data from onboard sensors (accelerometer and magnetometer)
- Demonstrating Zephyr RTOS features like device tree usage, GPIO control, and sensor APIs

The application first runs a comprehensive test of all board functions, then enters an infinite loop where LEDs are toggled sequentially every second.

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
    Available commands: led_on, led_off, led_blink, led_all_on, led_all_off, button_status, sensor_accel, sensor_magn, test_all

    uart:~$ led_on 0
    LED 0 turned on
    uart:~$ sensor_accel
    Accel X: 0.000000 m/s²
    Accel Y: 0.000000 m/s²
    Accel Z: 9.810000 m/s²
    uart:~$

Command Line Interface
======================

This sample includes a shell-based CLI for interactive control of the board's peripherals. After the initial test sequence, the application enters shell mode where you can execute commands to control LEDs, read sensors, and check button status.

Available Commands
------------------

LED Control:

- ``led_on <num>``: Turn on LED number <num> (0-7)
- ``led_off <num>``: Turn off LED number <num> (0-7)
- ``led_blink <num> <times>``: Blink LED <num> <times> times (500ms on/off each)
- ``led_all_on``: Turn all LEDs on
- ``led_all_off``: Turn all LEDs off

Sensor Reading:

- ``sensor_accel``: Read and display accelerometer values (X, Y, Z axes)
- ``sensor_magn``: Read and display magnetometer values (X, Y, Z axes)

Input/Status:

- ``button_status``: Check if the user button is pressed
- ``test_all``: Re-run the initial comprehensive test sequence

Benefits of CLI Implementation:

- **Interactive Control**: Allows real-time control and testing of hardware without code changes
- **Debugging Aid**: Useful for troubleshooting hardware issues
- **Educational**: Demonstrates shell integration in Zephyr
- **Extensible**: Easy to add new commands for additional features

Code Explanation
****************

This section provides a detailed breakdown of the code in ``src/main.c``, explaining each component, its purpose, implementation, and benefits.

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