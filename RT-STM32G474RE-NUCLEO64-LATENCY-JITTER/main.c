#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "ssd1306.h"
#include "stdio.h"
#include "stdint.h"

/* ======================
 *  Configurazione test
 * ====================== */
#define PERIOD_MS     500     /* Periodo del task OLED.               */
#define N_SAMPLES     1000    /* Numero di campioni di latenza.       */

#define BUFF_SIZE     20
static char buff[BUFF_SIZE];

/* ======================
 *  Configurazione I2C
 * ====================== */
static const I2CConfig i2ccfg = {
  STM32_TIMINGR_PRESC(8U)  |
  STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) |
  STM32_TIMINGR_SCLH(3U)   | STM32_TIMINGR_SCLL(9U),
  0,
  0
};

static const SSD1306Config ssd1306cfg = {
  &I2CD1,
  &i2ccfg,
  SSD1306_SAD_0X78,
};

static SSD1306Driver SSD1306D1;

/* ==========================
 *  Thread di carico CPU (base)
 * ========================== */
static THD_WORKING_AREA(waCpuLoad, 256);
static THD_FUNCTION(CpuLoadThread, arg) {
  (void)arg;
  chRegSetThreadName("CpuLoad");

  volatile float x = 1.0f;

  while (true) {
    /* Carico leggero */
    for (uint32_t i = 0; i < 20000U; i++) {
      x = x * 1.0001f + 0.0001f;
    }
    chThdYield();
  }
}

/* ==========================
 *  Thread OLED + misura
 * ========================== */

static THD_WORKING_AREA(waOledDisplay, 512);
static THD_FUNCTION(OledDisplay, arg) {
  (void)arg;
  chRegSetThreadName("OledDisplay");

  /* Inizializzazione OLED */
  ssd1306ObjectInit(&SSD1306D1);
  ssd1306Start(&SSD1306D1, &ssd1306cfg);
  ssd1306FillScreen(&SSD1306D1, 0x00);

  ssd1306GotoXy(&SSD1306D1, 0, 1);
  ssd1306Puts(&SSD1306D1, "Latency test", &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  ssd1306UpdateScreen(&SSD1306D1);

  /* Variabili per misura latenza */
  systime_t next = chVTGetSystemTimeX();

  uint32_t min_us = 0xFFFFFFFFU;
  uint32_t max_us = 0U;
  uint64_t sum_us = 0U;

  for (uint32_t i = 0; i < N_SAMPLES; i++) {

    next += TIME_MS2I(PERIOD_MS);

    systime_t wakeTime   = next;
    chThdSleepUntil(next);
    systime_t actualTime = chVTGetSystemTimeX();

    systime_t diff      = actualTime - wakeTime;
    uint32_t  lat_us    = TIME_I2US(diff);

    /* Statistiche online */
    if (lat_us < min_us) min_us = lat_us;
    if (lat_us > max_us) max_us = lat_us;
    sum_us += lat_us;

    /* Stampa ogni campione sulla seriale (SD2) */
    chprintf((BaseSequentialStream *)&SD2,
             "i=%4lu, lat=%lu us\r\n",
             (unsigned long)i,
             (unsigned long)lat_us);

    /* OLED minimale */
    ssd1306FillScreen(&SSD1306D1, 0x00);
    ssd1306GotoXy(&SSD1306D1, 0, 1);
    chsnprintf(buff, BUFF_SIZE, "Sample %lu", (unsigned long)(i+1));
    ssd1306Puts(&SSD1306D1, buff, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);

    ssd1306GotoXy(&SSD1306D1, 0, 16);
    chsnprintf(buff, BUFF_SIZE, "%lu us", (unsigned long)lat_us);
    ssd1306Puts(&SSD1306D1, buff, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);

    ssd1306UpdateScreen(&SSD1306D1);
  }

  uint32_t mean_us   = (uint32_t)(sum_us / N_SAMPLES);
  uint32_t jitter_us = max_us - min_us;

  chprintf((BaseSequentialStream *)&SD2,
           "\r\n=== ChibiOS Latency Summary ===\r\n");
  chprintf((BaseSequentialStream *)&SD2,
           "Period: %u ms, Samples: %u\r\n",
           (unsigned)PERIOD_MS, (unsigned)N_SAMPLES);
  chprintf((BaseSequentialStream *)&SD2,
           "Mode: LOAD BASE\r\n");
  chprintf((BaseSequentialStream *)&SD2,
           "Min: %lu us, Max: %lu us, Mean: %lu us, Jitter: %lu us\r\n",
           (unsigned long)min_us,
           (unsigned long)max_us,
           (unsigned long)mean_us,
           (unsigned long)jitter_us);

  /* OLED finale */
  ssd1306FillScreen(&SSD1306D1, 0x00);
  ssd1306GotoXy(&SSD1306D1, 0, 1);
  ssd1306Puts(&SSD1306D1, "Test done", &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  ssd1306GotoXy(&SSD1306D1, 0, 16);
  ssd1306Puts(&SSD1306D1, "See serial", &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
  ssd1306UpdateScreen(&SSD1306D1);

  while (true) {
    chThdSleepMilliseconds(1000);
  }
}

/* ==========================
 *  main()
 * ========================== */

int main(void) {

  halInit();
  chSysInit();

  /* USART2 PA2/PA3 */
  palSetLineMode(PAL_LINE(GPIOA, 2U),
                 PAL_MODE_ALTERNATE(7) | PAL_STM32_PUPDR_PULLUP |
                 PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);
  palSetLineMode(PAL_LINE(GPIOA, 3U),
                 PAL_MODE_ALTERNATE(7) | PAL_STM32_PUPDR_PULLUP |
                 PAL_STM32_OSPEED_HIGHEST | PAL_STM32_OTYPE_PUSHPULL);

  sdStart(&SD2, NULL);
  chprintf((BaseSequentialStream *)&SD2,
           "\r\n[ChibiOS] Latency test (LOAD BASE)...\r\n");

  /* I2C PB8=SCL, PB9=SDA */
  palSetLineMode(PAL_LINE(GPIOB, 8U), PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);
  palSetLineMode(PAL_LINE(GPIOB, 9U), PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);

  /* Carico base sempre attivo */
  chThdCreateStatic(waCpuLoad, sizeof(waCpuLoad),
                    NORMALPRIO - 1, CpuLoadThread, NULL);

  /* Thread OLED + misura */
  chThdCreateStatic(waOledDisplay, sizeof(waOledDisplay),
                    NORMALPRIO, OledDisplay, NULL);

  while (true) {
    chThdSleepMilliseconds(1000);
  }
}
