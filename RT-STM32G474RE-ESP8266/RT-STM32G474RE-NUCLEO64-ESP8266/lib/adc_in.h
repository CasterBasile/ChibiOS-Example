#ifndef ADC_IN_H
#define ADC_IN_H

#include "ch.h"
#include "hal.h"
#include "../lib/data.h"

/*
 * ADC Peripherals Config
 */

#define VOLTAGE_RES            ((float)3.3/4096)
#define ADC_GRP_NUM_CHANNELS   2
#define ADC_GRP_BUF_DEPTH      1

static const ADCConversionGroup linearcfg = {
          .circular     = false,
          .num_channels = ADC_GRP_NUM_CHANNELS,
          .end_cb       = NULL,
          .error_cb     = NULL,
          .cfgr         = 0U,
          .cfgr2        = 0U,
          .tr1          = ADC_TR_DISABLED,
          .tr2          = ADC_TR_DISABLED,
          .tr3          = ADC_TR_DISABLED,
          .awd2cr       = 0U,
          .awd3cr       = 0U,
          .smpr         = {
            ADC_SMPR1_SMP_AN6(ADC_SMPR_SMP_47P5) | ADC_SMPR1_SMP_AN7(ADC_SMPR_SMP_47P5),
            0U
          },
          .sqr          = {
            ADC_SQR1_SQ1_N(ADC_CHANNEL_IN6) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN7),
            0U,
            0U,
            0U
          }
};

/*
 * Functions' Prototypes
 */
void adc_init(void);
void adc_read(tx_data_t *);

#endif
