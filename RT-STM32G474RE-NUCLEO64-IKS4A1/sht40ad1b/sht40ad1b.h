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
 * @file    sht40ad1b.h
 * @brief   SHT40AD1B MEMS interface module header.
 *
 *
 * @addtogroup SHT40AD1B
 * @ingroup EX_ST
 * @{
 */
#ifndef _SHT40AD1B_H_
#define _SHT40AD1B_H_

#include "ex_hygrometer.h"
#include "ex_thermometer.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    Version identification
 * @{
 */
/**
 * @brief   SHT40AD1B driver version string.
 */
#define EX_SHT40AD1B_VERSION                   "1.0.1"

/**
 * @brief   SHT40AD1B driver version major number.
 */
#define EX_SHT40AD1B_MAJOR                     1

/**
 * @brief   SHT40AD1B driver version minor number.
 */
#define EX_SHT40AD1B_MINOR                     1

/**
 * @brief   SHT40AD1B driver version patch number.
 */
#define EX_SHT40AD1B_PATCH                     1
/** @} */

/**
 * @brief   SHT40AD1B hygrometer subsystem characteristics.
 * @note    Sensitivity is expressed as %rH/LSB whereas %rH stand for percentage
 *          of relative humidity.
 * @note    Bias is expressed as %rH.
 * @{
 */
#define SHT40AD1B_HYGRO_NUMBER_OF_AXES         1U

#define SHT40AD1B_HYGRO_SENS                   125.0f / (float)0xFFFF
#define SHT40AD1B_HYGRO_BIAS                   6.0f
/** @} */

/**
 * @brief   SHT40AD1B thermometer subsystem characteristics.
 * @note    Sensitivity is expressed as �C/LSB.
 * @note    Bias is expressed as �C.
 *
 * @{
 */
#define SHT40AD1B_THERMO_NUMBER_OF_AXES        1U

#define SHT40AD1B_THERMO_SENS                  175.0f / (float)0xFFFF
#define SHT40AD1B_THERMO_BIAS                  45.0f
/** @} */

/**
 * @name    SHT40AD1B communication interfaces related bit masks
 * @{
 */
#define SHT40AD1B_DI_MASK                      0xFF
#define SHT40AD1B_DI(n)                        (1 << n)
#define SHT40AD1B_AD_MASK                      0x3F
#define SHT40AD1B_AD(n)                        (1 << n)
#define SHT40AD1B_MS                           (1 << 6)
#define SHT40AD1B_RW                           (1 << 7)

#define SHT40AD1B_SUB_MS                       (1 << 7)

#define SHT40AD1B_SAD                          0x44
/** @} */

/**
 * @name    SHT40AD1B register addresses
 * @{
 */
#define SHT40AD1B_AD_WHO_AM_I                  0x89
#define SHT40AD1B_AD_OUT_HIGH_PRECISION        0xFD
#define SHT40AD1B_AD_OUT_MEDIUM_PRECISION      0xF6
#define SHT40AD1B_AD_OUT_LOW_PRECISION         0xE0
#define SHT40AD1B_AD_AV_SOFT_RESET             0x94
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */
/**
 * @brief   SHT40AD1B SPI interface switch.
 * @details If set to @p TRUE the support for SPI is included.
 * @note    The default is @p FALSE.
 */
#if !defined(SHT40AD1B_USE_SPI) || defined(__DOXYGEN__)
#define SHT40AD1B_USE_SPI                      FALSE
#endif

/**
 * @brief   SHT40AD1B shared SPI switch.
 * @details If set to @p TRUE the device acquires SPI bus ownership
 *          on each transaction.
 * @note    The default is @p FALSE. Requires SPI_USE_MUTUAL_EXCLUSION
 */
#if !defined(SHT40AD1B_SHARED_SPI) || defined(__DOXYGEN__)
#define SHT40AD1B_SHARED_SPI                   FALSE
#endif

/**
 * @brief   SHT40AD1B I2C interface switch.
 * @details If set to @p TRUE the support for I2C is included.
 * @note    The default is @p TRUE.
 */
#if !defined(SHT40AD1B_USE_I2C) || defined(__DOXYGEN__)
#define SHT40AD1B_USE_I2C                      TRUE
#endif

/**
 * @brief   SHT40AD1B shared I2C switch.
 * @details If set to @p TRUE the device acquires I2C bus ownership
 *          on each transaction.
 * @note    The default is @p FALSE. Requires I2C_USE_MUTUAL_EXCLUSION
 */
#if !defined(SHT40AD1B_SHARED_I2C) || defined(__DOXYGEN__)
#define SHT40AD1B_SHARED_I2C                   FALSE
#endif

/**
 * @brief   SHT40AD1B advanced configurations switch.
 * @details If set to @p TRUE more configurations are available.
 * @note    The default is @p FALSE.
 */
#if !defined(SHT40AD1B_USE_ADVANCED) || defined(__DOXYGEN__)
#define SHT40AD1B_USE_ADVANCED                 FALSE
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if !(SHT40AD1B_USE_SPI ^ SHT40AD1B_USE_I2C)
#error "SHT40AD1B_USE_SPI and SHT40AD1B_USE_I2C cannot be both true or both false"
#endif

#if SHT40AD1B_USE_SPI && !HAL_USE_SPI
#error "SHT40AD1B_USE_SPI requires HAL_USE_SPI"
#endif

#if SHT40AD1B_SHARED_SPI && !SPI_USE_MUTUAL_EXCLUSION
#error "SHT40AD1B_SHARED_SPI requires SPI_USE_MUTUAL_EXCLUSION"
#endif

#if SHT40AD1B_USE_I2C && !HAL_USE_I2C
#error "SHT40AD1B_USE_I2C requires HAL_USE_I2C"
#endif

#if SHT40AD1B_SHARED_I2C && !I2C_USE_MUTUAL_EXCLUSION
#error "SHT40AD1B_SHARED_I2C requires I2C_USE_MUTUAL_EXCLUSION"
#endif

/*
 * CHTODO: Add support for SHT40AD1B over SPI.
 */
#if SHT40AD1B_USE_SPI
#error "SHT40AD1B over SPI still not supported."
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @name    SHT40AD1B data structures and types.
 * @{
 */
/**
 * @brief   Structure representing a SHT40AD1B driver.
 */
typedef struct SHT40AD1BDriver SHT40AD1BDriver;

/**
 * @brief   SHT40AD1B output data rate and bandwidth.
 */
typedef enum {
  SHT40AD1B_ODR_ONE_SHOT = 0x00,       /**< One shot.                          */
  SHT40AD1B_ODR_1HZ = 0x01,            /**< Output data rate 1 Hz.             */
  SHT40AD1B_ODR_7HZ = 0x02,            /**< Output data rate 7 Hz.             */
  SHT40AD1B_ODR_12P5HZ = 0x03,         /**< Output data rate 12.5 Hz.          */
}sht40ad1b_odr_t;

/**
 * @brief   SHT40AD1B humidity resolution.
 */
typedef enum {
  SHT40AD1B_AVGH_4 = 0x00,             /**< Number of internal average is 4.   */
  SHT40AD1B_AVGH_8 = 0x01,             /**< Number of internal average is 8.   */
  SHT40AD1B_AVGH_16 = 0x02,            /**< Number of internal average is 16.  */
  SHT40AD1B_AVGH_32 = 0x03,            /**< Number of internal average is 32.  */
  SHT40AD1B_AVGH_64 = 0x04,            /**< Number of internal average is 64.  */
  SHT40AD1B_AVGH_128 = 0x05,           /**< Number of internal average is 128. */
  SHT40AD1B_AVGH_256 = 0x06,           /**< Number of internal average is 256. */
  SHT40AD1B_AVGH_512 = 0x07            /**< Number of internal average is 512. */
}sht40ad1b_avgh_t;

/**
 * @brief   SHT40AD1B temperature resolution.
 */
typedef enum {
  SHT40AD1B_AVGT_2 = 0x00,             /**< Number of internal average is 2.   */
  SHT40AD1B_AVGT_4 = 0x08,             /**< Number of internal average is 4.   */
  SHT40AD1B_AVGT_8 = 0x10,             /**< Number of internal average is 8.   */
  SHT40AD1B_AVGT_16 = 0x18,            /**< Number of internal average is 16.  */
  SHT40AD1B_AVGT_32 = 0x20,            /**< Number of internal average is 32.  */
  SHT40AD1B_AVGT_64 = 0x28,            /**< Number of internal average is 64.  */
  SHT40AD1B_AVGT_128 = 0x30,           /**< Number of internal average is 128. */
  SHT40AD1B_AVGT_256 = 0x38,           /**< Number of internal average is 256. */
}sht40ad1b_avgt_t;

/**
 * @brief   SHT40AD1B block data update.
 */
typedef enum {
  SHT40AD1B_BDU_CONTINUOUS = 0x00,     /**< Block data continuously updated.   */
  SHT40AD1B_BDU_BLOCKED = 0x40         /**< Block data updated after reading.  */
}sht40ad1b_bdu_t;

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  SHT40AD1B_UNINIT = 0,                /**< Not initialized.                   */
  SHT40AD1B_STOP = 1,                  /**< Stopped.                           */
  SHT40AD1B_READY = 2,                 /**< Ready.                             */
} sht40ad1b_state_t;

/**
 * @brief   SHT40AD1B configuration structure.
 */
typedef struct {

#if SHT40AD1B_USE_SPI || defined(__DOXYGEN__)
  /**
   * @brief SPI driver associated to this SHT40AD1B.
   */
  SPIDriver                 *spip;
  /**
   * @brief SPI configuration associated to this SHT40AD1B.
   */
  const SPIConfig           *spicfg;
#endif /* SHT40AD1B_USE_SPI */
#if SHT40AD1B_USE_I2C || defined(__DOXYGEN__)
  /**
   * @brief I2C driver associated to this SHT40AD1B.
   */
  I2CDriver                 *i2cp;
  /**
   * @brief I2C configuration associated to this SHT40AD1B.
   */
  const I2CConfig           *i2ccfg;
#endif /* SHT40AD1B_USE_I2C */
  /**
   * @brief SHT40AD1B hygrometer subsystem initial sensitivity.
   */
  float                     *hygrosensitivity;
  /**
   * @brief SHT40AD1B hygrometer subsystem initial bias.
   */
  float                     *hygrobias;
  /**
   * @brief SHT40AD1B thermometer subsystem initial sensitivity.
   */
  float                     *thermosensitivity;
  /**
   * @brief SHT40AD1B thermometer subsystem initial bias.
   */
  float                     *thermobias;
  /**
   * @brief SHT40AD1B output data rate selection.
   */
  sht40ad1b_odr_t              outputdatarate;
#if SHT40AD1B_USE_ADVANCED || defined(__DOXYGEN__)
  /**
   * @brief SHT40AD1B block data update.
   */
  sht40ad1b_bdu_t              blockdataupdate;
  /**
   * @brief SHT40AD1B hygrometer subsystem resolution.
   */
  sht40ad1b_avgh_t             hygroresolution;
  /**
   * @brief SHT40AD1B thermometer subsystem resolution.
   */
  sht40ad1b_avgt_t             thermoresolution;
#endif
} SHT40AD1BConfig;

/**
 * @brief   @p SHT40AD1B specific methods.
 * @note    No methods so far, just a common ancestor interface.
 */
#define _sht40ad1b_methods_alone

/**
 * @brief   @p SHT40AD1B specific methods with inherited ones.
 */
#define _sht40ad1b_methods                                                     \
  _base_object_methods                                                      \
  _sht40ad1b_methods_alone

/**
 * @extends BaseObjectVMT
 *
 * @brief @p SHT40AD1B virtual methods table.
 */
struct SHT40AD1BVMT {
  _sht40ad1b_methods
};

/**
 * @brief   @p SHT40AD1BDriver specific data.
 */
#define _sht40ad1b_data                                                        \
  /* Driver state.*/                                                        \
  sht40ad1b_state_t            state;                                          \
  /* Current configuration data.*/                                          \
  const SHT40AD1BConfig        *config;                                        \
  /* Hygrometer subsystem axes number.*/                                    \
  size_t                    hygroaxes;                                      \
  /* Hygrometer subsystem current sensitivity.*/                            \
  float                     hygrosensitivity;                               \
  /* Hygrometer subsystem current bias .*/                                  \
  float                     hygrobias;                                      \
  /* Hygrometer subsystem factory sensitivity.*/                            \
  float                     hygrofactorysensitivity;                        \
  /* Hygrometer subsystem factory bias .*/                                  \
  float                     hygrofactorybias;                               \
  /* Thermometer subsystem axes number.*/                                   \
  size_t                    thermoaxes;                                     \
  /* Thermometer subsystem current sensitivity.*/                           \
  float                     thermosensitivity;                              \
  /* Thermometer subsystem current bias.*/                                  \
  float                     thermobias;                                     \
  /* Thermometer subsystem factory sensitivity.*/                           \
  float                     thermofactorysensitivity;                       \
  /* Thermometer subsystem factory bias.*/                                  \
  float                     thermofactorybias;

/**
 * @brief   SHT40AD1B 2-axis hygrometer/thermometer class.
 */
struct SHT40AD1BDriver {
  /** @brief Virtual Methods Table.*/
  const struct SHT40AD1BVMT    *vmt;
  /** @brief Base hygrometer interface.*/
  BaseHygrometer            hygro_if;
  /** @brief Base thermometer interface.*/
  BaseThermometer           thermo_if;
  _sht40ad1b_data
};
/** @} */

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/**
 * @brief   Return the number of axes of the BaseHygrometer.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              the number of axes.
 *
 * @api
 */
#define sht40ad1bHygrometerGetAxesNumber(devp)                                 \
        hygrometerGetAxesNumber(&((devp)->hygro_if))

/**
 * @brief   Retrieves raw data from the BaseHygrometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
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
#define sht40ad1bHygrometerReadRaw(devp, axes)                                 \
        hygrometerReadRaw(&((devp)->hygro_if), axes)

/**
 * @brief   Retrieves cooked data from the BaseHygrometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as %rH.
 * @note    The axes array must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
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
#define sht40ad1bHygrometerReadCooked(devp, axes)                              \
        hygrometerReadCooked(&((devp)->hygro_if), axes)

/**
 * @brief   Set bias values for the BaseHygrometer.
 * @note    Bias must be expressed as %rH.
 * @note    The bias buffer must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bHygrometerSetBias(devp, bp)                                   \
        hygrometerSetBias(&((devp)->hygro_if), bp)

/**
 * @brief   Reset bias values for the BaseHygrometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bHygrometerResetBias(devp)                                     \
        hygrometerResetBias(&((devp)->hygro_if))

/**
 * @brief   Set sensitivity values for the BaseHygrometer.
 * @note    Sensitivity must be expressed as %rH/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseHygrometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bHygrometerSetSensitivity(devp, sp)                            \
        hygrometerSetSensitivity(&((devp)->hygro_if), sp)

/**
 * @brief   Reset sensitivity values for the BaseHygrometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bHygrometerResetSensitivity(devp)                              \
        hygrometerResetSensitivity(&((devp)->hygro_if))

/**
 * @brief   Return the number of axes of the BaseThermometer.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              the number of axes.
 *
 * @api
 */
#define sht40ad1bThermometerGetAxesNumber(devp)                                \
        thermometerGetAxesNumber(&((devp)->thermo_if))

/**
 * @brief   Retrieves raw data from the BaseThermometer.
 * @note    This data is retrieved from MEMS register without any algebraical
 *          manipulation.
 * @note    The axes array must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
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
#define sht40ad1bThermometerReadRaw(devp, axes)                                \
        thermometerReadRaw(&((devp)->thermo_if), axes)

/**
 * @brief   Retrieves cooked data from the BaseThermometer.
 * @note    This data is manipulated according to the formula
 *          cooked = (raw * sensitivity) - bias.
 * @note    Final data is expressed as �C.
 * @note    The axes array must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
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
#define sht40ad1bThermometerReadCooked(devp, axes)                             \
        thermometerReadCooked(&((devp)->thermo_if), axes)

/**
 * @brief   Set bias values for the BaseThermometer.
 * @note    Bias must be expressed as �C.
 * @note    The bias buffer must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 * @param[in] bp        a buffer which contains biases.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bThermometerSetBias(devp, bp)                                  \
        thermometerSetBias(&((devp)->thermo_if), bp)

/**
 * @brief   Reset bias values for the BaseThermometer.
 * @note    Default biases value are obtained from device datasheet when
 *          available otherwise they are considered zero.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bThermometerResetBias(devp)                                    \
        thermometerResetBias(&((devp)->thermo_if))

/**
 * @brief   Set sensitivity values for the BaseThermometer.
 * @note    Sensitivity must be expressed as �C/LSB.
 * @note    The sensitivity buffer must be at least the same size of the
 *          BaseThermometer axes number.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 * @param[in] sp        a buffer which contains sensitivities.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bThermometerSetSensitivity(devp, sp)                           \
        thermometerSetSensitivity(&((devp)->thermo_if), sp)

/**
 * @brief   Reset sensitivity values for the BaseThermometer.
 * @note    Default sensitivities value are obtained from device datasheet.
 *
 * @param[in] devp      pointer to @p SHT40AD1BDriver.
 *
 * @return              The operation status.
 * @retval MSG_OK       if the function succeeded.
 *
 * @api
 */
#define sht40ad1bThermometerResetSensitivity(devp)                             \
        thermometerResetSensitivity(&((devp)->thermo_if))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void sht40ad1bObjectInit(SHT40AD1BDriver *devp);
  void sht40ad1bStart(SHT40AD1BDriver *devp, const SHT40AD1BConfig *config);
  void sht40ad1bStop(SHT40AD1BDriver *devp);
#ifdef __cplusplus
}
#endif

#endif /* _SHT40AD1B_H_ */

/** @} */
