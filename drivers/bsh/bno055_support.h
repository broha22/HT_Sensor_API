#ifndef BNO055_SUPPORT
#define BNO055_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "bno055.h"
#define	I2C_BUFFER_LEN 8
#define I2C0 5
#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)
#include "../../headers/HTSensors.h"


struct bno055_t bno055;
int file_i2c;

s8 I2C_routine(unsigned int addr);
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BNO055_delay_msek(u32 msek);

int configure_bsh(SensorConfig *config);
double *read_bsh_gyr(SensorConfig *config);
double *read_bsh_acc(SensorConfig *config);
double *read_bsh_mag(SensorConfig *config);
#endif
