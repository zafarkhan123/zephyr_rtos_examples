/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * DHT22 Temperature and Humidity Sensor Application
 * STM32F3 Discovery Board
 *
 * Description: This application reads temperature and humidity data from a DHT22 sensor
 * connected to GPIO PA1 using manual bit-banging protocol implementation.
 * Outputs readings to UART serial port every 5 seconds.
 */

/* Include necessary Zephyr headers */
#include <zephyr/kernel.h>        /* Zephyr kernel API - timing, threading */
#include <zephyr/drivers/gpio.h>  /* GPIO driver for direct pin control */

/*
 * Global variables to store sensor readings
 * humidity: Stores the relative humidity in percentage (e.g., 65.3)
 * temperature: Stores the temperature in Celsius (e.g., 24.5)
 */
float humidity = 0.0f;
float temperature = 0.0f;

/*
 * Thread stack for DHT22 reading thread
 * Stack size: 1024 bytes (sufficient for GPIO operations and printing)
 */
#define DHT_STACK_SIZE 1024
K_THREAD_STACK_DEFINE(dht_stack, DHT_STACK_SIZE);

/*
 * Thread control block for DHT22 thread
 */
struct k_thread dht_thread_data;

/*
 * Device tree node for DHT22 sensor
 * DHT22_NODE: Reference to the DHT22 device tree alias
 * This must be defined in the device tree overlay
 */
#define DHT22_NODE DT_ALIAS(dht22)

/*
 * Compile-time check: Ensure DHT22 node exists and is enabled in device tree
 * If not, compilation will fail with an error message
 */
#if !DT_NODE_HAS_STATUS(DHT22_NODE, okay)
#error "DHT22 node is not okay - check device tree overlay"
#endif

/*
 * GPIO specification for DHT22 data pin
 * dht22: Structure containing GPIO port, pin number, and configuration
 * Retrieved from device tree using GPIO_DT_SPEC_GET macro
 * For STM32F3 Discovery, this points to PA1 with pull-up
 */
static const struct gpio_dt_spec dht22 =
    GPIO_DT_SPEC_GET(DHT22_NODE, dio_gpios);

/*
 * Function: dht22_start_signal()
 *
 * Purpose: Send the start signal to DHT22 sensor to initiate communication
 *
 * Description:
 * - Configure GPIO as output
 * - Pull data line LOW for 20ms (start signal)
 * - Pull HIGH for 30us (release)
 * - Switch to input mode to read sensor response
 *
 * Parameters: None
 * Return: void
 *
 * Notes:
 * - DHT22 protocol requires specific timing for initialization
 * - 20ms LOW pulse tells sensor to prepare data
 * - 30us HIGH pulse allows sensor to respond
 */
void dht22_start_signal(void) {
    /* Configure GPIO pin as output to control the data line */
    gpio_pin_configure_dt(&dht22, GPIO_OUTPUT);

    /* Pull data line LOW to send start signal */
    gpio_pin_set_dt(&dht22, 0);

    /* Hold LOW for 20ms (minimum 18ms required by DHT22) */
    k_msleep(20);

    /* Pull data line HIGH to release */
    gpio_pin_set_dt(&dht22, 1);

    /* Wait 30us for sensor to respond */
    k_busy_wait(30);

    /* Switch GPIO to input mode to read sensor data */
    gpio_pin_configure_dt(&dht22, GPIO_INPUT);
}

/*
 * Function: dht22_wait_for(int level, uint32_t timeout_us)
 *
 * Purpose: Wait for the GPIO pin to reach a specific level (HIGH or LOW)
 *
 * Description:
 * - Poll the GPIO pin until it matches the desired level
 * - Use busy waiting with 1us resolution
 * - Timeout after specified microseconds to prevent infinite loop
 *
 * Parameters:
 * - level: Desired GPIO level (0 = LOW, 1 = HIGH)
 * - timeout_us: Maximum time to wait in microseconds
 *
 * Return:
 * - 0 on success (level reached within timeout)
 * - -1 on timeout (level not reached)
 *
 * Notes:
 * - Used during sensor response and data bit reading
 * - DHT22 timing is critical, so timeout prevents hanging
 * - Increased timeout for better reliability
 */
