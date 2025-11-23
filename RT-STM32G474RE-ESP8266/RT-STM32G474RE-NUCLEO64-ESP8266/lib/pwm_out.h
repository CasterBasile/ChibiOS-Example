#ifndef PWM_OUT_H
#define PWM_OUT_H

#include "ch.h"
#include "hal.h"

/*
 * PWM Peripherals config
 */
#define PWM_TIMER_FREQUENCY     1000000
#define PWM_PERIOD_B            100
#define PWM_PERIOD_S            (PWM_TIMER_FREQUENCY * 20 / 1000)

static const PWMConfig pwmcfg_B = {
  .frequency = PWM_TIMER_FREQUENCY,
  .period = PWM_PERIOD_B,
  .callback = NULL,
  .channels = {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  }
};

static const PWMConfig pwmcfg_S = {
  .frequency = PWM_TIMER_FREQUENCY,
  .period = PWM_PERIOD_S,
  .callback = NULL,
  .channels = {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  }
};


/*
 * Functions' Prototypes
 */
void pwm_init(void);
void play_note(int);
void stop_note(void);
void open_window(void);
void close_window(void);
void pwm_term(void);

#endif
