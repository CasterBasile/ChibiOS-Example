#ifndef D_IN_H
#define D_IN_H

#include "ch.h"
#include "hal.h"
#include "../lib/data.h"

/*
 * Digital Input Peripherals
 */

void d_in_init(void);
void d_in_read(tx_data_t *);

#endif
