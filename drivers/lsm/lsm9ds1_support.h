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
#include "../../headers/HTSensors.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPiI2C.h>
#include "lsm9ds1_reg.h"

/* Struct Declarations -------------------------------------------------------*/
typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

/*
typedef struct SensorConfig {
  unsigned int valid;
  unsigned int id;
  unsigned int addr;
  SensorType sensor_type; 
  DriverLibrary driver_library;
  int x_offset;
  int y_offset;
  int z_offset;
} SensorConfig; */

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis3bit16_t data_raw_magnetic_field;
static lsm9ds1_id_t whoamI;
static lsm9ds1_status_t reg;

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);

/* Extern functions ----------------------------------------------------------*/
int configure_lsm(struct SensorConfig*);
double* read_lsm_acc(struct SensorConfig*);
double* read_lsm_gyr(struct SensorConfig*);
double* read_lsm_mag(struct SensorConfig*);
