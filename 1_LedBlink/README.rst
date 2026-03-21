.. zephyr:code-sample:: led_blink_stm32f3_disco
   :name: LED Sequential Blink — STM32F3 Discovery

   Blinks 8 LEDs in a sequential color pattern (Red → Blue → Green → Orange) on the STM32F3 Discovery board.

Overview
********

This sample demonstrates GPIO output control using the Zephyr device tree API
on the STM32F3 Discovery board. Eight on-board LEDs arranged around the board
are driven in pairs by color, cycling through four colors sequentially with a
1500 ms interval between each step.

LED mapping (device tree node labels)
======================================

+----------+--------------------+--------------------+
| Color    | LED 1 node label   | LED 2 node label   |
+==========+====================+====================+
| Red      | red_led_3          | red_led_10         |
+----------+--------------------+--------------------+
| Orange   | orange_led_5       | orange_led_8       |
+----------+--------------------+--------------------+
| Green    | green_led_7        | green_led_6        |
+----------+--------------------+--------------------+
| Blue     | blue_led_9         | blue_led_4         |
+----------+--------------------+--------------------+

Blink sequence
==============

Each step lasts **1500 ms**:

1. Red pair ON — all others OFF
2. Blue pair ON — all others OFF
3. Green pair ON — all others OFF
4. Orange pair ON — all others OFF
5. Repeat

Building and Running
********************

Build and flash for the STM32F3 Discovery board:

.. zephyr-app-commands::
   :zephyr-app: samples/basic/led_blink_stm32f3_disco
   :host-os: unix
   :board: stm32f3_disco
   :goals: build flash
   :compact:

To build on Windows using west:

.. code-block:: bat

   west build -b stm32f3_disco
   west flash

Requirements
============

- STM32F3 Discovery board
- Zephyr SDK and west build tool installed
- ``CONFIG_DEBUG=y`` is enabled in ``prj.conf``

Sample Output
=============

No console output is produced by default. The expected behavior is visible
on the board:

.. code-block:: none

    [Red LEDs ON]    ... 1500 ms
    [Blue LEDs ON]   ... 1500 ms
    [Green LEDs ON]  ... 1500 ms
    [Orange LEDs ON] ... 1500 ms
    (repeats)
