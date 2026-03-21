#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/display/cfb.h>

int main(void)
{
    /* Get a handle to the SSD1306 device defined in the overlay */
    const struct device *display = DEVICE_DT_GET(DT_NODELABEL(ssd1306));

    /* Check the device is ready before using it */
    if (!device_is_ready(display)) {
        printk("SSD1306 device not ready!\n");
        return -1;
    }

    /* Turn the display on (exit sleep mode) */
    display_blanking_off(display);

    /* Initialize the Character Frame Buffer */
    if (cfb_framebuffer_init(display)) {
        printk("CFB init failed!\n");
        return -1;
    }

    /* Clear the screen */
    cfb_framebuffer_clear(display, true);

    /* Print text at position (column=0, row=0) */
    cfb_print(display, "Hello, World!", 0, 0);
    cfb_print(display, "Zephyr RTOS", 0, 16);
    cfb_print(display, "STM32 F401RE", 0, 32);

    /* Push the frame buffer to the display */
    cfb_framebuffer_finalize(display);

    printk("Display initialized successfully!\n");

    /* Keep the program running */
    while (1) {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}