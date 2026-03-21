#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// Use DTS Labels from your table
static const struct gpio_dt_spec red_led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(red_led_3), gpios);
static const struct gpio_dt_spec orange_led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(orange_led_5), gpios);
static const struct gpio_dt_spec green_led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_7), gpios);
static const struct gpio_dt_spec blue_led1 = GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led_9), gpios);
static const struct gpio_dt_spec red_led2 = GPIO_DT_SPEC_GET(DT_NODELABEL(red_led_10), gpios);
static const struct gpio_dt_spec orange_led2 = GPIO_DT_SPEC_GET(DT_NODELABEL(orange_led_8), gpios);
static const struct gpio_dt_spec green_led2 = GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_6), gpios);
static const struct gpio_dt_spec blue_led2 = GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led_4), gpios);


int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&red_led1)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&red_led2)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&orange_led1)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&orange_led2)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&green_led1)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&green_led2)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&blue_led1)) {
        return -1;
    }
    if (!gpio_is_ready_dt(&blue_led2)) {
        return -1;
    }


    ret=gpio_pin_configure_dt(&red_led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&orange_led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&green_led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&blue_led1, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&red_led2, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&orange_led2, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&green_led2, GPIO_OUTPUT_ACTIVE);
        if (ret < 0) {
        return -1;
    }

    ret=gpio_pin_configure_dt(&blue_led2, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }
    gpio_pin_set_dt(&red_led1, 0);
    gpio_pin_set_dt(&red_led2, 0);
    gpio_pin_set_dt(&green_led1, 0);
    gpio_pin_set_dt(&green_led2, 0);
    gpio_pin_set_dt(&orange_led1, 0);
    gpio_pin_set_dt(&orange_led2, 0);
    gpio_pin_set_dt(&blue_led1, 0);
    gpio_pin_set_dt(&blue_led2, 0);

    //printk("LED blink started on GPIO %d\n", led.pin);

    while (1) {
        gpio_pin_set_dt(&red_led1, 1);
        gpio_pin_set_dt(&red_led2, 1);
        gpio_pin_set_dt(&green_led1, 0);
        gpio_pin_set_dt(&green_led2, 0);
        gpio_pin_set_dt(&orange_led1, 0);
        gpio_pin_set_dt(&orange_led2, 0);
        gpio_pin_set_dt(&blue_led1, 0);
        gpio_pin_set_dt(&blue_led2, 0);
        k_sleep(K_MSEC(1500));


        gpio_pin_set_dt(&red_led1, 0);
        gpio_pin_set_dt(&red_led2, 0);
        gpio_pin_set_dt(&green_led1, 0);
        gpio_pin_set_dt(&green_led2, 0);
        gpio_pin_set_dt(&orange_led1, 0);
        gpio_pin_set_dt(&orange_led2, 0);
        gpio_pin_set_dt(&blue_led1, 1);
        gpio_pin_set_dt(&blue_led2, 1);
        k_sleep(K_MSEC(1500));

        gpio_pin_set_dt(&red_led1, 0);
        gpio_pin_set_dt(&red_led2, 0);
        gpio_pin_set_dt(&green_led1, 1);
        gpio_pin_set_dt(&green_led2, 1);
        gpio_pin_set_dt(&orange_led1, 0);
        gpio_pin_set_dt(&orange_led2, 0);
        gpio_pin_set_dt(&blue_led1, 0);
        gpio_pin_set_dt(&blue_led2, 0);
        k_sleep(K_MSEC(1500));

        gpio_pin_set_dt(&red_led1, 0);
        gpio_pin_set_dt(&red_led2, 0);
        gpio_pin_set_dt(&green_led1, 0);
        gpio_pin_set_dt(&green_led2, 0);
        gpio_pin_set_dt(&orange_led1, 1);
        gpio_pin_set_dt(&orange_led2, 1);
        gpio_pin_set_dt(&blue_led1, 0);
        gpio_pin_set_dt(&blue_led2, 0);
        k_sleep(K_MSEC(1500));
        
    }

    return 0;
}