/*
    ChibiOS - Copyright (C) 2016..2024 Tammaro Cimmino

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/**
 * @file    sht40ad1b.c
 * @brief   SHT40AD1B MEMS interface module code.
 *
 * @addtogroup SHT40AD1B
 * @ingroup EX_ST
 * @{
 */

#include "hal.h"
#include "sht40ad1b.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define SHT40AD1B_SEL(mask, offset)    (int16_t)(mask << offset)

#define SHT40AD1B_FLAG_HYGRO_BIAS      0x01
#define SHT40AD1B_FLAG_HYGRO_SENS      0x02
#define SHT40AD1B_FLAG_THERMO_BIAS     0x04
#define SHT40AD1B_FLAG_THERMO_SENS     0x08

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if (SHT40AD1B_USE_I2C) || defined(__DOXYGEN__)
/**
 * @brief   Reads registers value using I2C.
 * @pre     The I2C interface must be initialized and the driver started.
 *
 * @param[in]  i2cp      pointer to the I2C interface
 * @param[out] rxbuf     pointer to an output buffer
 * @param[in]  n         number of consecutive register to read
 * @return               the operation status.
 *
 * @notapi
 */
static msg_t sht40ad1bI2CReadRegister(I2CDriver *i2cp, uint8_t* rxbuf, size_t n) {
  return i2cMasterTransmitTimeout(i2cp, SHT40AD1B_SAD, NULL, 0, rxbuf, n,
                                 TIME_INFINITE);
}


/**
 * @brief   Writes a value into a register using I2C.
 * @pre     The I2C interface must be initialized and the driver started.
 *
 * @param[in] i2cp       pointer to the I2C interface
 * @param[in] txbuf      buffer values to write
 * @param[in] n          size of txbuf
 * @return               the operation status.
 *
 * @notapi
 */
static msg_t sht40ad1bI2CWriteRegister(I2CDriver *i2cp, uint8_t* txbuf, size_t n) {

  return i2cMasterTransmitTimeout(i2cp, SHT40AD1B_SAD, txbuf, n, NULL, 0,
                                  TIME_INFINITE);
}
#endif /* SHT40AD1B_USE_I2C */

/**
  * @brief  Calculate CRC
  *
  * @param  data        data stream bytes
  * @param  count       number of data bytes
  * @return             CRC check sum of data stream
  *
  */
uint8_t crc_calculate(const uint8_t *data, uint16_t count)
{
  const uint8_t crc8_polynomial = 0x31;
  uint8_t crc = 0xFF;

  /* Calculate 8-bit checksum for given polynomial */
  for (uint16_t index = 0; index < count; index++) {
    crc ^= data[index];
    for (uint8_t crc_bit = 0U; crc_bit < 8U; crc_bit++) {

      if((crc & 0x80U) != 0U){              /* If the MSB is 1 */
        crc = (crc << 1) ^ crc8_polynomial;
      }
      else{
        crc = crc << 1;
      }
    }
  }

  return crc;
}

/**
  * @brief  Check CRC
  *
  * @param  data        data stream bytes
  * @param  count       number of data bytes
  * @param  crc         CRC check sum of data stream
  * @retval             0 if CRC is OK else 1
  *
  */
uint8_t crc_check(const uint8_t *data, uint16_t count, uint8_t crc)
{
  return (crc_calculate(data, count) == crc) ? 0U : 1U;
}

static msg_t data_read_raw(void *ip, int32_t axes[]) {
  SHT40AD1BDriver* devp;
  uint8_t buff[6];
  uint16_t tmp;
  msg_t msg;
  uint8_t command = SHT40AD1B_AD_OUT_HIGH_PRECISION;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
              "data_read_raw(), invalid state");

  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "data_read_raw(), channel not ready");

#if SHT40AD1B_SHARED_I2C
  i2cAcquireBus(devp->config->i2cp);
  i2cStart(devp->config->i2cp,
           devp->config->i2ccfg);
#endif /* SHT40AD1B_SHARED_I2C */

  msg = sht40ad1bI2CWriteRegister(devp->config->i2cp, &command, 1);

  chThdSleepMilliseconds(10);

  if (msg == MSG_OK) {
    msg = sht40ad1bI2CReadRegister(devp->config->i2cp, buff, 6);
  }

#if SHT40AD1B_SHARED_I2C
  i2cReleaseBus(devp->config->i2cp);
#endif /* SHT40AD1B_SHARED_I2C */

  if (msg == MSG_OK) {
    tmp = (buff[0] << 8) + buff[1];
    axes[0] = (int32_t)tmp;
    tmp = (buff[3] << 8) + buff[4];
    axes[1] = (int32_t)tmp;
  }

  /* Check CRC for temperature value */
  if (crc_check(&buff[0], 2, buff[2]) != 0U) {
    return MSG_RESET;
  }

  /* Check CRC for humidity value */
  if (crc_check(&buff[3], 2, buff[5]) != 0U) {
    return MSG_RESET;
  }

  return msg;
}