int dht22_wait_for(int level, uint32_t timeout_us) {
    /* Counter for timeout tracking */
    uint32_t count = 0;

    /* Loop until GPIO reaches desired level or timeout */
    while (gpio_pin_get_dt(&dht22) != level) {
        /* Increment counter */
        if (count++ > timeout_us) {
            /* Timeout exceeded - return error */
            return -1;
        }
        /* Wait 1 microsecond before checking again */
        k_busy_wait(1);
    }
    /* Success - level reached within timeout */
    return 0;
}

/*
 * Function: dht22_read_bit(void)
 *
 * Purpose: Read a single bit from DHT22 sensor
 *
 * Description:
 * - Wait for HIGH pulse start (bit transmission begins)
 * - Wait 40us to sample bit value (DHT22 timing)
 * - Read GPIO level at 40us mark
 * - Wait for LOW pulse end
 *
 * Parameters: None
 *
 * Return:
 * - 0 for LOW bit (0)
 * - 1 for HIGH bit (1)
 * - -1 on timeout/error
 *
 * Notes:
 * - DHT22 sends bits as pulses: short LOW = 0, long LOW = 1
 * - 40us delay positions us at the middle of the bit pulse
 */
int dht22_read_bit(void) {
    /* Wait for start of bit pulse (GPIO goes HIGH) */
    if (dht22_wait_for(1, 500))
        return -1;

    /* Wait 40us to sample the bit value */
    k_busy_wait(40);

    /* Read the bit value at this point */
    int bit = gpio_pin_get_dt(&dht22);

    /* Wait for end of bit pulse (GPIO goes LOW) */
    if (dht22_wait_for(0, 500))
        return -1;

    /* Return the bit value (0 or 1) */
    return bit;
}

/*
 * Function: dht22_read(uint8_t data[5])
 *
 * Purpose: Read complete 40-bit data packet from DHT22 sensor
 *
 * Description:
 * - Send start signal to sensor
 * - Read sensor acknowledgment pulses
 * - Read 40 data bits (5 bytes)
 * - Perform checksum validation
 * - Disable interrupts during timing-critical sections to prevent preemption
 *
 * Parameters:
 * - data: Array to store the 5 bytes of sensor data
 *
 * Return:
 * - 0 on success
 * - -1 on communication timeout
 * - -2 on checksum failure
 *
 * Notes:
 * - Data format: [humidity_H, humidity_L, temp_H, temp_L, checksum]
 * - Checksum = sum of first 4 bytes
 * - Interrupts are locked during the entire read to maintain precise timing
 * - Total read time: ~22ms, during which interrupts are disabled
 */
int dht22_read(uint8_t data[5]) {
    /* Disable interrupts for the entire read operation to ensure precise timing */
    unsigned int key = irq_lock();

    /* Send start signal to initiate communication */
    dht22_start_signal();

    /* Wait for sensor to pull LOW (acknowledgment start) */
    if (dht22_wait_for(0, 500)) {
        irq_unlock(key);
        return -1;
    }

    /* Wait for sensor to pull HIGH (acknowledgment end) */
    if (dht22_wait_for(1, 500)) {
        irq_unlock(key);
        return -1;
    }

    /* Wait for sensor to pull LOW (data transmission start) */
    if (dht22_wait_for(0, 500)) {
        irq_unlock(key);
        return -1;
    }

    /* Read 40 bits (5 bytes) of data */
    for (int i = 0; i < 40; i++) {
        /* Read one bit */
        int bit = dht22_read_bit();
        if (bit < 0) {
            /* Re-enable interrupts before returning */
            irq_unlock(key);
            return -1;
        }

        /* Shift current byte left and add new bit */
        data[i / 8] <<= 1;
        if (bit)
            data[i / 8] |= 1;
    }

    /* Re-enable interrupts */
    irq_unlock(key);

    /* Calculate checksum: sum of first 4 bytes */
    uint8_t sum = data[0] + data[1] + data[2] + data[3];

    /* Verify checksum matches the 5th byte */
    if (sum != data[4])
        return -2;  /* Checksum error */

    /* Success */
    return 0;
}

