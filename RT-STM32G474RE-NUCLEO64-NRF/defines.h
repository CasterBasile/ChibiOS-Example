/*
 * defines.h
 *
 *  Created on: 7 set 2023
 *      Author: Marco
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#include "rf.h"

#define MSG "HELLO"

/*PIN DEFINES*/
#define NRF_CE_LINE PAL_LINE(GPIOA,9U)
#define NRF_IRQ_LINE PAL_LINE(GPIOC,7U)
#define NRF_MISO_LINE PAL_LINE(GPIOA,6U)
#define NRF_MOSI_LINE PAL_LINE(GPIOA,7U)
#define NRF_SCK_LINE PAL_LINE(GPIOA,5U)
#define NRF_SPI_CS PAL_LINE(GPIOB,6U)

/*Cnfig structures*/

static const SPIConfig std_spi_cfg = {
  .circular = FALSE,
  .slave = FALSE,
  .data_cb = NULL,
  .error_cb = NULL,
  .ssport = GPIOB,
  .sspad = 6U,                               /* Line of CS. */
  SPI_CR1_BR_1 | SPI_CR1_BR_0,                    /* CR1 register */
  0                                               /* CR2 register */
};

static RFConfig nrf24l01_cfg = {
  .line_ce = NRF_CE_LINE,
  .line_irq = NRF_IRQ_LINE,
  .spip = &SPID1,
  .spicfg = &std_spi_cfg,
  .auto_retr_count = NRF24L01_ARC_15_times,     /* auto_retr_count */
  .auto_retr_delay = NRF24L01_ARD_4000us,       /* auto_retr_delay */
  .address_width = NRF24L01_AW_5_bytes,       /* address_width */
  .channel_freq = 42,                       /* channel_freq 2.4 + 0.13 GHz */
  .data_rate = NRF24L01_ADR_2Mbps,        /* data_rate */
  .out_pwr = NRF24L01_PWR_0dBm,         /* out_pwr */
  .lna = NRF24L01_LNA_disabled,     /* lna */
  .en_dpl = NRF24L01_DPL_enabled ,     /* en_dpl */
  .en_ack_pay = NRF24L01_ACK_PAY_disabled, /* en_ack_pay */
  .en_dyn_ack = NRF24L01_DYN_ACK_disabled  /* en_dyn_ack */
};

#endif /* DEFINES_H_ */
