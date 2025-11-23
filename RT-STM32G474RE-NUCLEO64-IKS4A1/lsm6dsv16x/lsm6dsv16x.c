/*
    ChibiOS - Copyright (C) 2016..2024 Giovanni Francesco Comune

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
 * @file    lsm6dsv16x.c
 * @brief   LSM6DSV16X MEMS interface module code.
 *
 * @addtogroup LSM6DSV16X
 * @ingroup EX_ST
 * @{
 */

#include "hal.h"
#include "lsm6dsv16x.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

#if (LSM6DSV16X_USE_I2C) || defined(__DOXYGEN__)
/**
 * @brief   Reads registers value using I2C.
 * @pre     The I2C interface must be initialized and the driver started.
 * @note    IF_ADD_INC bit must be 1 in CTRL_REG8
 *
 * @param[in]  i2cp      pointer to the I2C interface
 * @param[in]  sad       slave address without R bit
 * @param[in]  reg       first sub-register address
 * @param[out] rxbuf     pointer to an output buffer
 * @param[in]  n         number of consecutive register to read
 * @return               the operation status.
 * @notapi
 */
msg_t lsm6dsv16xI2CReadRegister(I2CDriver *i2cp, lsm6dsv16x_sad_t sad, uint8_t reg,
                             uint8_t* rxbuf, size_t n) {

  return i2cMasterTransmitTimeout(i2cp, sad, &reg, 1, rxbuf, n,
                                  TIME_INFINITE);
}

/**
 * @brief   Writes a value into a register using I2C.
 * @pre     The I2C interface must be initialized and the driver started.
 *
 * @param[in] i2cp       pointer to the I2C interface
 * @param[in] sad        slave address without R bit
 * @param[in] txbuf      buffer containing sub-address value in first position
 *                       and values to write
 * @param[in] n          size of txbuf less one (not considering the first
 *                       element)
 * @return               the operation status.
 * @notapi
 */
#define lsm6dsv16xI2CWriteRegister(i2cp, sad, txbuf, n)                        \
        i2cMasterTransmitTimeout(i2cp, sad, txbuf, n + 1, NULL, 0,          \
                                  TIME_INFINITE)
#endif /* LSM6DSV16X_USE_I2C */

/**
 * @brief   Return the number of axes of the BaseAccelerometer.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 *
 * @return              the number of axes.
 */
static size_t acc_get_axes_number(void *ip) {
  (void)ip;

  return LSM6DSV16X_ACC_NUMBER_OF_AXES;
}

/**
 * @brief   Retrieves raw data from the BaseAccelerometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t acc_read_raw(void *ip, int32_t axes[]) {
  LSM6DSV16XDriver* devp;
  uint8_t buff [LSM6DSV16X_ACC_NUMBER_OF_AXES * 2], i;
  int16_t tmp;
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_read_raw(), invalid state");
#if LSM6DSV16X_USE_I2C
  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "acc_read_raw(), channel not ready");

#if LSM6DSV16X_SHARED_I2C
  i2cAcquireBus(devp->config->i2cp);
  i2cStart(devp->config->i2cp,
           devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

  msg = lsm6dsv16xI2CReadRegister(devp->config->i2cp, devp->config->slaveaddress,
                               LSM6DSV16X_AD_OUTX_L_XL, buff,
                               LSM6DSV16X_ACC_NUMBER_OF_AXES * 2);

#if LSM6DSV16X_SHARED_I2C
  i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */
  if(msg == MSG_OK)
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
      tmp = buff[2 * i] + (buff[2 * i + 1] << 8);
      axes[i] = (int32_t)tmp;
    }
  return msg;
}

