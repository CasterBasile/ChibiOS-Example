#ifndef ICU_IN_H
#define ICU_IN_H

#include "ch.h"
#include "hal.h"
#include "../lib/data.h"


/*
 * ICU and DHT config
 */
#define ANSI_ESCAPE_CODE_ALLOWED    TRUE
#define MCU_REQUEST_WIDTH           18000
#define DHT_ERROR_WIDTH             200
#define DHT_START_BIT_WIDTH         80
#define DHT_LOW_BIT_WIDTH           28
#define DHT_HIGH_BIT_WIDTH          70
#define ICU_FREQUENCY               1000000

/*
 * DHT HUM and TMP sensor read function prototype
 */
void icu_read(tx_data_t *);

#endif
