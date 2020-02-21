#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "../../headers/HTSensors.h"

/* Function declarations */
int configure_nxp(struct SensorConfig*);
double* read_nxp_acc(struct SensorConfig*);
double* read_nxp_gyr(struct SensorConfig*);
double* read_nxp_mag(struct SensorConfig*);

int main() {
  struct SensorConfig nxp_acc;
  nxp_acc.sensor_type = Acc;
  nxp_acc.addr = 0x1F; 
  struct SensorConfig nxp_gyr;
  nxp_gyr.sensor_type = Gyr;
  nxp_gyr.addr = 0x21;
  struct SensorConfig nxp_mag;
  nxp_mag.sensor_type = Mag;
  nxp_mag.addr = 0x1F;
    
  int result1 = configure_nxp(&nxp_acc);
  int result2 = configure_nxp(&nxp_gyr);
  int result3 = configure_nxp(&nxp_mag);

  double nxp_acc_readings[3];
  double nxp_gyr_readings[3];
  double nxp_mag_readings[3];

  double* acc_return;
  double* gyr_return;
  double* mag_return;


  if (result1 == 0 && result2 == 0 && result3 == 0) {
    while(1) {
      
      acc_return = read_nxp_acc(&nxp_acc);
      gyr_return = read_nxp_gyr(&nxp_gyr);
      mag_return = read_nxp_mag(&nxp_mag);

      nxp_acc_readings[0] = *(acc_return);
      nxp_acc_readings[1] = *(acc_return+1);
      nxp_acc_readings[2] = *(acc_return+2);

      nxp_gyr_readings[0] = *(gyr_return);
      nxp_gyr_readings[1] = *(gyr_return+1);
      nxp_gyr_readings[2] = *(gyr_return+2);

      nxp_mag_readings[0] = *(mag_return);
      nxp_mag_readings[1] = *(mag_return+1);
      nxp_mag_readings[2] = *(mag_return+2);

      free(acc_return);
      free(gyr_return);
      free(mag_return);

      printf("ACC - [mg]: x:%4.2f\ty:%4.2f\tz:%4.2f\t\r\n",
              nxp_acc_readings[0], nxp_acc_readings[1], nxp_acc_readings[2]);

      printf("GYR - [mdps]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_gyr_readings[0], nxp_gyr_readings[1], nxp_gyr_readings[2]);

      printf("MAG - [mG]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_mag_readings[0], nxp_mag_readings[1], nxp_mag_readings[2]);
 
      usleep(200 * 1000); // 200 ms
 
    }
  }
  else { printf("Configure returned %d %d %d\n", result1, result2, result3); }
  return 0;
}