/**
 * @brief   Retrieves cooked data from the BaseAccelerometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as milli-G.
 * @note    The axes array must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 * @param[out] axes     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t acc_read_cooked(void *ip, float axes[]) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  int32_t raw[LSM6DSV16X_ACC_NUMBER_OF_AXES];
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_read_cooked(), invalid state");

  msg = acc_read_raw(ip, raw);
  for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
    axes[i] = (raw[i] * devp->accsensitivity[i]) - devp->accbias[i];
  }
  return msg;
}

/**
 * @brief   Set bias values for the BaseAccelerometer.
 * @note    Bias must be expressed as milli-G.
 * @note    The bias buffer must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t acc_set_bias(void *ip, float *bp) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (bp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_set_bias(), invalid state");

  for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
    devp->accbias[i] = bp[i];
  }
  return msg;
}

/**
 * @brief   Reset bias values for the BaseAccelerometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t acc_reset_bias(void *ip) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_reset_bias(), invalid state");

  for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
    devp->accbias[i] = LSM6DSV16X_ACC_BIAS;
  return msg;
}

/**
 * @brief   Set sensitivity values for the BaseAccelerometer.
 * @note    Sensitivity must be expressed as milli-G/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t acc_set_sensivity(void *ip, float *sp) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgCheck((ip != NULL) && (sp != NULL));

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_set_sensivity(), invalid state");

  for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
    devp->accsensitivity[i] = sp[i];
  }
  return msg;
}

/**
 * @brief   Reset sensitivity values for the BaseAccelerometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] ip        pointer to @p BaseAccelerometer interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 */
static msg_t acc_reset_sensivity(void *ip) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseAccelerometer*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_reset_sensivity(), invalid state");

  if(devp->config->accfullscale == LSM6DSV16X_2g)
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_2G;
  else if(devp->config->accfullscale == LSM6DSV16X_4g)
	for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_4G;
  else if(devp->config->accfullscale == LSM6DSV16X_8g)
	for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_8G;
  else if(devp->config->accfullscale == LSM6DSV16X_16g)
	for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_16G;
  else {
    osalDbgAssert(FALSE, "reset_sensivity(), accelerometer full scale issue");
    msg = MSG_RESET;
  }
  return msg;
}

/**
 * @brief   Changes the LSM6DSV16XDriver accelerometer fullscale value.
 * @note    This function also rescale sensitivities and biases based on
 *          previous and next fullscale value.
 * @note    A recalibration is highly suggested after calling this function.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver interface.
 * @param[in] fs        new fullscale value.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 */
static msg_t acc_set_full_scale(LSM6DSV16XDriver *devp, lsm6dsv16x_acc_fs_t fs) {
  float newfs, scale;
  uint8_t i, buff[2];
  msg_t msg;

  osalDbgCheck(devp != NULL);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "acc_set_full_scale(), invalid state");
  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "acc_set_full_scale(), channel not ready");

  /* Computing new fullscale value.*/
  if(fs == LSM6DSV16X_2g) {
    newfs = LSM6DSV16X_ACC_2G;
  }
  else if(fs == LSM6DSV16X_4g) {
    newfs = LSM6DSV16X_ACC_4G;
  }
  else if(fs == LSM6DSV16X_8g) {
    newfs = LSM6DSV16X_ACC_8G;
  }
  else if(fs == LSM6DSV16X_16g) {
    newfs = LSM6DSV16X_ACC_16G;
  }
  else {
    msg = MSG_RESET;
    return msg;
  }

  if(newfs != devp->accfullscale) {
    /* Computing scale value.*/
    scale = newfs / devp->accfullscale;
    devp->accfullscale = newfs;

lsm6dsv16x_ctrl8_t ctrl8;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL8_XL, (uint8_t*)&ctrl8, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl8.fs_xl = (uint8_t)fs & 0x3U;
	buff[0] = LSM6DSV16X_AD_CTRL8_XL;
	buff[1] = *(uint8_t*)&ctrl8;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    msg = lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               buff, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

    if(msg != MSG_OK)
      return msg;

    /* Scaling sensitivity and bias. Re-calibration is suggested anyway.*/
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
      devp->accsensitivity[i] *= scale;
      devp->accbias[i] *= scale;
    }
  }
  return msg;
}

/**
 * @brief   Return the number of axes of the BaseGyroscope.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 *
 * @return              the number of axes.
 */
static size_t gyro_get_axes_number(void *ip) {
  (void)ip;

  return LSM6DSV16X_GYRO_NUMBER_OF_AXES;
}

