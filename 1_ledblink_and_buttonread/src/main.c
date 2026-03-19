#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>

// Use DT_PATH to access LEDs inside the /leds node
#define LED_NODE(n) DT_PATH(leds, led_##n)

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

#define NUM_LEDS 8

// Button
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

// Sensors
static const struct device *accel = DEVICE_DT_GET(DT_ALIAS(accel0));
static const struct device *magn = DEVICE_DT_GET(DT_ALIAS(magn0));

void test_all_functions(void) {
    // Test UART output
    printk("Testing UART: Hello from STM32F3 Discovery Board!\n");

    // Test LEDs: blink each one
    printk("Testing LEDs...\n");
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_pin_set_dt(&leds[i], 1);
        k_sleep(K_MSEC(400));
        gpio_pin_set_dt(&leds[i], 0);
    }
    printk("LED test completed.\n");

    // Test button input
    printk("Testing button...\n");
    int btn_state = gpio_pin_get_dt(&button);
    if (btn_state) {
        printk("Button is pressed.\n");
    } else {
        printk("Button is not pressed.\n");
    }

    // Test accelerometer
    if (!device_is_ready(accel)) {
        printk("Accelerometer not ready\n");
    } else {
        printk("Testing accelerometer...\n");
        sensor_sample_fetch(accel);
        struct sensor_value val;
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &val);
        printk("Accel X: %d.%06d m/s²\n", val.val1, val.val2);
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &val);
        printk("Accel Y: %d.%06d m/s²\n", val.val1, val.val2);
        sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &val);
        printk("Accel Z: %d.%06d m/s²\n", val.val1, val.val2);
    }

    // Test magnetometer (e-compass)
    if (!device_is_ready(magn)) {
        printk("Magnetometer not ready\n");
    } else {
        printk("Testing magnetometer...\n");
        sensor_sample_fetch(magn);
        struct sensor_value val;
        sensor_channel_get(magn, SENSOR_CHAN_MAGN_X, &val);
        printk("Magn X: %d.%06d Gauss\n", val.val1, val.val2);
        sensor_channel_get(magn, SENSOR_CHAN_MAGN_Y, &val);
        printk("Magn Y: %d.%06d Gauss\n", val.val1, val.val2);
        sensor_channel_get(magn, SENSOR_CHAN_MAGN_Z, &val);
        printk("Magn Z: %d.%06d Gauss\n", val.val1, val.val2);
    }

    // Note: Gyroscope (L3GD20) is not configured in this Zephyr board setup
    // To add it, enable in board dts and prj.conf

    // Test timer/delay
    printk("Testing timer delay...\n");
    k_sleep(K_SECONDS(1));
    printk("Delay test passed.\n");

    // Additional tests can be added here (ADC, SPI, etc.)
    printk("All tests completed.\n");
}

/* Shell command handlers */

static int cmd_led_on(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_print(shell, "Usage: led on <num>");
        return -EINVAL;
    }
    int num = atoi(argv[1]);
    if (num < 0 || num >= NUM_LEDS) {
        shell_print(shell, "Invalid LED number (0-%d)", NUM_LEDS - 1);
        return -EINVAL;
    }
    gpio_pin_set_dt(&leds[num], 1);
    shell_print(shell, "LED %d turned on", num);
    return 0;
}

static int cmd_led_off(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_print(shell, "Usage: led off <num>");
        return -EINVAL;
    }
    int num = atoi(argv[1]);
    if (num < 0 || num >= NUM_LEDS) {
        shell_print(shell, "Invalid LED number (0-%d)", NUM_LEDS - 1);
        return -EINVAL;
    }
    gpio_pin_set_dt(&leds[num], 0);
    shell_print(shell, "LED %d turned off", num);
    return 0;
}

static int cmd_led_blink(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 3) {
        shell_print(shell, "Usage: led blink <num> <times>");
        return -EINVAL;
    }
    int num = atoi(argv[1]);
    int times = atoi(argv[2]);
    if (num < 0 || num >= NUM_LEDS) {
        shell_print(shell, "Invalid LED number (0-%d)", NUM_LEDS - 1);
        return -EINVAL;
    }
    if (times < 1) {
        shell_print(shell, "Invalid blink times");
        return -EINVAL;
    }
    for (int i = 0; i < times; i++) {
        gpio_pin_set_dt(&leds[num], 1);
        k_sleep(K_MSEC(500));
        gpio_pin_set_dt(&leds[num], 0);
        k_sleep(K_MSEC(500));
    }
    shell_print(shell, "LED %d blinked %d times", num, times);
    return 0;
}

