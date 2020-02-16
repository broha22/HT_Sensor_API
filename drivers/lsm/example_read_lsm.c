#include "lsm9ds1_reg.h"
#include "lsm9ds1_support.h"
#include <stdio.h>
#include <string.h>

/* Function declarations
int Configure_lsm(struct SensorConfig*);
double* Read_lsm_accel(struct SensorConfig*);
double* Read_lsm_gyro(struct SensorConfig*);
double* Read_lsm_mag(struct SensorConfig*); */

int main() {
  struct SensorConfig lsm_accel;
  lsm_accel.sensor_type = 1;
  lsm_accel.addr = 0x6a;
  struct SensorConfig lsm_gyro;
  lsm_gyro.sensor_type = 0;
  lsm_gyro.addr = 0x6a;
  struct SensorConfig lsm_mag;
  lsm_mag.sensor_type = 2;
  lsm_mag.addr = 0x1c;
  
    
  int result1 = Configure_lsm(&lsm_accel);
  int result2 = Configure_lsm(&lsm_gyro);
  int result3 = Configure_lsm(&lsm_mag);

  double lsm_accel_readings[3];
  double lsm_gyro_readings[3];
  double lsm_mag_readings[3];

  double* accel_return;
  double* gyro_return;
  double* mag_return;

  if (result1 == 0 && result2 == 0 && result3 == 0) {
    while(1) {
   
      accel_return = Read_lsm_accel(&lsm_accel);
      gyro_return = Read_lsm_gyro(&lsm_gyro);
      mag_return = Read_lsm_mag(&lsm_mag);

      lsm_accel_readings[0] = *(accel_return);
      lsm_accel_readings[1] = *(accel_return+1);
      lsm_accel_readings[2] = *(accel_return+2);

      lsm_gyro_readings[0] = *(gyro_return);
      lsm_gyro_readings[1] = *(gyro_return+1);
      lsm_gyro_readings[2] = *(gyro_return+2);

      lsm_mag_readings[0] = *(mag_return);
      lsm_mag_readings[1] = *(mag_return+1);
      lsm_mag_readings[2] = *(mag_return+2);

      printf("IMU - [mg]: x:%4.2f\ty:%4.2f\tz:%4.2f\t[mdps]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              lsm_accel_readings[0], lsm_accel_readings[1], lsm_accel_readings[2],
              lsm_gyro_readings[0], lsm_gyro_readings[1], lsm_gyro_readings[2]);

      printf("MAG - [mG]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              lsm_mag_readings[0], lsm_mag_readings[1], lsm_mag_readings[2]);
    }
  }
  return 0;
}
