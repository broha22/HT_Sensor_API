/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../lsm/lsm9ds1_support.h"

int configure_lsm (SensorConfig *config) {
  config->fd = 1;
  return 0;
}

double* read_lsm_acc (SensorConfig *config) {
  double* r = malloc(sizeof(double) * 3);
  r[0] = 1.0;
  r[1] = 1.0;
  r[2] = 1.0;
  return r;
}

double* read_lsm_gyr (SensorConfig *config) {
  double* r = malloc(sizeof(double) * 3);
  r[0] = 1.0;
  r[1] = 1.0;
  r[2] = 1.0;
  return r;
}

double* read_lsm_mag (SensorConfig *config) {
  double* r = malloc(sizeof(double) * 3);
  r[0] = 1.0;
  r[1] = 1.0;
  r[2] = 1.0;
  return r;
}