/*
 * Function: read_dht22(void)
 *
 * Purpose: Read and process DHT22 sensor data
 *
 * Description:
 * - Call dht22_read to get raw data
 * - Convert raw values to temperature and humidity
 * - Handle negative temperatures (bit 15 set)
 * - Print formatted output to UART
 *
 * Parameters: None
 * Return: void
 *
 * Notes:
 * - Raw humidity: 16-bit value, multiply by 0.1 for percentage
 * - Raw temperature: 16-bit value, multiply by 0.1 for Celsius
 * - Bit 15 of temperature indicates negative value
 */
void read_dht22(void) {
    /* Array to store raw sensor data (5 bytes) */
    uint8_t data[5];

    /* Attempt to read data from sensor */
    int ret = dht22_read(data);
    if (ret == 0) {
        /* Extract raw humidity (16-bit, big-endian) */
        uint16_t raw_hum = (data[0] << 8) | data[1];

        /* Extract raw temperature (16-bit, big-endian) */
        uint16_t raw_temp = (data[2] << 8) | data[3];

        /* Convert to float values (multiply by 0.1) */
        humidity = raw_hum * 0.1f;
        temperature = raw_temp * 0.1f;

        /* Check if temperature is negative (bit 15 set) */
        if (raw_temp & 0x8000) {
            temperature = -temperature;  /* Make negative */
        }

        /* Print formatted output to UART */
        printk("Temperature: %.1f°C, Humidity: %.1f%%\n", (double)temperature, (double)humidity);
    } else {
        /* Print error message based on return code */
        if (ret == -1) {
            printk("ERROR: DHT22 communication timeout\n");
        } else if (ret == -2) {
            printk("ERROR: DHT22 checksum failed\n");
        }
    }
}

/*
 * Thread function: dht_thread()
 *
 * Purpose: Dedicated thread for DHT22 sensor reading
 *
 * Description:
 * - Runs in a separate thread with high priority to minimize preemption
 * - Continuously reads DHT22 sensor every 5 seconds
 * - Calls read_dht22() to perform the actual reading and printing
 *
 * Parameters:
 * - p1, p2, p3: Unused thread parameters (standard Zephyr signature)
 *
 * Return: void
 *
 * Notes:
 * - Thread priority: 5 (higher than default main thread priority 0)
 * - Prevents timing disruption from lower-priority tasks
 */
void dht_thread(void *p1, void *p2, void *p3) {
    /* Infinite loop for continuous sensor reading */
    while (1) {
        /* Read and display sensor data */
        read_dht22();

        /* Wait 10 seconds before next reading */
        k_sleep(K_SECONDS(10));
    }
}

/*
 * Main Function: Application Entry Point
 *
 * Purpose: Initialize GPIO and start DHT22 reading thread
 *
 * Execution Flow:
 * 1. Check GPIO readiness
 * 2. Display startup messages
 * 3. Create and start high-priority DHT22 reading thread
 * 4. Main thread exits (or can do other tasks)
 *
 * Return: 0 on success, non-zero on initialization failure
 */
int main(void) {
    /* Display startup banner */
    printk("\n================================================\n");
    printk("DHT22 Sensor - Temperature & Humidity Monitor\n");
    printk("STM32F3 Discovery Board (PA1)\n");
    printk("================================================\n\n");

    /* Check if GPIO device is ready */
    if (!gpio_is_ready_dt(&dht22)) {
        printk("ERROR: GPIO not ready\n");
        return -1;
    }

    /* Initialization successful */
    printk("DHT22 sensor initialized on GPIO PA1\n");
    printk("Starting DHT22 reading thread (every 10 seconds)...\n\n");

    /* Create and start the DHT22 reading thread */
    k_thread_create(&dht_thread_data, dht_stack, DHT_STACK_SIZE,
                    dht_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);  /* Priority 5, start immediately */

    /* Main thread can exit or perform other tasks */
    return 0;
}
