/*
 ******************************************************************************
 * @file    lsm9ds1_support.h
 * @author  Isaac Grossberg
 * @brief   This file declares variables and functions used in the
 *          lsm9ds1_support.c file
 *
 ******************************************************************************
 */

/* Struct Declarations */
typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

typedef struct sensor{
    uint8_t addr_accel;
    uint8_t addr_gyro;
    uint8_t addr_mag;
    int file_desc_accel;
    int file_desc_gyro;
    int file_desc_mag;
}sensor;

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
int Configure_lsm(struct sensor* lsm);
double* Read_lsm_accel(struct sensor* lsm);
double* Read_lsm_gyro(struct sensor* lsm);
double* Read_lsm_mag(struct sensor* lsm);