/**
 * @brief   Retrieves raw data from the BaseGyroscope.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t gyro_read_raw(void *ip, int32_t axes[LSM6DSV16X_GYRO_NUMBER_OF_AXES]) {
  LSM6DSV16XDriver* devp;
  int16_t tmp;
  uint8_t i, buff [2 * LSM6DSV16X_GYRO_NUMBER_OF_AXES];
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_read_raw(), invalid state");
#if LSM6DSV16X_USE_I2C
  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "gyro_read_raw(), channel not ready");

#if LSM6DSV16X_SHARED_I2C
  i2cAcquireBus(devp->config->i2cp);
  i2cStart(devp->config->i2cp,
           devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

  msg = lsm6dsv16xI2CReadRegister(devp->config->i2cp, devp->config->slaveaddress,
                               LSM6DSV16X_AD_OUTX_L_G, buff,
                               LSM6DSV16X_GYRO_NUMBER_OF_AXES * 2);

#if	LSM6DSV16X_SHARED_I2C
  i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      tmp = buff[2 * i] + (buff[2 * i + 1] << 8);
      axes[i] = (int32_t)tmp;
    }
  return msg;
}

/**
 * @brief   Retrieves cooked data from the BaseGyroscope.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as DPS.
 * @note    The axes array must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 * @param[out] axes     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 */
static msg_t gyro_read_cooked(void *ip, float axes[]) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  int32_t raw[LSM6DSV16X_GYRO_NUMBER_OF_AXES];
  msg_t msg;

  osalDbgCheck((ip != NULL) && (axes != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_read_cooked(), invalid state");

  msg = gyro_read_raw(ip, raw);
  for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++){
    axes[i] = (raw[i] * devp->gyrosensitivity[i]) - devp->gyrobias[i];
  }
  return msg;
}

/**
 * @brief   Samples bias values for the BaseGyroscope.
 * @note    The LSM6DSV16X shall not be moved during the whole procedure.
 * @note    After this function internal bias is automatically updated.
 * @note    The behavior of this function depends on @p LSM6DSV16X_BIAS_ACQ_TIMES
 *          and @p LSM6DSV16X_BIAS_SETTLING_US.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t gyro_sample_bias(void *ip) {
  LSM6DSV16XDriver* devp;
  uint32_t i, j;
  int32_t raw[LSM6DSV16X_GYRO_NUMBER_OF_AXES];
  int32_t buff[LSM6DSV16X_GYRO_NUMBER_OF_AXES] = {0, 0, 0};
  msg_t msg;
	
  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_sample_bias(), invalid state");
#if LSM6DSV16X_USE_I2C
  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "gyro_sample_bias(), channel not ready");
#endif

  for(i = 0; i < LSM6DSV16X_GYRO_BIAS_ACQ_TIMES; i++){
    msg = gyro_read_raw(ip, raw);
		if(msg != MSG_OK)
			return msg;
    for(j = 0; j < LSM6DSV16X_GYRO_NUMBER_OF_AXES; j++){
      buff[j] += raw[j];
    }
    osalThreadSleepMicroseconds(LSM6DSV16X_GYRO_BIAS_SETTLING_US);
  }

  for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++){
    devp->gyrobias[i] = (buff[i] / LSM6DSV16X_GYRO_BIAS_ACQ_TIMES);
    devp->gyrobias[i] *= devp->gyrosensitivity[i];
  }
  return msg;
}

/**
 * @brief   Set bias values for the BaseGyroscope.
 * @note    Bias must be expressed as DPS.
 * @note    The bias buffer must be at least the same size of the BaseGyroscope
 *          axes number.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t gyro_set_bias(void *ip, float *bp) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (bp != NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_set_bias(), invalid state");

  for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
    devp->gyrobias[i] = bp[i];
  }
  return msg;
}

/**
 * @brief   Reset bias values for the BaseGyroscope.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t gyro_reset_bias(void *ip) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_reset_bias(), invalid state");

  for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
    devp->gyrobias[i] = LSM6DSV16X_GYRO_BIAS;
  return msg;
}

/**
 * @brief   Set sensitivity values for the BaseGyroscope.
 * @note    Sensitivity must be expressed as DPS/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 */
static msg_t gyro_set_sensivity(void *ip, float *sp) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck((ip != NULL) && (sp !=NULL));

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_set_sensivity(), invalid state");

  for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
    devp->gyrosensitivity[i] = sp[i];
  }
  return msg;
}

