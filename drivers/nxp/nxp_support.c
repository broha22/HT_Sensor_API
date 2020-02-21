/*
 ******************************************************************************
 * @file    nxp_support.c
 * @author  Isaac Grossberg
 * @brief   This file implements functions for setting up and reading from
 *          the NXP inertial and magnetic sensors (FXAS21002 and FXOS8700)
 *
 ******************************************************************************
 */

#include "nxp_support.h"

/*
 * @brief  Undo the two's complement of a 16-bit value
 * 
 * @param  val  16-bit value in two's complement form
 */
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

/*
 * @brief  Set up configuration registers and establish I2C communication
 *         channels for all sensors on the FXAS21002 and FXOS8700
 *
 * @param  lsm    sensor struct containing information about
 *                I2C addresses and file descriptors for communication
 */
int configure_nxp(struct SensorConfig* nxp) {

  /* Set up I2C Comunication */
  int fd = wiringPiI2CSetup(nxp->addr);
  if (fd < 0) {
    printf("I2C Setup failed\n");
    return -1;
  }
    
  /* Fill struct with new fd */
  nxp->fd = fd;

  int ret = 0;
  uint8_t mask;
  uint8_t write_buf;
  uint8_t read_buf;

  if (nxp->sensor_type == Acc || nxp->sensor_type == Mag) {
    
    /* Check Device ID and compare against WhoAmI */
    ret += platform_read(&fd, FXOS8700_WHOAMI, &read_buf, 1);
    if (read_buf != FXOS8700_WHOAMI_VAL) {
      printf("Device not found\n");
      return -1;
    }

    /* Reset device
     * [0] = Standy/active 0b0 sets standby
     */      
    write_buf = 0x00;
    ret += platform_write(&fd, FXOS8700_CTRL_REG1, &write_buf, 1);
    do { /* Wait for system mode to update to standby */
      platform_read(&fd, FXOS8700_SYSMOD, &read_buf, 1);
    }while ( (read_buf & 0x03) != 0x00);
    
    /* Reset mag
     * [6] Magnetic one-shot reset
     */
    uint8_t mag_reset = 0x40;
    mask = 0x40;
    platform_read(&fd, FXOS8700_M_CTRL_REG1, &read_buf, 1);
    write_buf = ((read_buf & ~mask) | mag_reset);
    ret += platform_write(&fd, FXOS8700_M_CTRL_REG1, &write_buf, 1);

    /* Set mode to accel + mag
     * [1:0] = 0b11 Hybrid mode (both sensors active)
     * [4:2] = 0b111 Max Over sampling rate (16)
     */
    uint8_t mode_sel = 0x1F;
    mask = 0x1F;
    platform_read(&fd, FXOS8700_M_CTRL_REG1, &read_buf, 1);
    write_buf = (mode_sel & (read_buf & mask));
    ret += platform_write(&fd, FXOS8700_M_CTRL_REG1, &write_buf, 1);

    /* Set up CTRL REG 2 for no auto-inc (change to 0x20 for auto-inc) */
    uint8_t auto_inc = 0x00;
    mask = 0x20;
    platform_read(&fd, FXOS8700_M_CTRL_REG2, &read_buf, 1);
    write_buf = ((read_buf & ~mask) | auto_inc);
    ret += platform_write(&fd, FXOS8700_M_CTRL_REG2, &write_buf, 1);


    /* Set ranges on sensors
     * Magnetic range fixed to 1200 uT
     * Accel range 2g ([1:0] = 0b00)
     * Mag range 500 dps
     * [4] HPF enable
     */
    uint8_t range_set = 0x10;
    mask = 0x10;
    platform_read(&fd, FXOS8700_XYZ_DATA_CFG, &read_buf, 1);
    write_buf = ((read_buf & ~mask) | range_set);
    ret += platform_write(&fd, FXOS8700_XYZ_DATA_CFG, &write_buf, 1);


    /* Set output data rates + set active
     * [3] Reduced noise mode enable
     */
    uint8_t red_noise = 0x05;
    mask = 0x07;
    platform_read(&fd, FXOS8700_CTRL_REG1, &read_buf, 1);
    write_buf = ((read_buf & ~mask) | red_noise);
    ret += platform_write(&fd, FXOS8700_CTRL_REG1, &write_buf, 1);

  }
  else if (nxp->sensor_type == Gyr) {
    
    /* Check Device ID and compare against WhoAmI */    
    ret += platform_read(&fd, FXAS21002_WHOAMI, &read_buf, 1);
    if (read_buf != FXAS21002_WHOAMI_VAL) {
      printf("Device not found\n");
      return -1;
    }

    /* Reset device
     * [6] Software Reset
     */
    write_buf = 0x40;
    ret += platform_write(&fd, FXAS21002_CTRLREG1, &write_buf, 1);
    do {
      platform_read(&fd, FXAS21002_CTRLREG1, &read_buf, 1);
      printf("Device in reset\n");
    }while (read_buf == 0x40);

    /* Set range 500 dps
     * [7:6] Low pass filter cut off freq
     * [5] SPI interface mode
     * [4:3] High pass filter cut off freq
     * [2] High pass filter enable
     * [1:0] Full scale range selection (0b10 = 500 dps)
     */
    //uint8_t range_set = 0x1E;
    uint8_t range_set = 0x1C;
    mask = 0xFF;
    platform_read(&fd, FXAS21002_CTRLREG0, &read_buf, 1);
    write_buf = ((read_buf & ~mask) | range_set);
    ret += platform_write(&fd, FXAS21002_CTRLREG0, &write_buf, 1);
    
    /* Set ODR + set active
     * [6] Soft rst
     * [5] Selftest enable
     * [4:2] ODR 0b000 = 800 Hz
     * [1] Set active active
     * [0] Set standby mode
     */
    uint8_t odr_set = 0x02;
    mask = 0x1F;
    platform_read(&fd, FXAS21002_CTRLREG1, &write_buf, 1);
    write_buf = ((read_buf & ~mask) | odr_set);
    ret += platform_write(&fd, FXAS21002_CTRLREG1, &write_buf, 1);

  }
  else {
    printf("Invalid Sensor type\n");
    return -1;
  }

  /* Wait necessary transition time from standby to active time (6ms) */
  usleep(6 * 1000);

  return ret;
}

