#ifndef FUNC_H
#define FUNC_H

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "data.h"
#include "../lib/pwm_out.h"
#include "../lib/d_out.h"

/*
 * DEBUG Functions
 * They print the data structs values in the terminal
 */
void print_data_tx(tx_data_t*);
void print_data_rx(rx_data_t*);

/*
 * Functions used by the Threads
 */
void serial_init(void);
void wifi_read(rx_data_t*);
void wifi_write(tx_data_t*);
void exe_wifi_request(rx_data_t*, uint8_t*);
void emergencyOff(uint8_t*);
void check_emergency(tx_data_t *, rx_data_t *, uint8_t *);

#endif