/**
 * @brief   Return the number of axes of the BaseHygrometer.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 *
 * @return              the number of axes.
 */
static size_t hygro_get_axes_number(void *ip) {
  (void)ip;

  return SHT40AD1B_HYGRO_NUMBER_OF_AXES;
}

/**
 * @brief   Retrieves raw data from the BaseHygrometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t hygro_read_raw(void *ip, int32_t axes[]) {
  SHT40AD1BDriver* devp;
  int32_t raw[2];
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_read_raw(), invalid state");

  msg = data_read_raw(devp, raw);

  if (msg == MSG_OK) {
    *axes = raw[1];
  }

  return msg;
}

/**
 * @brief   Retrieves cooked data from the BaseHygrometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as %rH.
 * @note    The axes array must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 * @param[out] axes     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t hygro_read_cooked(void *ip, float axes[]) {
  SHT40AD1BDriver* devp;
  int32_t raw;
  msg_t msg;
  float humidity;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_read_cooked(), invalid state");

  msg = hygro_read_raw(ip, &raw);

  if (msg == MSG_OK) {
    humidity = (raw * devp->hygrosensitivity) - devp->hygrobias;

    humidity = (humidity > 100.0f) ? 100.0f : (humidity <   0.0f) ?   0.0f : humidity;

    *axes = humidity;
  }

  return msg;
}

/**
 * @brief   Set bias values for the BaseHygrometer.
 * @note    Bias must be expressed as %rH.
 * @note    The bias buffer must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t hygro_set_bias(void *ip, float *bp) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (bp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_set_bias(), invalid state");

  devp->hygrobias = *bp;
  return msg;
}

/**
 * @brief   Reset bias values for the BaseHygrometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t hygro_reset_bias(void *ip) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_reset_bias(), invalid state");

  devp->hygrobias = SHT40AD1B_HYGRO_BIAS;
  return msg;
}

/**
 * @brief   Set sensitivity values for the BaseHygrometer.
 * @note    Sensitivity must be expressed as %rH/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t hygro_set_sensitivity(void *ip, float *sp) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (sp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_set_sensitivity(), invalid state");

  devp->hygrosensitivity = *sp;
  return msg;
}

/**
 * @brief   Reset sensitivity values for the BaseHygrometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] ip        pointer to @p BaseHygrometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t hygro_reset_sensitivity(void *ip) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

    /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseHygrometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "hygro_reset_sensitivity(), invalid state");

  devp->hygrosensitivity = SHT40AD1B_HYGRO_SENS;
  return msg;
}

/**
 * @brief   Return the number of axes of the BaseThermometer.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 *
 * @return              the number of axes.
 */
static size_t thermo_get_axes_number(void *ip) {
  (void)ip;

  return SHT40AD1B_THERMO_NUMBER_OF_AXES;
}

/**
 * @brief   Retrieves raw data from the BaseThermometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t thermo_read_raw(void *ip, int32_t axes[]) {
  SHT40AD1BDriver* devp;
  int32_t raw[2];
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_read_raw(), invalid state");

  msg = data_read_raw(devp, raw);

  if (msg == MSG_OK) {
    *axes = raw[0];
  }

  return msg;
}

/**
 * @brief   Retrieves cooked data from the BaseThermometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as °C.
 * @note    The axes array must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 * @param[out] axis     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t thermo_read_cooked(void *ip, float* axis) {
  SHT40AD1BDriver* devp;
  int32_t raw;
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axis != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_read_cooked(), invalid state");

  msg = thermo_read_raw(devp, &raw);

  if (msg == MSG_OK) {
    *axis = (raw * devp->thermosensitivity) - devp->thermobias;
  }

  return msg;
}

/**
 * @brief   Set bias values for the BaseThermometer.
 * @note    Bias must be expressed as °C.
 * @note    The bias buffer must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t thermo_set_bias(void *ip, float *bp) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (bp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_set_bias(), invalid state");

  devp->thermobias = *bp;

  return msg;
}

/**
 * @brief   Reset bias values for the BaseThermometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t thermo_reset_bias(void *ip) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_reset_bias(), invalid state");

  devp->thermobias = SHT40AD1B_THERMO_BIAS;

  return msg;
}

/**
 * @brief   Set sensitivity values for the BaseThermometer.
 * @note    Sensitivity must be expressed as °C/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t thermo_set_sensitivity(void *ip, float *sp) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (sp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_set_sensitivity(), invalid state");

  devp->thermosensitivity = *sp;

  return msg;
}

/**
 * @brief   Reset sensitivity values for the BaseThermometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] ip        pointer to @p BaseThermometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t thermo_reset_sensitivity(void *ip) {
  SHT40AD1BDriver* devp;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(SHT40AD1BDriver*, (BaseThermometer*)ip);

  osalDbgAssert((devp->state == SHT40AD1B_READY),
                "thermo_reset_sensitivity(), invalid state");

  devp->thermosensitivity = SHT40AD1B_THERMO_SENS;

  return msg;
}

static const struct SHT40AD1BVMT vmt_device = {
  (size_t)0
};

static const struct BaseHygrometerVMT vmt_hygrometer = {
  sizeof(struct SHT40AD1BVMT*),
  hygro_get_axes_number, hygro_read_raw, hygro_read_cooked,
  hygro_set_bias, hygro_reset_bias, hygro_set_sensitivity,
  hygro_reset_sensitivity
};

static const struct BaseThermometerVMT vmt_thermometer = {
  sizeof(struct SHT40AD1BVMT*) + sizeof(BaseHygrometer),
  thermo_get_axes_number, thermo_read_raw, thermo_read_cooked,
  thermo_set_bias, thermo_reset_bias, thermo_set_sensitivity,
  thermo_reset_sensitivity
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Initializes an instance.
 *
 * @param[out] devp     pointer to the @p SHT40AD1BDriver object
 *
 * @init
 */
