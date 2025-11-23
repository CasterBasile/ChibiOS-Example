#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys_clock.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* ======================
 * Configurazione test
 * ====================== */
#define PERIOD_MS   500
#define N_SAMPLES   1000

/* ======================
 * LED di bordo (per debug)
 * ====================== */
#define LED0_NODE DT_ALIAS(led0)
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "No alias 'led0' found in device tree"
#endif
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* ==========================
 * Thread di carico CPU (base)
 * ========================== */

static void cpu_load_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    volatile float x = 1.0f;

    while (1) {
        for (uint32_t i = 0U; i < 20000U; i++) {
            x = x * 1.0001f + 0.0001f;
        }
        k_yield();
    }
}

K_THREAD_STACK_DEFINE(cpu_stack, 512);
static struct k_thread cpu_thread_data;

/* ==========================
 * Mini font 5x7 per OLED
 * (solo numeri + lettere usate)
 * ========================== */

/* Font 5x7 per caratteri base: ' ', '0'-'9', 'S', 'a','m','p','l','e','u','s' */
typedef struct {
    char c;
    uint8_t data[5]; /* 5 colonne x 7 righe (LSB=top) */
} font5x7_t;

/* Definizione minima, sufficiente per "Sample N" e "XXXX us" */
static const font5x7_t font5x7_table[] = {
    { ' ', { 0x00,0x00,0x00,0x00,0x00 } },
    { '0', { 0x3E,0x51,0x49,0x45,0x3E } },
    { '1', { 0x00,0x42,0x7F,0x40,0x00 } },
    { '2', { 0x42,0x61,0x51,0x49,0x46 } },
    { '3', { 0x21,0x41,0x45,0x4B,0x31 } },
    { '4', { 0x18,0x14,0x12,0x7F,0x10 } },
    { '5', { 0x27,0x45,0x45,0x45,0x39 } },
    { '6', { 0x3C,0x4A,0x49,0x49,0x30 } },
    { '7', { 0x01,0x71,0x09,0x05,0x03 } },
    { '8', { 0x36,0x49,0x49,0x49,0x36 } },
    { '9', { 0x06,0x49,0x49,0x29,0x1E } },
    { 'S', { 0x22,0x49,0x49,0x49,0x31 } },
    { 'a', { 0x20,0x54,0x54,0x54,0x78 } },
    { 'm', { 0x7C,0x04,0x18,0x04,0x78 } },
    { 'p', { 0x7C,0x14,0x14,0x14,0x08 } },
    { 'l', { 0x00,0x44,0x7C,0x40,0x00 } },
    { 'e', { 0x38,0x54,0x54,0x54,0x18 } },
    { 'u', { 0x3C,0x40,0x40,0x20,0x7C } },
    { 's', { 0x08,0x54,0x54,0x54,0x20 } },
};

/* Cerca il carattere nella tabella font */
static const uint8_t *font5x7_get(char c)
{
    for (size_t i = 0; i < ARRAY_SIZE(font5x7_table); i++) {
        if (font5x7_table[i].c == c) {
            return font5x7_table[i].data;
        }
    }
    return font5x7_table[0].data; /* spazio di default */
}

/* ==========================
 * Funzioni helper per framebuffer
 * ========================== */

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define FB_SIZE      (OLED_WIDTH * OLED_HEIGHT / 8)

static uint8_t fb[FB_SIZE];

static void fb_clear(void)
{
    memset(fb, 0x00, sizeof(fb));
}

/* set pixel (x,y) = 1 */
static void fb_set_pixel(int x, int y)
{
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) {
        return;
    }
    int byte_index = x + (y / 8) * OLED_WIDTH;
    int bit = y % 8;
    fb[byte_index] |= (1U << bit);
}

/* Disegna un carattere 5x7 */
static void fb_draw_char(int x, int y, char c)
{
    const uint8_t *data = font5x7_get(c);

    for (int col = 0; col < 5; col++) {
        uint8_t col_data = data[col];
        for (int row = 0; row < 7; row++) {
            if (col_data & (1U << row)) {
                fb_set_pixel(x + col, y + row);
            }
        }
    }
}

/* Disegna una stringa (spaziatura 1 colonna tra char) */
static void fb_draw_text(int x, int y, const char *s)
{
    int cursor_x = x;
    while (*s) {
        fb_draw_char(cursor_x, y, *s);
        cursor_x += 6; /* 5 colonne + 1 di spazio */
        s++;
    }
}

/* ==========================
 * Thread OLED + misura
 * ========================== */

K_THREAD_STACK_DEFINE(oled_stack, 2048);
static struct k_thread oled_thread_data;