/**
 * @brief   Reset sensitivity values for the BaseGyroscope.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] ip        pointer to @p BaseGyroscope interface.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 */
static msg_t gyro_reset_sensivity(void *ip) {
  LSM6DSV16XDriver* devp;
  uint32_t i;
  msg_t msg = MSG_OK;

  osalDbgCheck(ip != NULL);

  /* Getting parent instance pointer.*/
  devp = objGetInstance(LSM6DSV16XDriver*, (BaseGyroscope*)ip);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_reset_sensivity(), invalid state");
  if(devp->config->gyrofullscale == LSM6DSV16X_125dps)
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_125DPS;
  else if(devp->config->gyrofullscale == LSM6DSV16X_250dps)
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_250DPS;
  else if(devp->config->gyrofullscale == LSM6DSV16X_500dps)
	for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_500DPS;
  else if(devp->config->gyrofullscale == LSM6DSV16X_1000dps)
	for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_1000DPS;
  else if(devp->config->gyrofullscale == LSM6DSV16X_2000dps)
	for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_2000DPS;
  else {
    osalDbgAssert(FALSE, "gyro_reset_sensivity(), full scale issue");
    return MSG_RESET;
  }
  return msg;
}

/**
 * @brief   Changes the LSM6DSV16XDriver gyroscope fullscale value.
 * @note    This function also rescale sensitivities and biases based on
 *          previous and next fullscale value.
 * @note    A recalibration is highly suggested after calling this function.
 *
 * @param[in] devp      pointer to @p BaseGyroscope interface.
 * @param[in] fs        new fullscale value.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 */
static msg_t gyro_set_full_scale(LSM6DSV16XDriver *devp, lsm6dsv16x_gyro_fs_t fs) {
  float newfs, scale;
  uint8_t i, buff[2];
  msg_t msg = MSG_OK;

  osalDbgCheck(devp != NULL);

  osalDbgAssert((devp->state == LSM6DSV16X_READY),
                "gyro_set_full_scale(), invalid state");
#if LSM6DSV16X_USE_I2C
  osalDbgAssert((devp->config->i2cp->state == I2C_READY),
                "gyro_set_full_scale(), channel not ready");
#endif

  if(fs == LSM6DSV16X_125dps) {
    newfs = LSM6DSV16X_GYRO_125DPS;
  }
  else if(fs == LSM6DSV16X_250dps) {
    newfs = LSM6DSV16X_GYRO_250DPS;
  }
  else if(fs == LSM6DSV16X_500dps) {
    newfs = LSM6DSV16X_GYRO_500DPS;
  }
  else if(fs == LSM6DSV16X_1000dps) {
    newfs = LSM6DSV16X_GYRO_1000DPS;
  }
  else if(fs == LSM6DSV16X_2000dps) {
    newfs = LSM6DSV16X_GYRO_2000DPS;
  }
  else if(fs == LSM6DSV16X_4000dps) {
      newfs = LSM6DSV16X_GYRO_4000DPS;
    }
  else {
    return MSG_RESET;
  }

  if(newfs != devp->gyrofullscale) {
    scale = newfs / devp->gyrofullscale;
    devp->gyrofullscale = newfs;

lsm6dsv16x_ctrl8_t ctrl8;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL8_XL, (uint8_t*)&ctrl8, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl8.fs_xl = (uint8_t)fs & 0x3U;
	buff[0] = LSM6DSV16X_AD_CTRL8_XL;
	buff[1] = *(uint8_t*)&ctrl8;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               buff, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

    /* Scaling sensitivity and bias. Re-calibration is suggested anyway. */
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      devp->gyrosensitivity[i] *= scale;
      devp->gyrobias[i] *= scale;
    }
  }
  return msg;
}

static const struct LSM6DSV16XVMT vmt_device = {
  (size_t)0,
  acc_set_full_scale, gyro_set_full_scale
};

static const struct BaseAccelerometerVMT vmt_accelerometer = {
  sizeof(struct LSM6DSV16XVMT*),
  acc_get_axes_number, acc_read_raw, acc_read_cooked,
  acc_set_bias, acc_reset_bias, acc_set_sensivity, acc_reset_sensivity
};

