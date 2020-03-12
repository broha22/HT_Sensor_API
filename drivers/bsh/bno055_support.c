  /*
 ******************************************************************************
 * @file    bno055_support.c
 * @author  Brogan Miner
 * @brief   This file implements functions for setting up and reading from
 *          the BNO055 inertial and magnetic sensors
 *
 ******************************************************************************
 */
#include "bno055_support.h"
#include "../../headers/HTSensors.h"

/*
 * @brief  Set up configuration registers and establish I2C communication
 *         channels for all sensors on the BNO055
 *
 * @param  config  configuration struct that contains all parameters necessary
 *                 necessary for correctly setting up the BNO055 for typical
 *                 operation
 */
int configure_bsh(SensorConfig *config) {
	I2C_routine(config->addr);
	config->fd = file_i2c;
	bno055_init(&bno055); //init sensor via driver function
  bno055_set_power_mode(BNO055_POWER_MODE_NORMAL); //set normal power mode

  //Configure units
  bno055_set_operation_mode(BNO055_OPERATION_MODE_CONFIG);
  bno055_set_accel_unit(BNO055_ACCEL_UNIT_MSQ); //acceleromter, m/s^2
  bno055_set_gyro_unit(BNO055_GYRO_UNIT_DPS); //gyro, degrees per second
  bno055_set_euler_unit(BNO055_EULER_UNIT_DEG); //euler, degrees

  bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);

	return 0;
}

/*
 * @brief  Read from Gyroscope
 *
 * @param  config  configuration struct that contains all parameters necessary
 *                 necessary for correctly setting up the BNO055 for typical
 *                 operation
 */
double *read_bsh_gyr(SensorConfig *config) {
  struct bno055_gyro_t gyro;
	memset(&gyro, 0, sizeof(struct bno055_gyro_t));
	bno055_read_gyro_xyz(&gyro);
  double *r = malloc(sizeof(double) * 3);
	r[0] = gyro.x;
	r[1] = gyro.y;
	r[2] = gyro.z;
	return r;
}

/*
 * @brief  Read from Accelerometer
 *
 * @param  config    configuration struct that contains all parameters necessary
 *                 necessary for correctly setting up the BNO055 for typical
 *                 operation
 */
double *read_bsh_acc(SensorConfig *config) {
	struct bno055_accel_t accel;
	memset(&accel, 0, sizeof(struct bno055_accel_t));
	bno055_read_accel_xyz(&accel);
  double *r = malloc(sizeof(double) * 3);
	r[0] = accel.x;
	r[1] = accel.y;
	r[2] = accel.z;
	return r;
}

/*
 * @brief  Read from Magnetometer
 *
 * @param  config  configuration struct that contains all parameters necessary
 *                 necessary for correctly setting up the BNO055 for typical
 *                 operation
 */
double *read_bsh_mag(SensorConfig *config) {
	struct bno055_mag_t mag;
	memset(&mag, 0, sizeof(struct bno055_mag_t));
	bno055_read_mag_xyz(&mag);
  double *r = malloc(sizeof(double) * 3);
	r[0] = mag.x;
	r[1] = mag.y;
	r[2] = mag.z;
	return r;
}

/*
 * @brief  Set up I2C communication over the shared bus
 *
 * @param  addr  specified I2C address for sensor to establish
 *               communication across the shared bus
 */
s8 I2C_routine(unsigned int addr) {
	bno055.bus_write = BNO055_I2C_bus_write;
	bno055.bus_read = BNO055_I2C_bus_read;
	bno055.delay_msec = BNO055_delay_msek;
	bno055.dev_addr = addr;

	// Open I2C file descriptor
	if ((file_i2c = open("/dev/i2c-1", O_RDWR)) < 0) {
    return -1;
  }
  // Establish slave address for the file
  if(ioctl(file_i2c, I2C_SLAVE, bno055.dev_addr) != 0) {
    return -1;
  }
  // Write ID to sensor to confirm identity
  u8 reg = BNO055_CHIP_ID_ADDR;
  if(write(file_i2c, &reg, 1) != 1) {
    return -1;
  }

	return BNO055_INIT_VALUE;
}

/*
 * @brief  Write register across I2C
 *
 * @param  dev_addr  I2C address of device
 * @param  reg_addr  register to write to
 * @param  reg_data  pointer to data to write in register
 * @param  cnt       number of consecutive registers to write
 *
 */
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  // Buffer to hold error value
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
  // Array for writing
  u8 array[I2C_BUFFER_LEN];
  // String position
  u8 stringpos = BNO055_INIT_VALUE;
	array[BNO055_INIT_VALUE] = reg_addr;
	for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] =
			*(reg_data + stringpos);
	}
  // Write operation
  if (write(file_i2c, array, cnt + 1) != cnt + 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  
  return (s8)BNO055_iERROR;
}

/*
 * @brief  Write register across I2C
 *
 * @param  dev_addr  I2C address of device
 * @param  reg_addr  register to read from
 * @param  reg_data  pointer to array for storing read data
 * @param  cnt       number of consecutive registers to read from
 *
 */
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  // Buffer to store error value
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
  // Array to hold read data
  u8 array[I2C_BUFFER_LEN] = {BNO055_INIT_VALUE};
  // String position
  u8 stringpos = BNO055_INIT_VALUE;

  // Send out address to read from
  if (write(file_i2c, &reg_addr, 1) != 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  sleep(0.15);
  // Check if read occured successfully
  if (read(file_i2c, array, cnt) != cnt) {
		printf("Failed to read from the i2c bus.\n");
	}
  // Copy data to param array
  for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos];
  }
	return (s8)BNO055_iERROR;
}

// Delay function
void BNO055_delay_msek(u32 msek) {
	sleep((double)msek / 1000.0);
}
