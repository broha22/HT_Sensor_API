#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* Generic Sensor Information struct */
typedef struct sensor{
    uint8_t addr_accel;
    uint8_t addr_gyro;
    uint8_t addr_mag;
    int file_desc_accel;
    int file_desc_gyro;
    int file_desc_mag;
}sensor;

/* Function declarations */
int Configure_nxp(struct sensor* nxp);
double* Read_nxp_accel(struct sensor* nxp);
double* Read_nxp_gyro(struct sensor* nxp);
double* Read_nxp_mag(struct sensor* nxp);

int main() {
  struct sensor nxp;
    
  int result = Configure_nxp(&nxp);

  double nxp_accel_readings[3];
  double nxp_gyro_readings[3];
  double nxp_mag_readings[3];

  double* accel_return;
  double* gyro_return;
  double* mag_return;


  if (result == 0) {
    while(1) {
      
      accel_return = Read_nxp_accel(&nxp);
      gyro_return = Read_nxp_gyro(&nxp);
      mag_return = Read_nxp_mag(&nxp);

      nxp_accel_readings[0] = *(accel_return);
      nxp_accel_readings[1] = *(accel_return+1);
      nxp_accel_readings[2] = *(accel_return+2);

      nxp_gyro_readings[0] = *(gyro_return);
      nxp_gyro_readings[1] = *(gyro_return+1);
      nxp_gyro_readings[2] = *(gyro_return+2);

      nxp_mag_readings[0] = *(mag_return);
      nxp_mag_readings[1] = *(mag_return+1);
      nxp_mag_readings[2] = *(mag_return+2);

      printf("IMU - [mg]: x:%4.2f\ty:%4.2f\tz:%4.2f\t[mdps]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_accel_readings[0], nxp_accel_readings[1], nxp_accel_readings[2],
              nxp_gyro_readings[0], nxp_gyro_readings[1], nxp_gyro_readings[2]);

      printf("MAG - [mG]: x:%4.2f\ty:%4.2f\tz:%4.2f\r\n",
              nxp_mag_readings[0], nxp_mag_readings[1], nxp_mag_readings[2]);
 
      usleep(500 * 1000); // 500 ms
 
    }
  }
  else { printf("Configure returned %d", result); }
  return 0;
}