static const struct BaseGyroscopeVMT vmt_gyroscope = {
  sizeof(struct LSM6DSV16XVMT*) + sizeof(BaseAccelerometer),
  gyro_get_axes_number, gyro_read_raw, gyro_read_cooked,
  gyro_sample_bias, gyro_set_bias, gyro_reset_bias,
  gyro_set_sensivity, gyro_reset_sensivity
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Initializes an instance.
 *
 * @param[out] devp     pointer to the @p LSM6DSV16XDriver object
 *
 * @init
 */
void lsm6dsv16xObjectInit(LSM6DSV16XDriver *devp) {
  devp->vmt = &vmt_device;
  devp->acc_if.vmt = &vmt_accelerometer;
  devp->gyro_if.vmt = &vmt_gyroscope;

  devp->config = NULL;

  devp->accaxes = LSM6DSV16X_ACC_NUMBER_OF_AXES;
  devp->gyroaxes = LSM6DSV16X_GYRO_NUMBER_OF_AXES;

  devp->state = LSM6DSV16X_STOP;
}

/**
 * @brief   Resets LSM6DSV16X registers.
 *
 * @param[in] devp      pointer to the @p LSM6DSV16XDriver object
 * @param[in] config    pointer to the @p LSM6DSV16XConfig object
 *
 * @api
 */
void lsm6dsv16xReset(LSM6DSV16XDriver *devp)
{
  lsm6dsv16x_ctrl3_t ctrl3;
  lsm6dsv16x_func_cfg_access_t func_cfg_access;

  uint8_t cr1[2];
  uint8_t cr2[2];

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating registers.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL3_C, (uint8_t*)&ctrl3, 1);
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_FUNC_CFG_ACCESS, (uint8_t*)&func_cfg_access, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl3.boot = ((uint8_t)1U & 0x04U) >> 2;
    ctrl3.sw_reset = ((uint8_t)1U & 0x02U) >> 1;
	cr1[0] = LSM6DSV16X_AD_CTRL3_C;
	cr1[1] = *(uint8_t*)&ctrl3;

    func_cfg_access.sw_por = (uint8_t)1U & 0x01U;
	cr2[0] = LSM6DSV16X_AD_FUNC_CFG_ACCESS;
	cr2[1] = *(uint8_t*)&func_cfg_access;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr1, 1);
    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr2, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

lsm6dsv16x_reset_t rst;

do 
{
#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL3_C, (uint8_t*)&ctrl3, 1);
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_FUNC_CFG_ACCESS, (uint8_t*)&func_cfg_access, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

    switch ((ctrl3.sw_reset << 2) + (ctrl3.boot << 1) + func_cfg_access.sw_por)
    {
      case LSM6DSV16X_RESET_READY:
        rst = LSM6DSV16X_RESET_READY;
        break;
  
      case LSM6DSV16X_RESET_GLOBAL_RST:
        rst = LSM6DSV16X_RESET_GLOBAL_RST;
        break;
  
      case LSM6DSV16X_RESET_RESTORE_CAL_PARAM:
        rst = LSM6DSV16X_RESET_RESTORE_CAL_PARAM;
        break;
  
      case LSM6DSV16X_RESET_RESTORE_CTRL_REGS:
        rst = LSM6DSV16X_RESET_RESTORE_CTRL_REGS;
        break;
  
      default:
        rst = LSM6DSV16X_RESET_GLOBAL_RST;
        break;
    }
  }
  while (rst != LSM6DSV16X_RESET_READY);
}

/**
 * @brief   Configures and activates LSM6DSV16X Complex Driver peripheral.
 *
 * @param[in] devp      pointer to the @p LSM6DSV16XDriver object
 * @param[in] config    pointer to the @p LSM6DSV16XConfig object
 *
 * @api
 */
