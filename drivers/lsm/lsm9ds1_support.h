/*
 ******************************************************************************
 * @file    lsm9ds1_support.h
 * @author  Isaac Grossberg
 * @brief   This file declares variables and functions used in the
 *          lsm9ds1_support.c file
 *
 ******************************************************************************
 */

/* Includes */
#include "../../headers/HTSensors.h"

/* Struct Declarations */
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
int Configure_lsm(struct SensorConfig*);
double* Read_lsm_accel(struct SensorConfig*);
double* Read_lsm_gyro(struct SensorConfig*);
double* Read_lsm_mag(struct SensorConfig*);
