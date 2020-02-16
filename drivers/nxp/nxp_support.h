/*
 ******************************************************************************
 * @file    lsm9ds1_support.h
 * @author  Isaac Grossberg
 * @brief   This file declares variables and functions used in the
 *          lsm9ds1_support.c file
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPiI2C.h>
#include "../../headers/HTSensors.h"

/* Register Address definitions */

#define FXOS8700_STATUS         0x00
#define FXOS8700_WHOAMI         0x0D
#define FXOS8700_XYZ_DATA_CFG   0x0E
#define FXOS8700_CTRL_REG1      0x2A
#define FXOS8700_M_CTRL_REG1    0x5B
#define FXOS8700_M_CTRL_REG2    0x5C
#define FXOS8700_WHOAMI_VAL     0xC7
#define FXOS8700_X_OUT_MSB      0x01
#define FXOS8700_M_OUT_X_MSB    0x33
#define FXOS8700_M_STATUS       0x32

#define FXAS21002_WHOAMI        0x0C
#define FXAS21002_WHOAMI_VAL    0xD7
#define FXAS21002_CTRLREG0      0x0D
/* CTRL_REG0
 * [7:6] Lowpass cut-off freq
 * [5]   SPI interface mode
 * [4:3] Highpass cut-off freq
 * [2]   Highpass enable (1)
 * [1:0] Full-scale range select (0b00 max range, 0b11 max res)
*/
#define FXAS21002_CTRLREG1      0x13
/* CTRL_REG1
 * [7]   N/A
 * [6]   Software reset (1)
 * [5]   Self-Test enable (1)
 * [4:2] ODR select
 * [1:0] Mode: 00 Standy, 01 Ready 1X Active
*/
#define FXAS21002_OUT_X_MSB     0x01
#define FXAS21002_STATUS        0x07

/* Struct Declarations */

#ifndef axis3bit16_t
#define axis3bit16_t
typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;
#endif

/* Private variables */
static axis3bit16_t data_raw_accel;
static axis3bit16_t data_raw_gyro;
static axis3bit16_t data_raw_mag;
static double acceleration_mg[3];
static double angular_rate_mdps[3];
static double magnetic_field_mgauss[3];

/* Platform specific read/write functions */
static int32_t platform_write(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);
static int32_t platform_read(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);

int configure_nxp(SensorConfig* lsm);
double* read_nxp_acc(SensorConfig* lsm);
double* read_nxp_gyr(SensorConfig* lsm);
double* read_nxp_mag(SensorConfig* lsm);
