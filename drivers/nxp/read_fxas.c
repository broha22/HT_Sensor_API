/*
*   File: read_fxas.c
*   Author: Isaac Grossberg
*   Brief: This file implements functions for setting up and reading from
*          the gyroscope on the fxas21002 sensor
*/

#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <wiringPiI2C.h>

/* Register Address definitions */

#define FXAS21002_REG_WHOAMI    0x0C
#define WHO_AM_I                0xD7
#define FXAS21002_REG_CTRLREG0  0x0D
#define FXAS21002_REG_CTRLREG1  0x13
#define FXAS21002_REG_OUT_XMSB  0x01
#define FXAS21002_REG_DR_STATUS 0x07

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
static axis3bit16_t data_raw_gyro;
static double angular_rate_mdps[3];

/* Platform specific read/write functions */
static int32_t platform_write(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);
static int32_t platform_read(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len);

uint16_t undoComplement(uint16_t val) {
  
  uint16_t mask = 0x8000;
  int flag = 0;
  if ( (val & mask) > 0 ) {
    flag = 1; //input is negative
  }

  val = ~(val);
  val = val + 1; 
  if (flag) {
    val = 0 - val;
  }

  return val;
}

int main() {
  
  uint8_t i2c_add = 0x21;
  uint8_t buf;

  int fd = wiringPiI2CSetup(i2c_add);
  /* Check device ID */
  // read from FXAS21002_REG_WHOAMI (0x0C) len=1
  int ret = platform_read(&fd, FXAS21002_REG_WHOAMI, &buf, 1);
  //Check against WhoAmI
  // compare to 0xD7 (factory programmed device id)
  if (buf != WHO_AM_I) {
    printf("Device not found\n");
    return -1;
  }

  /* Restore default device configuration */
  // write 0x40 to FXAS21002_REG_CTRLREG1 (0x13)
  uint8_t reset = 0x40;
  ret += platform_write(&fd, FXAS21002_REG_CTRLREG1, &reset, 1);
  //Wait for reset to finish 
  // read from FXAS21002_REG_CTRLREG1 (0x13)
  // check if (ctrlreg1 == 0x40)
  do {
    platform_read(&fd, FXAS21002_REG_CTRLREG1, &buf, 1);
  }while (buf == reset);

  /* Set range to be 500 dps */
  // write 0x03 to FXAS21002_REG_CTRLREG0 (0x0D)
  uint8_t range = 0x03;
  ret += platform_write(&fd, FXAS21002_REG_CTRLREG0, &range, 1);

  /* Set output data rate + set active */
  // write 0x03 to FXAS21002_REG_CTRLREG1 (0x13)
  uint8_t active = 0x03;
  ret += platform_write(&fd, FXAS21002_REG_CTRLREG1, &active, 1);

  /* Wait necessary transition time from standby to active time */
  // sleep 6 ms
  usleep(6 * 1000);

  while (1) {

    /* Check if data ready */
    platform_read(&fd, FXAS21002_REG_DR_STATUS, &buf, 1);
    int mask = 0x08; //mask out all but ZYXDR bit
    if ( (buf & mask ) > 0 ) {

      /* Read data from axes */
      // read from FXAS21002_REG_OUT_XMSB (0x01) len=6
      platform_read(&fd, FXAS21002_REG_OUT_XMSB, data_raw_gyro.u8bit, 6);

      /* Convert from raw to micro radians / second */
      angular_rate_mdps[0] = (double) data_raw_gyro.i16bit[0] * 62500 / 500;
      angular_rate_mdps[1] = (double) data_raw_gyro.i16bit[1] * 62500 / 500;
      angular_rate_mdps[2] = (double) data_raw_gyro.i16bit[2] * 62500 / 500;

      angular_rate_mdps[0] = undoComplement(angular_rate_mdps[0]);
      angular_rate_mdps[1] = undoComplement(angular_rate_mdps[1]);
      angular_rate_mdps[2] = undoComplement(angular_rate_mdps[2]);
      //double x_val = (double) (readings[0] * 62500) >> 500;
      printf("x: %d y: %d z: %d\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);

      //return angular_rate_mdps;
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

  int msb, lsb;
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
    lsb = wiringPiI2CReadReg8(*fd, reg+1);
    *bufp = lsb;
    *(bufp+1) = msb;

    msb = wiringPiI2CReadReg8(*fd, reg+2);
    lsb = wiringPiI2CReadReg8(*fd, reg+3);
    *(bufp+2) = lsb;
    *(bufp+3) = msb;
    
    msb = wiringPiI2CReadReg8(*fd, reg+4);
    lsb = wiringPiI2CReadReg8(*fd, reg+5);
    *(bufp+4) = lsb;
    *(bufp+5) = msb;
  }
  else { return 1; }
  //printf("Read %X from register %X (len %d, fd %d)\n", *bufp, reg, len, *fd);
  return 0;
}
