#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* ---- Pin Definitions ---- */

/* Rows (OUTPUT) */
#define ROW1 1
#define ROW2 4
#define ROW3 5
#define ROW4 6

/* Columns (INPUT + PULLDOWN) */
#define COL1 14
#define COL2 15
#define COL3 2
#define COL4 3

#define ROWS 4
#define COLS 4

static const struct device *gpio_dev;

/* Keypad layout */
static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

/* ---- Initialization ---- */
void keypad_init(void)
{
        gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));

        if (!device_is_ready(gpio_dev))
        {
                printk("GPIO device not ready\n");
                return;
        }

        /* Configure rows as OUTPUT HIGH */
        gpio_pin_configure(gpio_dev, ROW1, GPIO_OUTPUT_HIGH);
        gpio_pin_configure(gpio_dev, ROW2, GPIO_OUTPUT_HIGH);
        gpio_pin_configure(gpio_dev, ROW3, GPIO_OUTPUT_HIGH);
        gpio_pin_configure(gpio_dev, ROW4, GPIO_OUTPUT_HIGH);

        /* Configure columns as INPUT + PULLDOWN */
        gpio_pin_configure(gpio_dev, COL1, GPIO_INPUT | GPIO_PULL_DOWN);
        gpio_pin_configure(gpio_dev, COL2, GPIO_INPUT | GPIO_PULL_DOWN);
        gpio_pin_configure(gpio_dev, COL3, GPIO_INPUT | GPIO_PULL_DOWN);
        gpio_pin_configure(gpio_dev, COL4, GPIO_INPUT | GPIO_PULL_DOWN);

        printk("Keypad initialized\n");
}

/* ---- Scan Function ---- */
char scan_keypad(void)
{
        int rows[ROWS] = {ROW1, ROW2, ROW3, ROW4};
        int cols[COLS] = {COL1, COL2, COL3, COL4};

        for (int row = 0; row < ROWS; row++) {
                gpio_pin_set(gpio_dev, rows[row], 0);
        }

        for (int r = 0; r < ROWS; r++)
        {
                gpio_pin_set(gpio_dev, rows[r], 1);
                k_usleep(50);

                for (int c = 0; c < COLS; c++)
                {

                        if (gpio_pin_get(gpio_dev, cols[c]))
                        {

                                /* Debounce */
                                k_msleep(20);

                                if (gpio_pin_get(gpio_dev, cols[c]))
                                {

                                        printk("R%d C%d\n", r + 1, c + 1);

                                        return keymap[r][c];
                                }
                        }
                }

                /* Restore row LOW */
                gpio_pin_set(gpio_dev, rows[r], 0);
        }

        return 0;
}

/* ---- Main ---- */
void main(void)
{
        printk("Starting Keypad Example...\n");

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