static int cmd_led_all_on(const struct shell *shell, size_t argc, char **argv)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_pin_set_dt(&leds[i], 1);
    }
    shell_print(shell, "All LEDs turned on");
    return 0;
}

static int cmd_led_all_off(const struct shell *shell, size_t argc, char **argv)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_pin_set_dt(&leds[i], 0);
    }
    shell_print(shell, "All LEDs turned off");
    return 0;
}

static int cmd_button_status(const struct shell *shell, size_t argc, char **argv)
{
    int btn_state = gpio_pin_get_dt(&button);
    shell_print(shell, "Button is %s", btn_state ? "pressed" : "not pressed");
    return 0;
}

static int cmd_sensor_accel(const struct shell *shell, size_t argc, char **argv)
{
    if (!device_is_ready(accel)) {
        shell_print(shell, "Accelerometer not ready");
        return -EIO;
    }
    sensor_sample_fetch(accel);
    struct sensor_value val;
    sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &val);
    shell_print(shell, "Accel X: %d.%06d m/s²", val.val1, val.val2);
    sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &val);
    shell_print(shell, "Accel Y: %d.%06d m/s²", val.val1, val.val2);
    sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &val);
    shell_print(shell, "Accel Z: %d.%06d m/s²", val.val1, val.val2);
    return 0;
}

static int cmd_sensor_magn(const struct shell *shell, size_t argc, char **argv)
{
    if (!device_is_ready(magn)) {
        shell_print(shell, "Magnetometer not ready");
        return -EIO;
    }
    sensor_sample_fetch(magn);
    struct sensor_value val;
    sensor_channel_get(magn, SENSOR_CHAN_MAGN_X, &val);
    shell_print(shell, "Magn X: %d.%06d Gauss", val.val1, val.val2);
    sensor_channel_get(magn, SENSOR_CHAN_MAGN_Y, &val);
    shell_print(shell, "Magn Y: %d.%06d Gauss", val.val1, val.val2);
    sensor_channel_get(magn, SENSOR_CHAN_MAGN_Z, &val);
    shell_print(shell, "Magn Z: %d.%06d Gauss", val.val1, val.val2);
    return 0;
}

static int cmd_test_all(const struct shell *shell, size_t argc, char **argv)
{
    test_all_functions();
    return 0;
}

/* Register shell commands */
SHELL_CMD_REGISTER(led_on, NULL, "Turn LED on: led on <num>", cmd_led_on);
SHELL_CMD_REGISTER(led_off, NULL, "Turn LED off: led off <num>", cmd_led_off);
SHELL_CMD_REGISTER(led_blink, NULL, "Blink LED: led blink <num> <times>", cmd_led_blink);
SHELL_CMD_REGISTER(led_all_on, NULL, "Turn all LEDs on", cmd_led_all_on);
SHELL_CMD_REGISTER(led_all_off, NULL, "Turn all LEDs off", cmd_led_all_off);
SHELL_CMD_REGISTER(button_status, NULL, "Check button status", cmd_button_status);
SHELL_CMD_REGISTER(sensor_accel, NULL, "Read accelerometer", cmd_sensor_accel);
SHELL_CMD_REGISTER(sensor_magn, NULL, "Read magnetometer", cmd_sensor_magn);
SHELL_CMD_REGISTER(test_all, NULL, "Run all tests", cmd_test_all);

int main(void)
{
    // Configure all LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        if (!gpio_is_ready_dt(&leds[i])) {
            printk("LED %d not ready\n", i);
            return -1;
        }
        gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
    }

    // Configure button
    if (!gpio_is_ready_dt(&button)) {
        printk("Button not ready\n");
        return -1;
    }
    gpio_pin_configure_dt(&button, GPIO_INPUT);

    printk("STM32F3 Discovery - Testing Functions!\n");

    // Run tests
    test_all_functions();

    printk("CLI is now available. Use shell commands to control LEDs and sensors.\n");
    printk("Available commands: led_on, led_off, led_blink, led_all_on, led_all_off, button_status, sensor_accel, sensor_magn, test_all\n");

    return 0;
}