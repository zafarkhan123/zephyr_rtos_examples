.. _nucleo_f401re_blinky_button:

NUCLEO-F401RE Blinky & Button
#############################

Overview
********
This sample application demonstrates basic GPIO input and output on the ST 
NUCLEO-F401RE board using Zephyr RTOS. It toggles the user LED (LD2) every 2 
seconds and prints the current state of the user button (B1) to the console.

Requirements
************
* ST NUCLEO-F401RE board
* Micro-USB cable for power and serial console

Building and Running
********************
Build the application using the following west command:

.. code-block:: console

   west build -b nucleo_f401re .

Flash the board:

.. code-block:: console

   west flash

Sample Output
*************
Connect a serial terminal at 115200 8N1 to see the output:

.. code-block:: console

   Button status: 0 | LED Toggled
   Button status: 1 | LED Toggled
