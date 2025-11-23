#ifndef D_OUT_H
#define D_OUT_H

#include "ch.h"
#include "hal.h"

/*
 * Digital Output Peripherals
 */

void d_out_init(void);
void Wled_write(uint8_t);
void Rled_write(uint8_t);
void fan_on(void);
void fan_off(void);

#endif
