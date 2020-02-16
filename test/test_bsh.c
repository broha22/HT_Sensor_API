/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTSensors.h"
#include "../drivers/bsh/bno055_support.h"

int main (int argc, char** argv) {

  SensorConfig config;
  config.addr = 0x28;
  configure_bsh(&config);

  int counter = 0;
  while (1) {
    double* reads;
    if (counter == 0) {
      printf("ACC ");
      reads = read_bsh_acc(&config);
    } else if (counter == 1) {
      printf("GYR ");
      reads = read_bsh_gyr(&config);
    } else {
      printf("MAG ");
      reads = read_bsh_mag(&config);
    }
    printf("X: %f, Y: %f, Z: %f\n", reads[0], reads[1], reads[2]);
    free(reads);
    counter = (counter + 1) % 3;
  }

  return 0;
}