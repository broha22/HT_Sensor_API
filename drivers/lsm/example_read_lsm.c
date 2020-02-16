#include "lsm9ds1_reg.h"
#include "lsm9ds1_support.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  struct SensorConfig lsm_acc;
  lsm_acc.sensor_type = 1;
  lsm_acc.addr = 0x6a;
  struct SensorConfig lsm_gyr;
  lsm_gyr.sensor_type = 0;
  lsm_gyr.addr = 0x6a;
  struct SensorConfig lsm_mag;
  lsm_mag.sensor_type = 2;
  lsm_mag.addr = 0x1c;
  
    
  int result1 = configure_lsm(&lsm_acc);
  int result2 = configure_lsm(&lsm_gyr);
  int result3 = configure_lsm(&lsm_mag);

  double lsm_accel_readings[3];
  double lsm_gyro_readings[3];
  double lsm_mag_readings[3];

  double* accel_return;
  double* gyro_return;
  double* mag_return;

  if (result1 == 0 && result2 == 0 && result3 == 0) {
    while(1) {
   
      accel_return = read_lsm_acc(&lsm_acc);
      gyro_return = read_lsm_gyr(&lsm_gyr);
      mag_return = read_lsm_mag(&lsm_mag);

      lsm_accel_readings[0] = *(accel_return);
      lsm_accel_readings[1] = *(accel_return+1);
      lsm_accel_readings[2] = *(accel_return+2);

      lsm_gyro_readings[0] = *(gyro_return);
      lsm_gyro_readings[1] = *(gyro_return+1);
      lsm_gyro_readings[2] = *(gyro_return+2);

      lsm_mag_readings[0] = *(mag_return);
      lsm_mag_readings[1] = *(mag_return+1);
      lsm_mag_readings[2] = *(mag_return+2);

      free(accel_return);
      free(gyro_return);
      free(mag_return);

      printf("IMU - [mg]: x:%4.2f\ty:%4.2f\tz:%4.2f\t[mdps]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              lsm_accel_readings[0], lsm_accel_readings[1], lsm_accel_readings[2],
              lsm_gyro_readings[0], lsm_gyro_readings[1], lsm_gyro_readings[2]);

      printf("MAG - [mG]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              lsm_mag_readings[0], lsm_mag_readings[1], lsm_mag_readings[2]);
    }
  }
  return 0;
}
