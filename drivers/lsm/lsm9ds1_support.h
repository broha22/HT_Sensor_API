/*
 ******************************************************************************
 * @file    lsm9ds1_support.h
 * @author  Isaac Grossberg
 * @brief   This file declares variables and functions used in the
 *          lsm9ds1_support.c file
 *
 ******************************************************************************
 */
#include <stdlib.h>
#include "lsm9ds1_reg.h"
#include "../../headers/HTSensors.h"

/* Struct Declarations */
#ifndef axis3bit16_t
#define axis3bit16_t
typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;
#endif

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t data_raw_magnetic_field;
static double acceleration_mg[3];
static double angular_rate_mdps[3];
static double magnetic_field_mgauss[3];
static lsm9ds1_id_t whoamI;
static lsm9ds1_status_t reg;
static uint8_t rst;

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

/* Extern functions ----------------------------------------------------------*/
int configure_lsm(SensorConfig* lsm);
double* read_lsm_acc(SensorConfig* lsm);
double* read_lsm_gyr(SensorConfig* lsm);
double* read_lsm_mag(SensorConfig* lsm);