void lsm6dsv16xStart(LSM6DSV16XDriver *devp, const LSM6DSV16XConfig *config) {
  uint32_t i;
  uint8_t cr[11];
  osalDbgCheck((devp != NULL) && (config != NULL));

  osalDbgAssert((devp->state == LSM6DSV16X_STOP) ||
                (devp->state == LSM6DSV16X_READY),
                "lsm6dsv16xStart(), invalid state");

  devp->config = config;

  lsm6dsv16xReset(devp);

  lsm6dsv16x_ctrl3_t ctrl3;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL3_C, (uint8_t*)&ctrl3, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl3.if_inc = 1U;
	ctrl3.bdu = 1U;
	cr[0] = LSM6DSV16X_AD_CTRL3_C;
	cr[1] = *(uint8_t*)&ctrl3;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

// setting acceleration data rate

lsm6dsv16x_ctrl1_t ctrl1;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL1_XL, (uint8_t*)&ctrl1, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl1.odr_xl = (uint8_t)config->accoutdatarate & 0x0Fu;
	cr[0] = LSM6DSV16X_AD_CTRL1_XL;
	cr[1] = *(uint8_t*)&ctrl1;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

// setting acceleration full scale

lsm6dsv16x_ctrl8_t ctrl8;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL8_XL, (uint8_t*)&ctrl8, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl8.fs_xl = (uint8_t)devp->config->accfullscale & 0x3U;
	cr[0] = LSM6DSV16X_AD_CTRL8_XL;
	cr[1] = *(uint8_t*)&ctrl8;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

// setting gyroscope data rate

lsm6dsv16x_ctrl2_t ctrl2;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL2_G, (uint8_t*)&ctrl2, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl2.odr_g = (uint8_t)devp->config->gyrooutdatarate & 0x0Fu;
	cr[0] = LSM6DSV16X_AD_CTRL2_G;
	cr[1] = *(uint8_t*)&ctrl2;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

lsm6dsv16x_haodr_cfg_t haodr;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_HAODR_CFG, (uint8_t*)&haodr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	haodr.haodr_sel = ((uint8_t)devp->config->gyrooutdatarate >> 4) & 0xFU;;
	cr[0] = LSM6DSV16X_HAODR_CFG;
	cr[1] = *(uint8_t*)&haodr;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

// setting gyroscope full scale

lsm6dsv16x_ctrl6_t ctrl6;

#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    /* Updating register.*/
    lsm6dsv16xI2CReadRegister(devp->config->i2cp,
                                 devp->config->slaveaddress,
                                 LSM6DSV16X_AD_CTRL6_C, (uint8_t*)&ctrl6, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

	ctrl6.fs_g = (uint8_t)devp->config->gyrofullscale & 0xfu;
	cr[0] = LSM6DSV16X_AD_CTRL6_C;
	cr[1] = *(uint8_t*)&ctrl6;


#if LSM6DSV16X_USE_I2C
#if	LSM6DSV16X_SHARED_I2C
		i2cAcquireBus(devp->config->i2cp);
		i2cStart(devp->config->i2cp,
						 devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */

    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                               cr, 1);

#if	LSM6DSV16X_SHARED_I2C
		i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */

  /* Storing sensitivity according to user settings */
  if(devp->config->accfullscale == LSM6DSV16X_2g) {
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
     if(devp->config->accsensitivity == NULL)
       devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_2G;
     else
       devp->accsensitivity[i] = devp->config->accsensitivity[i];
    }
    devp->accfullscale = LSM6DSV16X_ACC_2G;
  }
  else if(devp->config->accfullscale == LSM6DSV16X_4g) {
   for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
     if(devp->config->accsensitivity == NULL)
       devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_4G;
     else
       devp->accsensitivity[i] = devp->config->accsensitivity[i];
    }
   devp->accfullscale = LSM6DSV16X_ACC_4G;
  }
  else if(devp->config->accfullscale == LSM6DSV16X_8g) {
   for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
     if(devp->config->accsensitivity == NULL)
       devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_8G;
     else
       devp->accsensitivity[i] = devp->config->accsensitivity[i];
    }
   devp->accfullscale = LSM6DSV16X_ACC_8G;
  }
  else if(devp->config->accfullscale == LSM6DSV16X_16g) {
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
      if(devp->config->accsensitivity == NULL)
        devp->accsensitivity[i] = LSM6DSV16X_ACC_SENS_16G;
      else
        devp->accsensitivity[i] = devp->config->accsensitivity[i];
    }
    devp->accfullscale = LSM6DSV16X_ACC_16G;
  }
  else
    osalDbgAssert(FALSE, "lsm6dsv16xStart(), accelerometer full scale issue");

  /* Storing bias information */
  if(devp->config->accbias != NULL)
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accbias[i] = devp->config->accbias[i];
  else
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++)
      devp->accbias[i] = LSM6DSV16X_ACC_BIAS;

  if(devp->config->gyrofullscale == LSM6DSV16X_125dps) {
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      if(devp->config->gyrosensitivity == NULL)
        devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_125DPS;
      else
        devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
    }
    devp->gyrofullscale = LSM6DSV16X_GYRO_125DPS;
  }
  else if(devp->config->gyrofullscale == LSM6DSV16X_250dps) {
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      if(devp->config->gyrosensitivity == NULL)
        devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_250DPS;
      else
        devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
    }
    devp->gyrofullscale = LSM6DSV16X_GYRO_250DPS;
  }
  else if(devp->config->gyrofullscale == LSM6DSV16X_500dps) {
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      if(devp->config->gyrosensitivity == NULL)
        devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_500DPS;
      else
        devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
    }
    devp->gyrofullscale = LSM6DSV16X_GYRO_500DPS;
  }
  else if(devp->config->gyrofullscale == LSM6DSV16X_1000dps) {
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      if(devp->config->gyrosensitivity == NULL)
        devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_1000DPS;
      else
        devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
    }
    devp->gyrofullscale = LSM6DSV16X_GYRO_1000DPS;
  }
  else if(devp->config->gyrofullscale == LSM6DSV16X_2000dps) {
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      if(devp->config->gyrosensitivity == NULL)
        devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_2000DPS;
      else
        devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
    }
    devp->gyrofullscale = LSM6DSV16X_GYRO_2000DPS;
  }
  else if(devp->config->gyrofullscale == LSM6DSV16X_4000dps) {
      for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
        if(devp->config->gyrosensitivity == NULL)
          devp->gyrosensitivity[i] = LSM6DSV16X_GYRO_SENS_4000DPS;
        else
          devp->gyrosensitivity[i] = devp->config->gyrosensitivity[i];
      }
      devp->gyrofullscale = LSM6DSV16X_GYRO_4000DPS;
    }
  else
    osalDbgAssert(FALSE, "lsm6dsv16xStart(), gyroscope full scale issue");

  /* Storing bias information */
  if(devp->config->gyrobias != NULL)
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrobias[i] = devp->config->gyrobias[i];
  else
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++)
      devp->gyrobias[i] = LSM6DSV16X_GYRO_BIAS;

  /* This is the MEMS transient recovery time */
  osalThreadSleepMilliseconds(5);

  devp->state = LSM6DSV16X_READY;
}

