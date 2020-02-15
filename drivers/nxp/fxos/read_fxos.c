/*
*   File: read_fxos.c
*   Author: Isaac Grossberg
*   Brief: This file implements functions for setting up and reading from
*          the gyroscope on the fxos8700cq sensor board
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPiI2C.h>

/* Register Address definitions */

#define FXOS8700_STATUS       0x00
#define FXOS8700_WHOAMI       0x0D
#define FXOS8700_XYZ_DATA_CFG 0x0E
#define FXOS8700_CTRL_REG1    0x2A
#define FXOS8700_M_CTRL_REG1  0x5B
#define FXOS8700_M_CTRL_REG2  0x5C
#define FXOS8700_WHOAMI_VAL   0xC7
#define FXOS8700_X_OUT_MSB    0x01
#define FXOS8700_M_OUT_X_MSB  0x33
#define FXOS8700_M_STATUS     0x32

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
} sensor;

/* Private variables */
static axis3bit16_t data_raw_accel;
static axis3bit16_t data_raw_mag;
static double accel_data_um_s2[3];
static double mag_data_mgauss[3];

/* Platform specific read/write functions */
static int32_t platform_write(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);
static int32_t platform_read(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);

int main() {

  uint8_t i2c_add = 0x1F;
  uint8_t buf;

  int fd = wiringPiI2CSetup(i2c_add);

  /* Check Device ID */
  int ret = platform_read(&fd, FXOS8700_WHOAMI, &buf, 1);

  /* Check against WhoAmI */
  if (buf != FXOS8700_WHOAMI_VAL) {
    printf("Device not found\n");
    return -1;
  }

  /* Put device in reset */
  uint8_t reset = 0x00;
  ret += platform_write(&fd, FXOS8700_CTRL_REG1, &reset, 1);
  reset = 0x20;
  ret += platform_write(&fd, FXOS8700_M_CTRL_REG1, &reset, 1);

  /* Set mode to accel + mag */
  // [1:0] = 0b11 Hybrid mode (both sensors active)
  buf = 0x03;
  ret += platform_write(&fd, FXOS8700_M_CTRL_REG1, &buf, 1);

  /* Set up CTRL REG 2 for no auto-inc (change to 0x20 for auto-inc) */
  buf = 0x00;
  ret += platform_write(&fd, FXOS8700_M_CTRL_REG2, &buf, 1);

  /* Set ranges on sensors */
  // Magnetic range fixed to 1200 uT
  // Accel range 2g ([1:0] = 0b00)
  buf = 0x00;
  ret += platform_write(&fd, FXOS8700_XYZ_DATA_CFG, &buf, 1);

  /* Set output data rates + set active */
  buf = 0x01;
  ret += platform_write(&fd, FXOS8700_CTRL_REG1, &buf, 1);

  int64_t transformed_data;  //Extended storage for data transformation operations
  uint8_t mask = 0x07; //Mask for data ready bits
  while (1) {

    /* Check if accel data ready */
    // [2:0] = 0b111 X,Y,Z data ready
    platform_read(&fd, FXOS8700_STATUS, &buf, 1);
    if ( (buf & mask) == 0x07) {

      /* Read data from accel */
      // NOTE: LSB[7:2] have real data
      platform_read(&fd, FXOS8700_X_OUT_MSB, data_raw_accel.u8bit, 6);

      /* Convert accel data */
      // Use a signed 64 bit var to hold transformed data
      // Given range = 2g, use 14 frac bits
      // 1. Multiply raw by 9806650
      // 2. Right shift by frac bits (14)
      // 3. Result is in micro m/s^2
      transformed_data = (int64_t) data_raw_accel.i16bit[0];
      transformed_data = (transformed_data * 9806650LL) >> 14;
      accel_data_um_s2[0] = transformed_data / 1000000; // um/s^2 -> m/s^2
      
      transformed_data = (int64_t) data_raw_accel.i16bit[1];
      transformed_data = (transformed_data * 9806650LL) >> 14;
      accel_data_um_s2[1] = transformed_data / 1000000; // um/s^2 -> m/s^2

      transformed_data = (int64_t) data_raw_accel.i16bit[2];
      transformed_data = (transformed_data * 9806650LL) >> 14;
      accel_data_um_s2[2] = transformed_data / 1000000; // um/s^2 -> m/s^2

      printf("Accel:\tx: %d,y: %d,z: %d\n", accel_data_um_s2[0], accel_data_um_s2[1], accel_data_um_s2[2]);
    }

    /* Check if mag data ready */
    // [2:0] = 0b111 X,Y,Z data ready
    platform_read(&fd, FXOS8700_M_STATUS, &buf, 1);
    if ( (buf & mask) == 0x07) {

      /* Read data from mag */
      platform_read(&fd, FXOS8700_M_OUT_X_MSB, data_raw_mag.u8bit, 6);

      /* Convert mag data */
      // Raw data has resolution of 0.1 uT / LSB or 1 mG / LSB
      // Conveniently, raw data is already in desired format
      mag_data_mgauss[0] = data_raw_mag.i16bit[0];
      mag_data_mgauss[1] = data_raw_mag.i16bit[0];
      mag_data_mgauss[2] = data_raw_mag.i16bit[0];

      printf("Mag:\tx: %d,y: %d,z: %d\n", mag_data_mgauss[0], mag_data_mgauss[1], mag_data_mgauss[2]);
    }
  }
}

static int32_t platform_write(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len)
{
  int* fd = handle;

  //printf("Writing %X to register %X (len %d,fd %d)\n", *bufp, reg, len, *fd);
  if (len == 1) {
    wiringPiI2CWriteReg8(*fd, reg, *bufp);
  }
  else if (len == 2) {
    wiringPiI2CWriteReg16(*fd, reg, *bufp);
  }
  else { return 1; }

  return 0;
}

static int32_t platform_read(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len)
{
  int* fd = handle;

  uint8_t msb, lsb;
  if (len == 1) {
    msb = wiringPiI2CReadReg8(*fd, reg);
    *bufp = msb;
  }
  else if (len == 2) {
    msb = wiringPiI2CReadReg8(*fd, reg);
    lsb = wiringPiI2CReadReg8(*fd, reg+1);
    *bufp = lsb;
    *(bufp+1) = msb;
  }
  else if (len == 6) {
    msb = wiringPiI2CReadReg8(*fd, reg);
    lsb = wiringPiI2CReadReg16(*fd, reg+1);
    *bufp = msb;
    *(bufp+1) = lsb;

    msb = wiringPiI2CReadReg8(*fd, reg+2);
    lsb = wiringPiI2CReadReg16(*fd, reg+3);
    *(bufp+2) = msb;
    *(bufp+3) = lsb;

    msb = wiringPiI2CReadReg8(*fd, reg+4);
    lsb = wiringPiI2CReadReg16(*fd, reg+5);
    *(bufp+4) = msb;
    *(bufp+5) = lsb;
  
  }
  else { return 1; }
  //printf("Read %X from register %X (len %d, fd %d)\n", *bufp, reg, len, *fd);
  return 0;mag_data_mgauss[0] = data_raw_mag.i16bit[0];
}

  