/*
 * @brief  Read from Accelerometer
 *
 * @param  nxp    sensor struct containing information about
 *                I2C addresses and file descriptors for communication
 *
 */
double* read_nxp_acc(struct SensorConfig* nxp) {

  uint8_t mask = 0x07;       //Mask for data ready bits
  uint8_t read_buf;
  int fd = nxp->fd;

  /* Allocate memory for sensor readings */
  double* acceleration_mg = (double*) malloc(sizeof(double)*3);

  /* Check if accel data ready */
  // [2:0] = 0b111 X,Y,Z data ready
  do {
    platform_read(&fd, FXOS8700_STATUS, &read_buf, 1);
  }while ( (read_buf & mask) != 0x07 );

  /* Read data from accel */
  // NOTE: LSB[7:2] have real data, [1:0] garbage values
  platform_read(&fd, FXOS8700_X_OUT_MSB, data_raw_accel.u8bit, 6);

  /* Convert accel data
   * 14 bit resolution
   * Range: 2g, Units: 0.244 mg/LSB
   * Range: 4g, Units: 0.488 mg/LSB 
   * Range: 8g, Units: 0.976 mg/LSB
   */
  double conv_factor = 0.244;
  *acceleration_mg = (double) undoComplement((data_raw_accel.i16bit[0] >> 2) * conv_factor);
  *(acceleration_mg+1) = (double) undoComplement((data_raw_accel.i16bit[1] >> 2) * conv_factor);
  *(acceleration_mg+2) = (double) undoComplement((data_raw_accel.i16bit[2] >> 2) * conv_factor);

  //*acceleration_mg = (double) (data_raw_accel.i16bit[0] >> 2) * conv_factor;
  //*(acceleration_mg+1) = (double) (data_raw_accel.i16bit[1] >> 2) * conv_factor;
  //*(acceleration_mg+2) = (double) (data_raw_accel.i16bit[2] >> 2) * conv_factor;
      
  return acceleration_mg;
}

