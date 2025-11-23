#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <string.h> // Include this for strncmp

static BaseSequentialStream *chp2 = (BaseSequentialStream*)&SD2;
static BaseSequentialStream *chp1 = (BaseSequentialStream*)&SD1;

static SerialConfig uartCfg1 = {
    38400,       // Baudrate
    0,
    USART_CR2_STOP1_BITS,
    0
};

// Define the LED pin for your board
#define GPIO_LED GPIOA // Change this to the correct GPIO port
#define LED_PIN 5     // Change this to the correct pin number

/*
 * Function to initialize serial communication
 */
static void serial_init(void) {
    // Configure UART2 pins (PA2: TX, PA3: RX)
    palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7)); // UART2 TX
    palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7)); // UART2 RX
    sdStart(&SD2, &uartCfg1); // Start UART2 with configuration

    // Configure UART1 pins (PC4: TX, PC5: RX)
    palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7)); // UART1 TX
    palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7)); // UART1 RX
    sdStart(&SD1, &uartCfg1); // Start UART1 with configuration
}

/*
 * Function to control the LED
 */
static void control_led(bool state) {
    if (state) {
        palSetPad(GPIO_LED, LED_PIN); // Set the LED pin high
    } else {
        palClearPad(GPIO_LED, LED_PIN); // Set the LED pin low
    }
}

/*
 * This thread reads from the UART and processes commands to control the LED
 */
static THD_WORKING_AREA(waSerialEcho, 1024);
static THD_FUNCTION(thdSerialEcho, arg) {
    (void)arg;
    chRegSetThreadName("SerialEcho");
    uint8_t buf[100];
    size_t n;

    while (true) {
        n = sdReadTimeout(&SD1, buf, sizeof(buf)-1, TIME_MS2I(500));
        if (n > 0) {
            buf[n] = '\0'; // Null-terminate the buffer
            // Check if the received data matches "led_ON" or "led_OFF"
            if (strncmp((char*)buf, "led ON", 6) == 0) {
                control_led(true); // Turn the LED on
            } else if (strncmp((char*)buf, "led OFF", 7) == 0) {
                control_led(false); // Turn the LED off
            }
            // Echo the received command to the other serial port
            chprintf(chp2, "%s", (char*)buf);
        }
        chThdSleepMilliseconds(100);
    }
}

/*
 * Application entry point
 */
int main(void) {
    halInit();
    chSysInit();

    serial_init(); // Initialize serial ports

    // Initialize LED pin
    palSetPadMode(GPIO_LED, LED_PIN, PAL_MODE_OUTPUT_PUSHPULL); // Set GPIO pin mode

    // Create the serial echo thread
    chThdCreateStatic(waSerialEcho, sizeof(waSerialEcho), NORMALPRIO, thdSerialEcho, NULL);

    // Main loop
    int i = 0;
    while (true) {
        chThdSleepMilliseconds(1000); // Main loop delay
        chprintf(chp1, "contatore:%d\n", i++);
        chprintf(chp1, "contatore_doppio:%d\n", i*2);
    }
}
