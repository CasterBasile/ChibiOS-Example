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
 * @file    lsm6dsv16x.h
 * @brief   LSM6DSV16X MEMS interface module header.
 *
 * @addtogroup LSM6DSV16X
 * @ingroup EX_ST
 * @{
 */
#ifndef _LSM6DSV16X_H_
#define _LSM6DSV16X_H_

#include "ex_accelerometer.h"
#include "ex_gyroscope.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Version identification
 * @{
 */
/**
 * @brief   LSM6DSV16X driver version string.
 */
#define EX_LSM6DSV16X_VERSION                  "1.0.1"

#define DRV_LITTLE_ENDIAN  __ORDER_LITTLE_ENDIAN__
#define DRV_BIG_ENDIAN     __ORDER_BIG_ENDIAN__

#define DRV_BYTE_ORDER    DRV_LITTLE_ENDIAN

/**
 * @brief   LSM6DSV16X driver version major number.
 */
#define EX_LSM6DSV16X_MAJOR                    1

/**
 * @brief   LSM6DSV16X driver version minor number.
 */
#define EX_LSM6DSV16X_MINOR                    0

/**
 * @brief   LSM6DSV16X driver version patch number.
 */
#define EX_LSM6DSV16X_PATCH                    1
/** @} */

/**
 * @brief   LSM6DSV16X accelerometer subsystem characteristics.
 * @note    Sensitivity is expressed as milli-G/LSB whereas
 *          1 milli-G = 0.00980665 m/s^2.
 * @note    Bias is expressed as milli-G.
 *
 * @{
 */
#define LSM6DSV16X_ACC_NUMBER_OF_AXES          3U

#define LSM6DSV16X_ACC_2G                      2.0f
#define LSM6DSV16X_ACC_4G                      4.0f
#define LSM6DSV16X_ACC_8G                      8.0f
#define LSM6DSV16X_ACC_16G                     16.0f

#define LSM6DSV16X_ACC_SENS_2G                 0.061f
#define LSM6DSV16X_ACC_SENS_4G                 0.122f
#define LSM6DSV16X_ACC_SENS_8G                 0.244f
#define LSM6DSV16X_ACC_SENS_16G                0.488f

#define LSM6DSV16X_ACC_BIAS                    0.0f
/** @} */

/**
 * @brief   L3GD20 gyroscope system characteristics.
 * @note    Sensitivity is expressed as DPS/LSB whereas DPS stand for Degree
 *          per second [�/s].
 * @note    Bias is expressed as DPS.
 *
 * @{
 */
#define LSM6DSV16X_GYRO_NUMBER_OF_AXES         3U

#define LSM6DSV16X_GYRO_125DPS                 125.0f
#define LSM6DSV16X_GYRO_250DPS                 250.0f
#define LSM6DSV16X_GYRO_500DPS                 500.0f
#define LSM6DSV16X_GYRO_1000DPS                1000.0f
#define LSM6DSV16X_GYRO_2000DPS                2000.0f
#define LSM6DSV16X_GYRO_4000DPS                4000.0f

#define LSM6DSV16X_GYRO_SENS_125DPS            0.004375f
#define LSM6DSV16X_GYRO_SENS_250DPS            0.008750f
#define LSM6DSV16X_GYRO_SENS_500DPS            0.017500f
#define LSM6DSV16X_GYRO_SENS_1000DPS           0.035000f
#define LSM6DSV16X_GYRO_SENS_2000DPS           0.070000f
#define LSM6DSV16X_GYRO_SENS_4000DPS           0.140000f

#define LSM6DSV16X_GYRO_BIAS                   0.0f
/** @} */

/**
 * @name   LSM6DSV16X communication interfaces related bit masks
 * @{
 */
#define LSM6DSV16X_DI_MASK                     0xFF
#define LSM6DSV16X_DI(n)                       (1 << n)
#define LSM6DSV16X_AD_MASK                     0x7F
#define LSM6DSV16X_AD(n)                       (1 << n)
#define LSM6DSV16X_MS                          (1 << 7)
/** @} */

/**
 * @name   LSM6DSV16X register addresses
 * @{
 */
#define LSM6DSV16X_AD_FUNC_CFG_ACCESS          0x01
#define LSM6DSV16X_AD_SENSOR_SYNC_TIME_FRAME   0x04
#define LSM6DSV16X_AD_SENSOR_SYNC_RES_RATIO    0x05
#define LSM6DSV16X_AD_FIFO_CTRL1               0x07
#define LSM6DSV16X_AD_FIFO_CTRL2               0x08
#define LSM6DSV16X_AD_FIFO_CTRL3               0x09
#define LSM6DSV16X_AD_FIFO_CTRL4               0x0A
#define LSM6DSV16X_AD_FIFO_CTRL5               0x0B
//#define LSM6DSV16X_AD_DRDY_PULSE_CFG_G         0x0B
#define LSM6DSV16X_AD_INT1_CTRL                0x0D
#define LSM6DSV16X_AD_INT2_CTRL                0x0E
#define LSM6DSV16X_AD_WHO_AM_I                 0x0F
#define LSM6DSV16X_AD_CTRL1_XL                 0x10
#define LSM6DSV16X_AD_CTRL2_G                  0x11
#define LSM6DSV16X_AD_CTRL3_C                  0x12
#define LSM6DSV16X_AD_CTRL4_C                  0x13
#define LSM6DSV16X_AD_CTRL5_C                  0x14
#define LSM6DSV16X_AD_CTRL6_C                  0x15
#define LSM6DSV16X_AD_CTRL7_G                  0x16
#define LSM6DSV16X_AD_CTRL8_XL                 0x17
#define LSM6DSV16X_AD_CTRL9_XL                 0x18
#define LSM6DSV16X_AD_CTRL10_C                 0x19
#define LSM6DSV16X_AD_MASTER_CONFIG            0x1A
#define LSM6DSV16X_AD_WAKE_UP_SRC              0x1B
#define LSM6DSV16X_AD_TAP_SRC                  0x1C
#define LSM6DSV16X_AD_D6D_SRC                  0x1D
#define LSM6DSV16X_AD_STATUS_REG               0x1E
#define LSM6DSV16X_AD_OUT_TEMP_L               0x20
#define LSM6DSV16X_AD_OUT_TEMP_H               0x21
#define LSM6DSV16X_AD_OUTX_L_G                 0x22
#define LSM6DSV16X_AD_OUTX_H_G                 0x23
#define LSM6DSV16X_AD_OUTY_L_G                 0x24
#define LSM6DSV16X_AD_OUTY_H_G                 0x25
#define LSM6DSV16X_AD_OUTZ_L_G                 0x26
#define LSM6DSV16X_AD_OUTZ_H_G                 0x27
#define LSM6DSV16X_AD_OUTX_L_XL                0x28
#define LSM6DSV16X_AD_OUTX_H_XL                0x29
#define LSM6DSV16X_AD_OUTY_L_XL                0x2A
#define LSM6DSV16X_AD_OUTY_H_XL                0x2B
#define LSM6DSV16X_AD_OUTZ_L_XL                0x2C
#define LSM6DSV16X_AD_OUTZ_H_XL                0x2D
#define LSM6DSV16X_AD_SENSORHUB1_REG           0x2E
#define LSM6DSV16X_AD_SENSORHUB2_REG           0x2F
#define LSM6DSV16X_AD_SENSORHUB3_REG           0x30
#define LSM6DSV16X_AD_SENSORHUB4_REG           0x31
#define LSM6DSV16X_AD_SENSORHUB5_REG           0x32
#define LSM6DSV16X_AD_SENSORHUB6_REG           0x33
#define LSM6DSV16X_AD_SENSORHUB7_REG           0x34
#define LSM6DSV16X_AD_SENSORHUB8_REG           0x35
#define LSM6DSV16X_AD_SENSORHUB9_REG           0x36
#define LSM6DSV16X_AD_SENSORHUB10_REG          0x37
#define LSM6DSV16X_AD_SENSORHUB11_REG          0x38
#define LSM6DSV16X_AD_SENSORHUB12_REG          0x39
#define LSM6DSV16X_AD_FIFO_STATUS1             0x3A
#define LSM6DSV16X_AD_FIFO_STATUS2             0x3B
#define LSM6DSV16X_AD_FIFO_STATUS3             0x3C
#define LSM6DSV16X_AD_FIFO_STATUS4             0x3D
#define LSM6DSV16X_AD_FIFO_DATA_OUT_L          0x3E
#define LSM6DSV16X_AD_FIFO_DATA_OUT_H          0x3F
#define LSM6DSV16X_AD_TIMESTAMP0_REG           0x40
#define LSM6DSV16X_AD_TIMESTAMP1_REG           0x41
#define LSM6DSV16X_AD_TIMESTAMP2_REG           0x42
#define LSM6DSV16X_AD_STEP_TIMESTAMP_L         0x49
#define LSM6DSV16X_AD_STEP_TIMESTAMP_H         0x4A
#define LSM6DSV16X_AD_STEP_COUNTER_L           0x4B
#define LSM6DSV16X_AD_STEP_COUNTER_H           0x4C
#define LSM6DSV16X_AD_SENSORHUB13_REG          0x4D
#define LSM6DSV16X_AD_SENSORHUB14_REG          0x4E
#define LSM6DSV16X_AD_SENSORHUB15_REG          0x4F
#define LSM6DSV16X_AD_SENSORHUB16_REG          0x50
#define LSM6DSV16X_AD_SENSORHUB17_REG          0x51
#define LSM6DSV16X_AD_SENSORHUB18_REG          0x52
#define LSM6DSV16X_AD_FUNC_SRC1                0x53
#define LSM6DSV16X_AD_FUNC_SRC2                0x54
#define LSM6DSV16X_AD_WRIST_TILT_IA            0x55
#define LSM6DSV16X_AD_TAP_CFG                  0x58
#define LSM6DSV16X_AD_TAP_THS_6D               0x59
#define LSM6DSV16X_AD_INT_DUR2                 0x5A
#define LSM6DSV16X_AD_WAKE_UP_THS              0x5B
#define LSM6DSV16X_AD_WAKE_UP_DUR              0x5C
#define LSM6DSV16X_AD_FREE_FALL                0x5D
#define LSM6DSV16X_AD_MD1_CFG                  0x5E
#define LSM6DSV16X_AD_MD2_CFG                  0x5F
#define LSM6DSV16X_AD_MASTER_CMD_CODE          0x60
#define LSM6DSV16X_AD_SENS_SYNC_SPI_ERROR_CODE 0x61
#define LSM6DSV16X_HAODR_CFG                   0x62U
#define LSM6DSV16X_AD_OUT_MAG_RAW_X_L          0x66
#define LSM6DSV16X_AD_OUT_MAG_RAW_X_H          0x67
#define LSM6DSV16X_AD_OUT_MAG_RAW_Y_L          0x68
#define LSM6DSV16X_AD_OUT_MAG_RAW_Y_H          0x69
#define LSM6DSV16X_AD_OUT_MAG_RAW_Z_L          0x6A
#define LSM6DSV16X_AD_OUT_MAG_RAW_Z_H          0x6B
#define LSM6DSV16X_AD_X_OFS_USR                0x73
#define LSM6DSV16X_AD_Y_OFS_USR                0x74
#define LSM6DSV16X_AD_Z_OFS_USR                0x75
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL1_XL register bits definitions
 * @{
 */
#define LSMDSL_CTRL1_XL_BW0_XL              (1 << 0)
#define LSMDSL_CTRL1_XL_LPF1_BW_SEL         (1 << 1)
#define LSMDSL_CTRL1_XL_FS_MASK             0x0C
#define LSMDSL_CTRL1_XL_FS_XL0              (1 << 2)
#define LSMDSL_CTRL1_XL_FS_XL1              (1 << 3)
#define LSMDSL_CTRL1_XL_ODR_XL0             (1 << 4)
#define LSMDSL_CTRL1_XL_ODR_XL1             (1 << 5)
#define LSMDSL_CTRL1_XL_ODR_XL2             (1 << 6)
#define LSMDSL_CTRL1_XL_ODR_XL3             (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL2_G register bits definitions
 * @{
 */
#define LSMDSL_CTRL2_G_FS_MASK              0x0E
#define LSMDSL_CTRL2_G_FS_125               (1 << 1)
#define LSMDSL_CTRL2_G_FS_G0                (1 << 2)
#define LSMDSL_CTRL2_G_FS_G1                (1 << 3)
#define LSMDSL_CTRL2_G_ODR_G0               (1 << 4)
#define LSMDSL_CTRL2_G_ODR_G1               (1 << 5)
#define LSMDSL_CTRL2_G_ODR_G2               (1 << 6)
#define LSMDSL_CTRL2_G_ODR_G3               (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL3_C register bits definitions
 * @{
 */
#define LSMDSL_CTRL3_C_SW_RESET             (1 << 0)
#define LSMDSL_CTRL3_C_BLE                  (1 << 1)
#define LSMDSL_CTRL3_C_IF_INC               (1 << 2)
#define LSMDSL_CTRL3_C_SIM                  (1 << 3)
#define LSMDSL_CTRL3_C_PP_OD                (1 << 4)
#define LSMDSL_CTRL3_C_H_LACTIVE            (1 << 5)
#define LSMDSL_CTRL3_C_BDU                  (1 << 6)
#define LSMDSL_CTRL3_C_BOOT                 (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL4_C register bits definitions
 * @{
 */
#define LSMDSL_CTRL4_C_NOT_USED_01          (1 << 0)
#define LSMDSL_CTRL4_C_LPF1_SEL_G           (1 << 1)
#define LSMDSL_CTRL4_C_I2C_DISABLE          (1 << 2)
#define LSMDSL_CTRL4_C_DRDY_MASK            (1 << 3)
#define LSMDSL_CTRL4_C_DEN_DRDY_IN          (1 << 4)
#define LSMDSL_CTRL4_C_INT2_ON_INT          (1 << 5)
#define LSMDSL_CTRL4_C_SLEEP                (1 << 6)
#define LSMDSL_CTRL4_C_DEN_XL_EN            (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL5_C register bits definitions
 * @{
 */
#define LSMDSL_CTRL5_C_ST0_XL               (1 << 0)
#define LSMDSL_CTRL5_C_ST1_XL               (1 << 1)
#define LSMDSL_CTRL5_C_ST0_G                (1 << 2)
#define LSMDSL_CTRL5_C_ST1_G                (1 << 3)
#define LSMDSL_CTRL5_C_DEN_LH               (1 << 4)
#define LSMDSL_CTRL5_C_ROUNDING0            (1 << 5)
#define LSMDSL_CTRL5_C_ROUNDING1            (1 << 6)
#define LSMDSL_CTRL5_C_ROUNDING2            (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL6_C register bits definitions
 * @{
 */
#define LSMDSL_CTRL6_C_FTYPE_0              (1 << 0)
#define LSMDSL_CTRL6_C_FTYPE_1              (1 << 1)
#define LSMDSL_CTRL6_C_USR_OFF_W            (1 << 3)
#define LSMDSL_CTRL6_C_XL_HM_MODE           (1 << 4)
#define LSMDSL_CTRL6_C_LVL2_EN              (1 << 5)
#define LSMDSL_CTRL6_C_LVL_EN               (1 << 6)
#define LSMDSL_CTRL6_C_TRIG_EN              (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL7_G register bits definitions
 * @{
 */
#define LSMDSL_CTRL7_G_ROUNDING_ST          (1 << 2)
#define LSMDSL_CTRL7_G_HPM0_G               (1 << 4)
#define LSMDSL_CTRL7_G_HPM1_G               (1 << 5)
#define LSMDSL_CTRL7_G_HP_EN_G              (1 << 6)
#define LSMDSL_CTRL7_G_G_HM_MODE            (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL8_XL register bits definitions
 * @{
 */
#define LSMDSL_CTRL8_XL_LOW_PASS_ON         (1 << 0)
#define LSMDSL_CTRL8_XL_HP_SLOPE_XL         (1 << 2)
#define LSMDSL_CTRL8_XL_INPUT_COMPO         (1 << 3)
#define LSMDSL_CTRL8_XL_HP_REF_MODE         (1 << 4)
#define LSMDSL_CTRL8_XL_HPCF_XL0            (1 << 5)
#define LSMDSL_CTRL8_XL_HPCF_XL1            (1 << 6)
#define LSMDSL_CTRL8_XL_LPF2_XL_EN          (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL9_XL register bits definitions
 * @{
 */
#define LSMDSL_CTRL9_XL_SOFT_EN             (1 << 2)
#define LSMDSL_CTRL9_XL_DEN_XL_G            (1 << 4)
#define LSMDSL_CTRL9_XL_DEN_Z               (1 << 5)
#define LSMDSL_CTRL9_XL_DEN_Y               (1 << 6)
#define LSMDSL_CTRL9_XL_DEN_X               (1 << 7)
/** @} */

/**
 * @name    LSM6DSV16X_AD_CTRL10_C register bits definitions
 * @{
 */
#define LSMDSL_CTRL10_C_SIGN_MOTION         (1 << 0)
#define LSMDSL_CTRL10_C_PEDO_RST_ST         (1 << 1)
#define LSMDSL_CTRL10_C_FUNC_EN             (1 << 2)
#define LSMDSL_CTRL10_C_TILT_EN             (1 << 3)
#define LSMDSL_CTRL10_C_PEDO_EN             (1 << 4)
#define LSMDSL_CTRL10_C_TIMER_EN            (1 << 5)
#define LSMDSL_CTRL10_C_WRIST_TILT          (1 << 7)
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   LSM6DSV16X SPI interface switch.
 * @details If set to @p TRUE the support for SPI is included.
 * @note    The default is @p FALSE.
 */
#if !defined(LSM6DSV16X_USE_SPI) || defined(__DOXYGEN__)
#define LSM6DSV16X_USE_SPI                     FALSE
#endif

/**
 * @brief   LSM6DSV16X shared SPI switch.
 * @details If set to @p TRUE the device acquires SPI bus ownership
 *          on each transaction.
 * @note    The default is @p FALSE. Requires SPI_USE_MUTUAL_EXCLUSION.
 */
#if !defined(LSM6DSV16X_SHARED_SPI) || defined(__DOXYGEN__)
#define LSM6DSV16X_SHARED_SPI                  FALSE
#endif

/**
 * @brief   LSM6DSV16X I2C interface switch.
 * @details If set to @p TRUE the support for I2C is included.
 * @note    The default is @p TRUE.
 */
#if !defined(LSM6DSV16X_USE_I2C) || defined(__DOXYGEN__)
#define LSM6DSV16X_USE_I2C                     TRUE
#endif

/**
 * @brief   LSM6DSV16X shared I2C switch.
 * @details If set to @p TRUE the device acquires I2C bus ownership
 *          on each transaction.
 * @note    The default is @p FALSE. Requires I2C_USE_MUTUAL_EXCLUSION.
 */
#if !defined(LSM6DSV16X_SHARED_I2C) || defined(__DOXYGEN__)
#define LSM6DSV16X_SHARED_I2C                  FALSE
#endif

/**
 * @brief   LSM6DSV16X advanced configurations switch.
 * @details If set to @p TRUE more configurations are available.
 * @note    The default is @p FALSE.
 */
#if !defined(LSM6DSV16X_USE_ADVANCED) || defined(__DOXYGEN__)
#define LSM6DSV16X_USE_ADVANCED                FALSE
#endif

/**
 * @brief   Number of acquisitions for gyroscope bias removal.
 * @details This is the number of acquisitions performed to compute the
 *          bias. A repetition is required in order to remove noise.
 */
#if !defined(LSM6DSV16X_GYRO_BIAS_ACQ_TIMES) || defined(__DOXYGEN__)
#define LSM6DSV16X_GYRO_BIAS_ACQ_TIMES         50
#endif

/**
 * @brief   Settling time for gyroscope bias removal.
 * @details This is the time between each bias acquisition.
 */
#if !defined(LSM6DSV16X_GYRO_BIAS_SETTLING_US) || defined(__DOXYGEN__)
#define LSM6DSV16X_GYRO_BIAS_SETTLING_US       5000
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !(LSM6DSV16X_USE_SPI ^ LSM6DSV16X_USE_I2C)
#error "LSM6DSV16X_USE_SPI and LSM6DSV16X_USE_I2C cannot be both true or both false"
#endif

#if LSM6DSV16X_USE_SPI && !HAL_USE_SPI
#error "LSM6DSV16X_USE_SPI requires HAL_USE_SPI"
#endif

#if LSM6DSV16X_SHARED_SPI && !SPI_USE_MUTUAL_EXCLUSION
#error "LSM6DSV16X_SHARED_SPI requires SPI_USE_MUTUAL_EXCLUSION"
#endif

#if LSM6DSV16X_USE_I2C && !HAL_USE_I2C
#error "LSM6DSV16X_USE_I2C requires HAL_USE_I2C"
#endif

#if LSM6DSV16X_SHARED_I2C && !I2C_USE_MUTUAL_EXCLUSION
#error "LSM6DSV16X_SHARED_I2C requires I2C_USE_MUTUAL_EXCLUSION"
#endif

/*
 * CHTODO: Add support for LSM6DSV16X over SPI.
 */
#if LSM6DSV16X_USE_SPI
#error "LSM6DSV16X over SPI still not supported"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @name    LSM6DSV16X data structures and types.
 * @{
 */
/**
 * @brief   Structure representing a LSM6DSV16X driver.
 */
typedef struct LSM6DSV16XDriver LSM6DSV16XDriver;

/**
 * @brief  Accelerometer and Gyroscope Slave Address.
 */
typedef enum {
  LSM6DSV16X_SAD_GND = 0x6A,           /**< SAD pin connected to GND.          */
  LSM6DSV16X_SAD_VCC = 0x6B            /**< SAD pin connected to VCC.          */
} lsm6dsv16x_sad_t;

/**
 * @brief   LSM6DSV16X accelerometer subsystem full scale.
 */
typedef enum {
  LSM6DSV16X_2g  = 0x0,
  LSM6DSV16X_4g  = 0x1,
  LSM6DSV16X_8g  = 0x2,
  LSM6DSV16X_16g = 0x3,
} lsm6dsv16x_acc_fs_t;

/**
 * @brief   LSM6DSV16X accelerometer subsystem output data rate.
 */
typedef enum {
  LSM6DSV16X_ACC_ODR_OFF              = 0x0,
  LSM6DSV16X_ACC_ODR_AT_1Hz875        = 0x1,
  LSM6DSV16X_ACC_ODR_AT_7Hz5          = 0x2,
  LSM6DSV16X_ACC_ODR_AT_15Hz          = 0x3,
  LSM6DSV16X_ACC_ODR_AT_30Hz          = 0x4,
  LSM6DSV16X_ACC_ODR_AT_60Hz          = 0x5,
  LSM6DSV16X_ACC_ODR_AT_120Hz         = 0x6,
  LSM6DSV16X_ACC_ODR_AT_240Hz         = 0x7,
  LSM6DSV16X_ACC_ODR_AT_480Hz         = 0x8,
  LSM6DSV16X_ACC_ODR_AT_960Hz         = 0x9,
  LSM6DSV16X_ACC_ODR_AT_1920Hz        = 0xA,
  LSM6DSV16X_ACC_ODR_AT_3840Hz        = 0xB,
  LSM6DSV16X_ACC_ODR_AT_7680Hz        = 0xC,
  LSM6DSV16X_ACC_ODR_HA01_AT_15Hz625  = 0x13,
  LSM6DSV16X_ACC_ODR_HA01_AT_31Hz25   = 0x14,
  LSM6DSV16X_ACC_ODR_HA01_AT_62Hz5    = 0x15,
  LSM6DSV16X_ACC_ODR_HA01_AT_125Hz    = 0x16,
  LSM6DSV16X_ACC_ODR_HA01_AT_250Hz    = 0x17,
  LSM6DSV16X_ACC_ODR_HA01_AT_500Hz    = 0x18,
  LSM6DSV16X_ACC_ODR_HA01_AT_1000Hz   = 0x19,
  LSM6DSV16X_ACC_ODR_HA01_AT_2000Hz   = 0x1A,
  LSM6DSV16X_ACC_ODR_HA01_AT_4000Hz   = 0x1B,
  LSM6DSV16X_ACC_ODR_HA01_AT_8000Hz   = 0x1C,
  LSM6DSV16X_ACC_ODR_HA02_AT_12Hz5    = 0x23,
  LSM6DSV16X_ACC_ODR_HA02_AT_25Hz     = 0x24,
  LSM6DSV16X_ACC_ODR_HA02_AT_50Hz     = 0x25,
  LSM6DSV16X_ACC_ODR_HA02_AT_100Hz    = 0x26,
  LSM6DSV16X_ACC_ODR_HA02_AT_200Hz    = 0x27,
  LSM6DSV16X_ACC_ODR_HA02_AT_400Hz    = 0x28,
  LSM6DSV16X_ACC_ODR_HA02_AT_800Hz    = 0x29,
  LSM6DSV16X_ACC_ODR_HA02_AT_1600Hz   = 0x2A,
  LSM6DSV16X_ACC_ODR_HA02_AT_3200Hz   = 0x2B,
  LSM6DSV16X_ACC_ODR_HA02_AT_6400Hz   = 0x2C,
} lsm6dsv16x_acc_odr_t;

/**
 * @brief   LSM6DSV16X accelerometer subsystem output data rate.
 */
typedef enum {
  LSM6DSV16X_ACC_LP_DISABLED = 0x00,   /**< Low power disabled                 */
  LSM6DSV16X_ACC_LP_ENABLED = 0x10     /**< Low power enabled                  */
} lsm6dsv16x_acc_lp_t;

/**
 * @brief LSM6DSV16X gyroscope subsystem full scale.
 */
typedef enum {
  LSM6DSV16X_125dps  = 0x0,
  LSM6DSV16X_250dps  = 0x1,
  LSM6DSV16X_500dps  = 0x2,
  LSM6DSV16X_1000dps = 0x3,
  LSM6DSV16X_2000dps = 0x4,
  LSM6DSV16X_4000dps = 0xc,
} lsm6dsv16x_gyro_fs_t;

/**
 * @brief   LSM6DSV16X gyroscope subsystem output data rate.
 */
typedef enum {
  LSM6DSV16X_GYRO_ODR_OFF              = 0x0,
  LSM6DSV16X_GYRO_ODR_AT_1Hz875        = 0x1,
  LSM6DSV16X_GYRO_ODR_AT_7Hz5          = 0x2,
  LSM6DSV16X_GYRO_ODR_AT_15Hz          = 0x3,
  LSM6DSV16X_GYRO_ODR_AT_30Hz          = 0x4,
  LSM6DSV16X_GYRO_ODR_AT_60Hz          = 0x5,
  LSM6DSV16X_GYRO_ODR_AT_120Hz         = 0x6,
  LSM6DSV16X_GYRO_ODR_AT_240Hz         = 0x7,
  LSM6DSV16X_GYRO_ODR_AT_480Hz         = 0x8,
  LSM6DSV16X_GYRO_ODR_AT_960Hz         = 0x9,
  LSM6DSV16X_GYRO_ODR_AT_1920Hz        = 0xA,
  LSM6DSV16X_GYRO_ODR_AT_3840Hz        = 0xB,
  LSM6DSV16X_GYRO_ODR_AT_7680Hz        = 0xC,
  LSM6DSV16X_GYRO_ODR_HA01_AT_15Hz625  = 0x13,
  LSM6DSV16X_GYRO_ODR_HA01_AT_31Hz25   = 0x14,
  LSM6DSV16X_GYRO_ODR_HA01_AT_62Hz5    = 0x15,
  LSM6DSV16X_GYRO_ODR_HA01_AT_125Hz    = 0x16,
  LSM6DSV16X_GYRO_ODR_HA01_AT_250Hz    = 0x17,
  LSM6DSV16X_GYRO_ODR_HA01_AT_500Hz    = 0x18,
  LSM6DSV16X_GYRO_ODR_HA01_AT_1000Hz   = 0x19,
  LSM6DSV16X_GYRO_ODR_HA01_AT_2000Hz   = 0x1A,
  LSM6DSV16X_GYRO_ODR_HA01_AT_4000Hz   = 0x1B,
  LSM6DSV16X_GYRO_ODR_HA01_AT_8000Hz   = 0x1C,
  LSM6DSV16X_GYRO_ODR_HA02_AT_12Hz5    = 0x23,
  LSM6DSV16X_GYRO_ODR_HA02_AT_25Hz     = 0x24,
  LSM6DSV16X_GYRO_ODR_HA02_AT_50Hz     = 0x25,
  LSM6DSV16X_GYRO_ODR_HA02_AT_100Hz    = 0x26,
  LSM6DSV16X_GYRO_ODR_HA02_AT_200Hz    = 0x27,
  LSM6DSV16X_GYRO_ODR_HA02_AT_400Hz    = 0x28,
  LSM6DSV16X_GYRO_ODR_HA02_AT_800Hz    = 0x29,
  LSM6DSV16X_GYRO_ODR_HA02_AT_1600Hz   = 0x2A,
  LSM6DSV16X_GYRO_ODR_HA02_AT_3200Hz   = 0x2B,
  LSM6DSV16X_GYRO_ODR_HA02_AT_6400Hz   = 0x2C,
} lsm6dsv16x_gyro_odr_t;

#define LSM6DSV16X_CTRL1                       0x10U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t odr_xl               : 4;
  uint8_t op_mode_xl           : 3;
  uint8_t not_used0            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 1;
  uint8_t op_mode_xl           : 3;
  uint8_t odr_xl               : 4;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl1_t;

#define LSM6DSV16X_CTRL2                       0x11U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t odr_g                : 4;
  uint8_t op_mode_g            : 3;
  uint8_t not_used0            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 1;
  uint8_t op_mode_g            : 3;
  uint8_t odr_g                : 4;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl2_t;

#define LSM6DSV16X_CTRL3                       0x12U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t sw_reset             : 1;
  uint8_t not_used0            : 1;
  uint8_t if_inc               : 1;
  uint8_t not_used1            : 3;
  uint8_t bdu                  : 1;
  uint8_t boot                 : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t boot                 : 1;
  uint8_t bdu                  : 1;
  uint8_t not_used1            : 3;
  uint8_t if_inc               : 1;
  uint8_t not_used0            : 1;
  uint8_t sw_reset             : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl3_t;

#define LSM6DSV16X_CTRL4                       0x13U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t int2_in_lh           : 1;
  uint8_t drdy_pulsed          : 1;
  uint8_t int2_drdy_temp       : 1;
  uint8_t drdy_mask            : 1;
  uint8_t int2_on_int1         : 1;
  uint8_t not_used0            : 3;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 3;
  uint8_t int2_on_int1         : 1;
  uint8_t drdy_mask            : 1;
  uint8_t int2_drdy_temp       : 1;
  uint8_t drdy_pulsed          : 1;
  uint8_t int2_in_lh           : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl4_t;

#define LSM6DSV16X_CTRL5                       0x14U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t int_en_i3c           : 1;
  uint8_t bus_act_sel          : 2;
  uint8_t not_used0            : 5;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 5;
  uint8_t bus_act_sel          : 2;
  uint8_t int_en_i3c           : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl5_t;

#define LSM6DSV16X_CTRL6                       0x15U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t fs_g                 : 4;
  uint8_t lpf1_g_bw            : 3;
  uint8_t not_used0            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 1;
  uint8_t lpf1_g_bw            : 3;
  uint8_t fs_g                 : 4;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl6_t;

#define LSM6DSV16X_CTRL7                       0x16U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t lpf1_g_en            : 1;
  uint8_t not_used0            : 3;
  uint8_t ah_qvar_c_zin        : 2;
  uint8_t int2_drdy_ah_qvar    : 1;
  uint8_t ah_qvar_en           : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t ah_qvar_en           : 1;
  uint8_t int2_drdy_ah_qvar    : 1;
  uint8_t ah_qvar_c_zin        : 2;
  uint8_t not_used0            : 3;
  uint8_t lpf1_g_en            : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl7_t;

#define LSM6DSV16X_CTRL8                       0x17U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t fs_xl                : 2;
  uint8_t not_used0            : 1;
  uint8_t xl_dualc_en          : 1;
  uint8_t not_used1            : 1;
  uint8_t hp_lpf2_xl_bw        : 3;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t hp_lpf2_xl_bw        : 3;
  uint8_t not_used1            : 1;
  uint8_t xl_dualc_en          : 1;
  uint8_t not_used0            : 1;
  uint8_t fs_xl                : 2;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl8_t;

#define LSM6DSV16X_CTRL9                       0x18U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t usr_off_on_out       : 1;
  uint8_t usr_off_w            : 1;
  uint8_t not_used0            : 1;
  uint8_t lpf2_xl_en           : 1;
  uint8_t hp_slope_xl_en       : 1;
  uint8_t xl_fastsettl_mode    : 1;
  uint8_t hp_ref_mode_xl       : 1;
  uint8_t not_used1            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used1            : 1;
  uint8_t hp_ref_mode_xl       : 1;
  uint8_t xl_fastsettl_mode    : 1;
  uint8_t hp_slope_xl_en       : 1;
  uint8_t lpf2_xl_en           : 1;
  uint8_t not_used0            : 1;
  uint8_t usr_off_w            : 1;
  uint8_t usr_off_on_out       : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl9_t;

#define LSM6DSV16X_CTRL10                      0x19U
typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t st_xl                : 2;
  uint8_t st_g                 : 2;
  uint8_t not_used0            : 2;
  uint8_t emb_func_debug       : 1;
  uint8_t not_used1            : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used1            : 1;
  uint8_t emb_func_debug       : 1;
  uint8_t not_used0            : 2;
  uint8_t st_g                 : 2;
  uint8_t st_xl                : 2;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_ctrl10_t;

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t fifo_mode            : 3;
  uint8_t g_eis_fifo_en        : 1;
  uint8_t odr_t_batch          : 2;
  uint8_t dec_ts_batch         : 2;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t dec_ts_batch         : 2;
  uint8_t odr_t_batch          : 2;
  uint8_t g_eis_fifo_en        : 1;
  uint8_t fifo_mode            : 3;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_fifo_ctrl4_t;

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t haodr_sel            : 2;
  uint8_t not_used0            : 6;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t not_used0            : 6;
  uint8_t haodr_sel            : 2;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_haodr_cfg_t;

typedef struct
{
#if DRV_BYTE_ORDER == DRV_LITTLE_ENDIAN
  uint8_t ois_ctrl_from_ui     : 1;
  uint8_t spi2_reset           : 1;
  uint8_t sw_por               : 1;
  uint8_t fsm_wr_ctrl_en       : 1;
  uint8_t not_used0            : 2;
  uint8_t shub_reg_access      : 1;
  uint8_t emb_func_reg_access  : 1;
#elif DRV_BYTE_ORDER == DRV_BIG_ENDIAN
  uint8_t emb_func_reg_access  : 1;
  uint8_t shub_reg_access      : 1;
  uint8_t not_used0            : 2;
  uint8_t fsm_wr_ctrl_en       : 1;
  uint8_t sw_por               : 1;
  uint8_t spi2_reset           : 1;
  uint8_t ois_ctrl_from_ui     : 1;
#endif /* DRV_BYTE_ORDER */
} lsm6dsv16x_func_cfg_access_t;

typedef enum
{
  LSM6DSV16X_RESET_READY             = 0x0,
  LSM6DSV16X_RESET_GLOBAL_RST        = 0x1,
  LSM6DSV16X_RESET_RESTORE_CAL_PARAM = 0x2,
  LSM6DSV16X_RESET_RESTORE_CTRL_REGS = 0x4,
} lsm6dsv16x_reset_t;

/**
 * @brief LSM6DSV16X gyroscope subsystem low mode configuration.
 */
typedef enum {
  LSM6DSV16X_GYRO_LP_DISABLED = 0x00,  /**< Low power mode disabled.           */
  LSM6DSV16X_GYRO_LP_ENABLED = 0x80    /**< Low power mode enabled.            */
} lsm6dsv16x_gyro_lp_t;

/**
 * @brief  LSM6DSV16X gyroscope subsystem output selection.
 */
typedef enum {
  LSM6DSV16X_GYRO_LPF_DISABLED = -1,   /**< Low pass filter disabled.          */
  LSM6DSV16X_GYRO_LPF_FTYPE0 = 0x00,   /**< Refer to table 68 of Datasheet.    */
  LSM6DSV16X_GYRO_LPF_FTYPE1 = 0x01,   /**< Refer to table 68 of Datasheet.    */
  LSM6DSV16X_GYRO_LPF_FTYPE2 = 0x10,   /**< Refer to table 68 of Datasheet.    */
  LSM6DSV16X_GYRO_LPF_FTYPE3 = 0x11    /**< Refer to table 68 of Datasheet.    */
} lsm6dsv16x_gyro_lpf_t;

/**
 * @brief LSM6DSV16X block data update.
 */
typedef enum {
  LSM6DSV16X_BDU_CONTINUOUS = 0x00,    /**< Block data continuously updated.   */
  LSM6DSV16X_BDU_BLOCKED = 0x40        /**< Block data updated after reading.  */
} lsm6dsv16x_bdu_t;

/**
 * @brief LSM6DSV16X endianness.
 */
typedef enum {
  LSM6DSV16X_END_LITTLE = 0x00,        /**< Little endian.                     */
  LSM6DSV16X_END_BIG = 0x20            /**< Big endian.                        */
} lsm6dsv16x_end_t;

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  LSM6DSV16X_UNINIT = 0,               /**< Not initialized.                   */
  LSM6DSV16X_STOP = 1,                 /**< Stopped.                           */
  LSM6DSV16X_READY = 2,                /**< Ready.                             */
} lsm6dsv16x_state_t;

/**
 * @brief LSM6DSV16X configuration structure.
 */
typedef struct {
#if (LSM6DSV16X_USE_SPI) || defined(__DOXYGEN__)
  /**
   * @brief SPI driver associated to this LSM6DSV16X.
   */
  SPIDriver                 *spip;
  /**
   * @brief SPI configuration associated to this LSM6DSV16X accelerometer
   *        subsystem.
   */
  const SPIConfig           *accspicfg;
#endif /* LSM6DSV16X_USE_SPI */
#if (LSM6DSV16X_USE_I2C) || defined(__DOXYGEN__)
  /**
   * @brief I2C driver associated to this LSM6DSV16X.
   */
  I2CDriver                 *i2cp;
  /**
   * @brief I2C configuration associated to this LSM6DSV16X accelerometer
   *        subsystem.
   */
  const I2CConfig           *i2ccfg;
  /**
   * @brief LSM6DSV16X Slave Address
   */
  lsm6dsv16x_sad_t             slaveaddress;
#endif /* LSM6DSV16X_USE_I2C */
  /**
   * @brief LSM6DSV16X accelerometer subsystem initial sensitivity.
   */
  float                     *accsensitivity;
  /**
   * @brief LSM6DSV16X accelerometer subsystem initial bias.
   */
  float                     *accbias;
  /**
   * @brief LSM6DSV16X accelerometer subsystem full scale.
   */
  lsm6dsv16x_acc_fs_t          accfullscale;
  /**
   * @brief LSM6DSV16X accelerometer subsystem output data rate.
   */
  lsm6dsv16x_acc_odr_t         accoutdatarate;
#if LSM6DSV16X_USE_ADVANCED || defined(__DOXYGEN__)
  /**
   * @brief LSM6DSV16X accelerometer subsystem low power mode.
   */
  lsm6dsv16x_acc_lp_t          acclpmode;
#endif /* LSM6DSV16X_USE_ADVANCED */
  /**
   * @brief LSM6DSV16X gyroscope subsystem initial sensitivity.
   */
  float                     *gyrosensitivity;
  /**
   * @brief LSM6DSV16X gyroscope subsystem initial bias.
   */
  float                     *gyrobias;
  /**
   * @brief LSM6DSV16X gyroscope subsystem full scale.
   */
  lsm6dsv16x_gyro_fs_t         gyrofullscale;
  /**
   * @brief LSM6DSV16X gyroscope subsystem output data rate.
   */
  lsm6dsv16x_gyro_odr_t        gyrooutdatarate;
#if LSM6DSV16X_USE_ADVANCED || defined(__DOXYGEN__)
  /**
   * @brief LSM6DSV16X gyroscope subsystem low mode configuration.
   */
  lsm6dsv16x_gyro_lp_t         gyrolpmode;
  /**
   * @brief LSM6DSV16X gyroscope subsystem low pass filter configuration.
   */
  lsm6dsv16x_gyro_lpf_t        gyrolowpassfilter;
  /**
   * @brief LSM6DSV16X block data update
   */
  lsm6dsv16x_bdu_t             blockdataupdate;
  /**
   * @brief LSM6DSV16X  endianness
   */
  lsm6dsv16x_end_t             endianness;
#endif /* LSM6DSV16X_USE_ADVANCED */
} LSM6DSV16XConfig;

/**
 * @brief   @p LSM6DSV16X specific methods.
 */
#define _lsm6dsv16x_methods_alone                                              \
  /* Change full scale value of LSM6DSV16X accelerometer subsystem .*/         \
  msg_t (*acc_set_full_scale)(LSM6DSV16XDriver *devp, lsm6dsv16x_acc_fs_t fs);    \
  /* Change full scale value of LSM6DSV16X gyroscope subsystem .*/             \
  msg_t (*gyro_set_full_scale)(LSM6DSV16XDriver *devp, lsm6dsv16x_gyro_fs_t fs);

/**
 * @brief   @p LSM6DSV16X specific methods with inherited ones.
 */
#define _lsm6dsv16x_methods                                                    \
  _base_object_methods                                                      \
  _lsm6dsv16x_methods_alone

/**
 * @extends BaseObjectVMT
 *
 * @brief @p LSM6DSV16X virtual methods table.
 */
struct LSM6DSV16XVMT {
  _lsm6dsv16x_methods
};

/**
 * @brief   @p LSM6DSV16XDriver specific data.
 */
#define _lsm6dsv16x_data                                                       \
  _base_sensor_data                                                         \
  /* Driver state.*/                                                        \
  lsm6dsv16x_state_t           state;                                          \
  /* Current configuration data.*/                                          \
  const LSM6DSV16XConfig       *config;                                        \
  /* Accelerometer subsystem axes number.*/                                 \
  size_t                    accaxes;                                        \
  /* Accelerometer subsystem current sensitivity.*/                         \
  float                     accsensitivity[LSM6DSV16X_ACC_NUMBER_OF_AXES];     \
  /* Accelerometer subsystem current bias .*/                               \
  float                     accbias[LSM6DSV16X_ACC_NUMBER_OF_AXES];            \
  /* Accelerometer subsystem current full scale value.*/                    \
  float                     accfullscale;                                   \
  /* Gyroscope subsystem axes number.*/                                     \
  size_t                    gyroaxes;                                       \
  /* Gyroscope subsystem current sensitivity.*/                             \
  float                     gyrosensitivity[LSM6DSV16X_GYRO_NUMBER_OF_AXES];   \
  /* Gyroscope subsystem current Bias.*/                                    \
  float                     gyrobias[LSM6DSV16X_GYRO_NUMBER_OF_AXES];          \
  /* Gyroscope subsystem current full scale value.*/                        \
  float                     gyrofullscale;

/**
 * @brief LSM6DSV16X 6-axis accelerometer/gyroscope class.
 */
struct LSM6DSV16XDriver {
  /** @brief Virtual Methods Table.*/
  const struct LSM6DSV16XVMT     *vmt;
  /** @brief Base accelerometer interface.*/
  BaseAccelerometer           acc_if;
  /** @brief Base gyroscope interface.*/
  BaseGyroscope               gyro_if;
  _lsm6dsv16x_data
};
/** @} */

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Return the number of axes of the BaseAccelerometer.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              the number of axes.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerGetAxesNumber(devp)                             \
        accelerometerGetAxesNumber(&((devp)->acc_if))

/**
 * @brief   Retrieves raw data from the BaseAccelerometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerReadRaw(devp, axes)                             \
        accelerometerReadRaw(&((devp)->acc_if), axes)

/**
 * @brief   Retrieves cooked data from the BaseAccelerometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as milli-G.
 * @note    The axes array must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[out] axes     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerReadCooked(devp, axes)                          \
        accelerometerReadCooked(&((devp)->acc_if), axes)

/**
 * @brief   Set bias values for the BaseAccelerometer.
 * @note    Bias must be expressed as milli-G.
 * @note    The bias buffer must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerSetBias(devp, bp)                               \
        accelerometerSetBias(&((devp)->acc_if), bp)

/**
 * @brief   Reset bias values for the BaseAccelerometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerResetBias(devp)                                 \
        accelerometerResetBias(&((devp)->acc_if))

/**
 * @brief   Set sensitivity values for the BaseAccelerometer.
 * @note    Sensitivity must be expressed as milli-G/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseAccelerometer axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerSetSensitivity(devp, sp)                        \
        accelerometerSetSensitivity(&((devp)->acc_if), sp)

/**
 * @brief   Reset sensitivity values for the BaseAccelerometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerResetSensitivity(devp)                          \
        accelerometerResetSensitivity(&((devp)->acc_if))

/**
 * @brief   Changes the LSM6DSV16XDriver accelerometer fullscale value.
 * @note    This function also rescale sensitivities and biases based on
 *          previous and next fullscale value.
 * @note    A recalibration is highly suggested after calling this function.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] fs        new fullscale value.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 *
 * @api
 */
#define lsm6dsv16xAccelerometerSetFullScale(devp, fs)                          \
        (devp)->vmt->acc_set_full_scale(devp, fs)

/**
 * @brief   Return the number of axes of the BaseGyroscope.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              the number of axes.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeGetAxesNumber(devp)                                 \
        gyroscopeGetAxesNumber(&((devp)->gyro_if))

/**
 * @brief   Retrieves raw data from the BaseGyroscope.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[out] axes     a buffer which would be filled with raw data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeReadRaw(devp, axes)                                 \
        gyroscopeReadRaw(&((devp)->gyro_if), axes)

/**
 * @brief   Retrieves cooked data from the BaseGyroscope.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as DPS.
 * @note    The axes array must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[out] axes     a buffer which would be filled with cooked data.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeReadCooked(devp, axes)                              \
        gyroscopeReadCooked(&((devp)->gyro_if), axes)

/**
 * @brief   Samples bias values for the BaseGyroscope.
 * @note    The LSM6DSV16X shall not be moved during the whole procedure.
 * @note    After this function internal bias is automatically updated.
 * @note    The behavior of this function depends on @p LSM6DSV16X_BIAS_ACQ_TIMES
 *          and @p LSM6DSV16X_BIAS_SETTLING_US.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    if one or more I2C errors occurred, the errors can
 *                      be retrieved using @p i2cGetErrors().
 * @retval MSG_TIMEOUT  if a timeout occurred before operation end.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeSampleBias(devp)                                    \
        gyroscopeSampleBias(&((devp)->gyro_if))

/**
 * @brief   Set bias values for the BaseGyroscope.
 * @note    Bias must be expressed as DPS.
 * @note    The bias buffer must be at least the same size of the BaseGyroscope
 *          axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeSetBias(devp, bp)                                   \
        gyroscopeSetBias(&((devp)->gyro_if), bp)

/**
 * @brief   Reset bias values for the BaseGyroscope.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeResetBias(devp)                                     \
        gyroscopeResetBias(&((devp)->gyro_if))

/**
 * @brief   Set sensitivity values for the BaseGyroscope.
 * @note    Sensitivity must be expressed as DPS/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseGyroscope axes number.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeSetSensitivity(devp, sp)                            \
        gyroscopeSetSensitivity(&((devp)->gyro_if), sp)

/**
 * @brief   Reset sensitivity values for the BaseGyroscope.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeResetSensitivity(devp)                              \
        gyroscopeResetSensitivity(&((devp)->gyro_if))

/**
 * @brief   Changes the LSM6DSV16XDriver gyroscope fullscale value.
 * @note    This function also rescale sensitivities and biases based on
 *          previous and next fullscale value.
 * @note    A recalibration is highly suggested after calling this function.
 *
 * @param[in] devp      pointer to @p LSM6DSV16XDriver.
 * @param[in] fs        new fullscale value.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 * @retval MSG_RESET    otherwise.
 *
 * @api
 */
#define lsm6dsv16xGyroscopeSetFullScale(devp, fs)                              \
        (devp)->vmt->acc_set_full_scale(devp, fs)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void lsm6dsv16xObjectInit(LSM6DSV16XDriver *devp);
  void lsm6dsv16xStart(LSM6DSV16XDriver *devp, const LSM6DSV16XConfig *config);
  void lsm6dsv16xStop(LSM6DSV16XDriver *devp);
#ifdef __cplusplus
}
#endif

#endif /* _LSM6DSV16X_H_ */

/** @} */