/*
 * @brief  Read from Gyroscope
 *
 * @param  nxp    sensor struct containing information about
 *                I2C addresses and file descriptors for communication
 *
 */
 double* read_nxp_gyr(struct SensorConfig* nxp) {

  uint8_t mask = 0x07;       //Mask for data ready bits
  uint8_t read_buf;
  int fd = nxp->fd;

  /* Allocate memory for sensor readings */
  double* angular_rate_mdps = (double*) malloc(sizeof(double)*3);

  /* Check if data ready */
  do {
    platform_read(&fd, FXAS21002_STATUS, &read_buf, 1);
  }while ( (read_buf & mask ) != 0x07 );

  /* Read data from axes */
  platform_read(&fd, FXAS21002_OUT_X_MSB, data_raw_gyro.u8bit, 6);

  /* Convert from raw to mdps
   * Range: 2000, Units: 62.5 mdps/LSB
   * Range: 1000, Units: 31.25 mdps/LSB
   * Range: 500, Units: 15.625 mdps/LSB
   * Range: 250, Units: 7.8125 mdps/LSB
   */
  double conv_factor = 62.5; //15.625
  *angular_rate_mdps = (double) undoComplement(data_raw_gyro.i16bit[0] * conv_factor);
  *(angular_rate_mdps+1) = (double) undoComplement(data_raw_gyro.i16bit[1] * conv_factor);
  *(angular_rate_mdps+2) = (double) undoComplement(data_raw_gyro.i16bit[2] * conv_factor);

  //*angular_rate_mdps = (double) data_raw_gyro.i16bit[0] * conv_factor;
  //*(angular_rate_mdps+1) = (double) data_raw_gyro.i16bit[1] * conv_factor;
  //*(angular_rate_mdps+2) = (double) data_raw_gyro.i16bit[2] * conv_factor;
    
  return angular_rate_mdps;
 }

 /*
 * @brief  Read from Magnetometer
 *
 * @param  nxp    sensor struct containing information about
 *                I2C addresses and file descriptors for communication
 *
 */
 double* read_nxp_mag(struct SensorConfig* nxp) {

  uint8_t mask = 0x07;       //Mask for data ready bits
  uint8_t read_buf;
  int fd = nxp->fd;

  /* Allocate memory for sensor readings */
  double* magnetic_field_mgauss = (double*) malloc(sizeof(double)*3);

  /* Wait for mag data ready */
  // [2:0] = 0b111 X,Y,Z data ready
  do {
  platform_read(&fd, FXOS8700_M_STATUS, &read_buf, 1);
  }while ( (read_buf & mask) != 0x07 );

  /* Read data from mag */
  platform_read(&fd, FXOS8700_M_OUT_X_MSB, data_raw_mag.u8bit, 6);

  /* Convert mag data
   * Raw data has resolution of 0.1 uT/LSB or 1 mG/LSB
   * No unit conversion necessary
   */
  *magnetic_field_mgauss = (double) undoComplement(data_raw_mag.i16bit[0]) - 50000;
  *(magnetic_field_mgauss+1) = (double) undoComplement(data_raw_mag.i16bit[1]) - 50000;
  *(magnetic_field_mgauss+2) = (double) undoComplement(data_raw_mag.i16bit[2]) - 50000;

  //*magnetic_field_mgauss = (double) data_raw_mag.i16bit[0];
  //*(magnetic_field_mgauss+1) = (double) data_raw_mag.i16bit[1];
  //*(magnetic_field_mgauss+2) = (double) data_raw_mag.i16bit[2];

  return magnetic_field_mgauss;
 }


 /*
 * @brief  Write device register via I2C
 *
 * @param  handle    file descriptor
 * @param  reg       register to write to
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive registers to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
  int *fd = handle;

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

/*
 * @brief  Read device register via I2C
 *
 * @param  handle    file descriptor
 * @param  reg       register to read from
 * @param  bufp      pointer to buffer that stores the data read
 * @param  len       number of consecutive registers to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{

  int *fd = handle;

  int result;
  if (len == 1) {
    result = wiringPiI2CReadReg8(*fd, reg);
    *bufp = result;
    //printf("Read %X from register %X (len %d,fd %d)\n", *bufp, reg, len, *fd);
  }
  else if (len == 2) {
    result = wiringPiI2CReadReg16(*fd, reg);
    *bufp = result;
    //printf("Read %X from register %X (len %d,fd %d)\n", *bufp, reg, len, *fd);
  }
  else if (len == 6) {
    uint16_t x_result = wiringPiI2CReadReg16(*fd, reg);
    //printf("Raw read %X from register %X\n", x_result, reg);
    uint16_t y_result = wiringPiI2CReadReg16(*fd, reg+2);
    //printf("Raw read %X from register %X\n", y_result, reg+2);
    uint16_t z_result = wiringPiI2CReadReg16(*fd, reg+4);
    //printf("Raw read %X from register %X\n", z_result, reg+4);
    *bufp = x_result;
    *(bufp+1) = x_result >> 8;
    *(bufp+2) = y_result;
    *(bufp+3) = y_result >> 8;
    *(bufp+4) = z_result;
    *(bufp+5) = z_result >> 8;

    /*int i;
    for (i=0; i<6; i++) {
      printf("Read %X from register %X\n", (uint8_t) *(bufp+i),reg+i);
    }*/
  }
  else { return 1; }
  return 0;
}