/**
 * @brief   Deactivates the LSM6DSV16X Complex Driver peripheral.
 *
 * @param[in] devp       pointer to the @p LSM6DSV16XDriver object
 *
 * @api
 */
void lsm6dsv16xStop(LSM6DSV16XDriver *devp) {
  uint8_t cr[2];

  osalDbgCheck(devp != NULL);

  osalDbgAssert((devp->state == LSM6DSV16X_STOP) || (devp->state == LSM6DSV16X_READY),
                "lsm6dsv16xStop(), invalid state");

  if (devp->state == LSM6DSV16X_READY) {
#if LSM6DSV16X_USE_I2C
#if LSM6DSV16X_SHARED_I2C
    i2cAcquireBus(devp->config->i2cp);
    i2cStart(devp->config->i2cp, devp->config->i2ccfg);
#endif /* LSM6DSV16X_SHARED_I2C */


    cr[0] = LSM6DSV16X_AD_CTRL1_XL;
    /* Disabling accelerometer.*/
    cr[1] = LSM6DSV16X_ACC_ODR_OFF;
    /* Disabling gyroscope.*/
    cr[2] = LSM6DSV16X_GYRO_ODR_OFF;
    lsm6dsv16xI2CWriteRegister(devp->config->i2cp, devp->config->slaveaddress,
                            cr, 2);

    i2cStop(devp->config->i2cp);
#if LSM6DSV16X_SHARED_I2C
    i2cReleaseBus(devp->config->i2cp);
#endif /* LSM6DSV16X_SHARED_I2C */
#endif /* LSM6DSV16X_USE_I2C */
  }
  devp->state = LSM6DSV16X_STOP;
}
/** @} */
