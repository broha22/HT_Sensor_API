#include "bno055_support.h"
#include "../../headers/HTSensors.h"

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

s8 I2C_routine(unsigned int addr) {
	bno055.bus_write = BNO055_I2C_bus_write;
	bno055.bus_read = BNO055_I2C_bus_read;
	bno055.delay_msec = BNO055_delay_msek;
	bno055.dev_addr = addr;


	if ((file_i2c = open("/dev/i2c-1", O_RDWR)) < 0) {
    return -1;
  }
  if(ioctl(file_i2c, I2C_SLAVE, bno055.dev_addr) != 0) {
    return -1;
  }
  u8 reg = BNO055_CHIP_ID_ADDR;
  if(write(file_i2c, &reg, 1) != 1) {
    return -1;
  }

	return BNO055_INIT_VALUE;
}

s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN];
	u8 stringpos = BNO055_INIT_VALUE;
	array[BNO055_INIT_VALUE] = reg_addr;
	for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		array[stringpos + BNO055_I2C_BUS_WRITE_ARRAY_INDEX] =
			*(reg_data + stringpos);
	}

  if (write(file_i2c, array, cnt + 1) != cnt + 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  
  return (s8)BNO055_iERROR;
}

s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt) {
  s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[I2C_BUFFER_LEN] = {BNO055_INIT_VALUE};
	u8 stringpos = BNO055_INIT_VALUE;

  if (write(file_i2c, &reg_addr, 1) != 1) {
		printf("Failed to write to the i2c bus.\n");
	}
  sleep(0.15);
  if (read(file_i2c, array, cnt) != cnt) {
		printf("Failed to read from the i2c bus.\n");
	}
  for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++) {
		*(reg_data + stringpos) = array[stringpos];
  }
	return (s8)BNO055_iERROR;
}

void BNO055_delay_msek(u32 msek) {
	sleep((double)msek / 1000.0);
}
