#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "../../headers/HTSensors.h"

/* Generic Sensor Information struct */
/*typedef struct sensor{
    uint8_t addr_accel;
    uint8_t addr_gyro;
    uint8_t addr_mag;
    int file_desc_accel;
    int file_desc_gyro;
    int file_desc_mag;
}sensor;*/

/* Function declarations */
int configure_nxp(struct SensorConfig*);
double* read_nxp_acc(struct SensorConfig*);
double* read_nxp_gyr(struct SensorConfig*);
double* read_nxp_mag(struct SensorConfig*);

int main() {
  struct SensorConfig nxp_acc;
  nxp_acc.sensor_type = 1;
  nxp_acc.addr = 0x1F; 
  struct SensorConfig nxp_gyr;
  nxp_gyr.sensor_type = 0;
  nxp_gyr.addr = 0x21;
  struct SensorConfig nxp_mag;
  nxp_mag.sensor_type = 2;
  nxp_mag.addr = 0x1F;
    
  int result1 = configure_nxp(&nxp_acc);
  int result2 = configure_nxp(&nxp_gyr);
  int result3 = configure_nxp(&nxp_mag);

  double nxp_accel_readings[3];
  double nxp_gyro_readings[3];
  double nxp_mag_readings[3];

  double* accel_return;
  double* gyro_return;
  double* mag_return;


  if (result1 == 0 && result2 == 0 && result3 == 0) {
    while(1) {
      
      accel_return = read_nxp_acc(&nxp_acc);
      gyro_return = read_nxp_gyr(&nxp_gyr);
      mag_return = read_nxp_mag(&nxp_mag);

      nxp_accel_readings[0] = *(accel_return);
      nxp_accel_readings[1] = *(accel_return+1);
      nxp_accel_readings[2] = *(accel_return+2);

      nxp_gyro_readings[0] = *(gyro_return);
      nxp_gyro_readings[1] = *(gyro_return+1);
      nxp_gyro_readings[2] = *(gyro_return+2);

      nxp_mag_readings[0] = *(mag_return);
      nxp_mag_readings[1] = *(mag_return+1);
      nxp_mag_readings[2] = *(mag_return+2);

      free(accel_return);
      free(gyro_return);
      free(mag_return);

      printf("IMU - [mg]: x:%4.2f\ty:%4.2f\tz:%4.2f\t[mdps]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_accel_readings[0], nxp_accel_readings[1], nxp_accel_readings[2],
              nxp_gyro_readings[0], nxp_gyro_readings[1], nxp_gyro_readings[2]);

      printf("MAG - [mG]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_mag_readings[0], nxp_mag_readings[1], nxp_mag_readings[2]);
 
      usleep(50 * 1000); // 500 ms
 
    }
  }
  else { printf("Configure returned %d %d %d", result1, result2, result3); }
  return 0;
}