static void oled_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    const struct device *display =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        printk("Display not ready!\n");
        return;
    }

    struct display_capabilities caps;
    display_get_capabilities(display, &caps);

    struct display_buffer_descriptor buf_desc = {
        .buf_size = sizeof(fb),
        .width    = OLED_WIDTH,
        .height   = OLED_HEIGHT,
        .pitch    = OLED_WIDTH,
    };

    fb_clear();
    display_blanking_off(display);
    display_write(display, 0, 0, &buf_desc, fb);

    printk("[Zephyr] OLED latency thread starting...\n");

    /* Parametri timing ad alta risoluzione */
    uint32_t cycles_per_sec = sys_clock_hw_cycles_per_sec();
    if (cycles_per_sec == 0U) {
        printk("sys_clock_hw_cycles_per_sec() == 0!\n");
        return;
    }
    uint32_t cycles_per_us = cycles_per_sec / 1000000U;
    if (cycles_per_us == 0U) {
        cycles_per_us = 1U;
    }

    int64_t min_us = INT64_MAX;
    int64_t max_us = INT64_MIN;
    int64_t sum_us = 0;

    /* tempo iniziale in cicli */
    uint32_t prev_cycles_32 = k_cycle_get_32();
    uint64_t prev_cycles = (uint64_t)prev_cycles_32;

    char line1[32];
    char line2[32];

    for (uint32_t i = 0; i < N_SAMPLES; i++) {

        /* dormi circa PERIOD_MS */
        k_msleep(PERIOD_MS);

        /* cicli al wake-up */
        uint32_t curr_cycles_32 = k_cycle_get_32();
        uint64_t curr_cycles = (uint64_t)curr_cycles_32;

        /* differenza in cicli con gestione wrap */
        uint64_t delta_cycles;
        if (curr_cycles >= prev_cycles) {
            delta_cycles = curr_cycles - prev_cycles;
        } else {
            delta_cycles = (uint64_t)curr_cycles + (1ULL << 32) - prev_cycles;
        }

        uint64_t interval_us = delta_cycles / (uint64_t)cycles_per_us;
        int64_t lat_us = (int64_t)interval_us - (int64_t)(PERIOD_MS * 1000);

        if (lat_us < min_us) min_us = lat_us;
        if (lat_us > max_us) max_us = lat_us;
        sum_us += lat_us;

        /* Serial print */
        printk("i=%4u, interval=%llu us, lat=%lld us\n",
               (unsigned int)i,
               (unsigned long long)interval_us,
               (long long)lat_us);

        /* LED di debug */
        gpio_pin_toggle_dt(&led);

        /* Aggiorna OLED: "Sample N" e "XXXX us" */
        fb_clear();
        snprintk(line1, sizeof(line1), "Sample %lu", (unsigned long)(i + 1));
        snprintk(line2, sizeof(line2), "%lld us", (long long)lat_us);
        fb_draw_text(0, 8, line1);
        fb_draw_text(0, 24, line2);
        display_write(display, 0, 0, &buf_desc, fb);

        prev_cycles = curr_cycles;
    }

    int64_t mean_us = sum_us / N_SAMPLES;
    int64_t jitter_us = max_us - min_us;

    printk("\n=== Zephyr OLED Latency Summary ===\n");
    printk("Period: %d ms, Samples: %d\n", PERIOD_MS, N_SAMPLES);
    printk("Mode: LOAD BASE (OLED)\n");
    printk("Min: %lld us, Max: %lld us, Mean: %lld us, Jitter: %lld us\n",
           (long long)min_us,
           (long long)max_us,
           (long long)mean_us,
           (long long)jitter_us);

    /* Messaggio finale su OLED */
    fb_clear();
    fb_draw_text(0, 8,  "Test done");
    fb_draw_text(0, 24, "See serial");
    display_write(display, 0, 0, &buf_desc, fb);

    while (1) {
        k_msleep(1000);
    }
}

/* ==========================
 * main()
 * ========================== */

int main(void)
{
    int ret;

    printk("\n[Zephyr] latency_oled starting...\n");

    /* Configura LED */
    if (!device_is_ready(led.port)) {
        printk("LED device not ready!\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("LED config failed: %d\n", ret);
        return 0;
    }

    /* Thread di carico CPU */
    k_thread_create(&cpu_thread_data,
                    cpu_stack,
                    K_THREAD_STACK_SIZEOF(cpu_stack),
                    cpu_load_thread,
                    NULL, NULL, NULL,
                    K_PRIO_PREEMPT(8), 0, K_NO_WAIT);

    /* Thread OLED + misura */
    k_thread_create(&oled_thread_data,
                    oled_stack,
                    K_THREAD_STACK_SIZEOF(oled_stack),
                    oled_thread,
                    NULL, NULL, NULL,
                    K_PRIO_PREEMPT(7), 0, K_NO_WAIT);

    while (1) {
        k_msleep(1000);
    }

    return 0;
}