void sht40ad1bObjectInit(SHT40AD1BDriver *devp) {

  devp->vmt = &vmt_device;
  devp->hygro_if.vmt = &vmt_hygrometer;
  devp->thermo_if.vmt = &vmt_thermometer;

  devp->config = NULL;

  devp->hygroaxes = SHT40AD1B_HYGRO_NUMBER_OF_AXES;
  devp->thermoaxes = SHT40AD1B_THERMO_NUMBER_OF_AXES;

  devp->hygrobias = 0.0f;
  devp->thermobias = 0.0f;

  devp->state = SHT40AD1B_STOP;
}

/**
 * @brief   Configures and activates SHT40AD1B Complex Driver peripheral.
 *
 * @param[in] devp      pointer to the @p SHT40AD1BDriver object
 * @param[in] config    pointer to the @p SHT40AD1BConfig object
 *
 * @api
 */
void sht40ad1bStart(SHT40AD1BDriver *devp, const SHT40AD1BConfig *config) {
  osalDbgCheck((devp != NULL) && (config != NULL));

  osalDbgAssert((devp->state == SHT40AD1B_STOP) || (devp->state == SHT40AD1B_READY),
                 "sht40ad1bStart(), invalid state");

  devp->config = config;

#if SHT40AD1B_SHARED_I2C
  i2cAcquireBus(devp->config->i2cp);
#endif /* SHT40AD1B_SHARED_I2C */

  /* Intializing the I2C. */
  i2cStart(devp->config->i2cp, devp->config->i2ccfg);

#if SHT40AD1B_SHARED_I2C
    i2cReleaseBus(devp->config->i2cp);
#endif /* SHT40AD1B_SHARED_I2C */

    if(devp->config->hygrosensitivity == NULL) {
      devp->hygrosensitivity = SHT40AD1B_HYGRO_SENS;
    }
    else{
      /* Taking hygrometer sensitivity from user configurations */
      devp->hygrosensitivity = *(devp->config->hygrosensitivity);
    }

    if(devp->config->hygrobias == NULL) {
      devp->hygrobias = SHT40AD1B_HYGRO_BIAS;
    }
    else{
      /* Taking hygrometer bias from user configurations */
      devp->hygrobias = *(devp->config->hygrobias);
    }

    if(devp->config->thermosensitivity == NULL) {
      devp->thermosensitivity = SHT40AD1B_THERMO_SENS;
    }
    else{
      /* Taking thermometer sensitivity from user configurations */
      devp->thermosensitivity = *(devp->config->thermosensitivity);
    }

    if(devp->config->thermobias == NULL) {
      devp->thermobias = SHT40AD1B_THERMO_BIAS;
    }
    else{
      /* Taking thermometer bias from user configurations */
      devp->thermobias = *(devp->config->thermobias);
    }
  /* This is the MEMS transient recovery time */
  osalThreadSleepMilliseconds(5);

  devp->state = SHT40AD1B_READY;
}

/**
 * @brief   Deactivates the SHT40AD1B Complex Driver peripheral.
 *
 * @param[in] devp       pointer to the @p SHT40AD1BDriver object
 *
 * @api
 */
void sht40ad1bStop(SHT40AD1BDriver *devp) {

  osalDbgCheck(devp != NULL);

  osalDbgAssert((devp->state == SHT40AD1B_STOP) || (devp->state == SHT40AD1B_READY),
                "sht40ad1bStop(), invalid state");

  if (devp->state == SHT40AD1B_READY) {

#if SHT40AD1B_SHARED_I2C
  i2cAcquireBus(devp->config->i2cp);
  i2cStart(devp->config->i2cp, devp->config->i2ccfg);
#endif /* SHT40AD1B_SHARED_I2C */

  i2cStop(devp->config->i2cp);
#if SHT40AD1B_SHARED_I2C
  i2cReleaseBus(devp->config->i2cp);
#endif /* SHT40AD1B_SHARED_I2C */
  }
  devp->state = SHT40AD1B_STOP;
}
/** @} */
