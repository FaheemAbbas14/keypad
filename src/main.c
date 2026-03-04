#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define ROWS 4
#define COLS 4

#define KEYPAD_NODE DT_PATH(zephyr_user)

BUILD_ASSERT(DT_NODE_HAS_PROP(KEYPAD_NODE, row_gpios), "Missing row-gpios in zephyr,user node");
BUILD_ASSERT(DT_NODE_HAS_PROP(KEYPAD_NODE, col_gpios), "Missing col-gpios in zephyr,user node");
BUILD_ASSERT(DT_PROP_LEN(KEYPAD_NODE, row_gpios) == ROWS, "row-gpios must have 4 entries");
BUILD_ASSERT(DT_PROP_LEN(KEYPAD_NODE, col_gpios) == COLS, "col-gpios must have 4 entries");

static const struct gpio_dt_spec row_gpios[ROWS] = {
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, row_gpios, 0),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, row_gpios, 1),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, row_gpios, 2),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, row_gpios, 3),
};

static const struct gpio_dt_spec col_gpios[COLS] = {
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, col_gpios, 0),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, col_gpios, 1),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, col_gpios, 2),
        GPIO_DT_SPEC_GET_BY_IDX(KEYPAD_NODE, col_gpios, 3),
};

/* Keypad layout */
static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

/* ---- Initialization ---- */
void keypad_init(void)
{
        for (int i = 0; i < ROWS; i++) {
                if (!gpio_is_ready_dt(&row_gpios[i])) {
                        printk("Row GPIO %d not ready\n", i + 1);
                        return;
                }

                gpio_pin_configure_dt(&row_gpios[i], GPIO_OUTPUT_INACTIVE);
        }

        for (int i = 0; i < COLS; i++) {
                if (!gpio_is_ready_dt(&col_gpios[i])) {
                        printk("Column GPIO %d not ready\n", i + 1);
                        return;
                }

                gpio_pin_configure_dt(&col_gpios[i], GPIO_INPUT);
        }

        printk("Keypad initialized\n");
}

/* ---- Scan Function ---- */
char scan_keypad(void)
{
        for (int row = 0; row < ROWS; row++) {
                gpio_pin_set_dt(&row_gpios[row], 0);
        }

        for (int r = 0; r < ROWS; r++)
        {
                gpio_pin_set_dt(&row_gpios[r], 1);
                k_usleep(50);

                for (int c = 0; c < COLS; c++)
                {

                        if (gpio_pin_get_dt(&col_gpios[c]))
                        {

                                /* Debounce */
                                k_msleep(20);

                                if (gpio_pin_get_dt(&col_gpios[c]))
                                {


                                        return keymap[r][c];
                                }
                        }
                }

                /* Restore row LOW */
                gpio_pin_set_dt(&row_gpios[r], 0);
        }

        return 0;
}

/* ---- Main ---- */
void main(void)
{
        printk("Starting Keypad\n");

        keypad_init();

        char last_key = 0;

        while (1)
        {

                char key = scan_keypad();

                if (key && key != last_key)
                {
                        printk("Key pressed: %c\n", key);
                        last_key = key;
                }

                if (!key)
                {
                        last_key = 0;
                }

                k_msleep(50);
        }
}