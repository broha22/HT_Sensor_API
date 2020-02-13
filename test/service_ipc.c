/*
 * @Author: you@you.you
 * @Date:   Wednesday February 12th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday February 12th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../headers/HTSensors.h"
#include <string.h>

int main (int argc, char **argv) {

  /* Open shared memory and get variables ready */
  int shm_fd;
  const int shm_size_cfg = (sizeof(SensorConfig) * MAX_SENSORS);
  const int shm_size_rd = (sizeof(SensorRead) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size_cfg + shm_size_rd);

  /* Initialize vars and pointers */
  unsigned int index = 0;
  void *config_top = mmap(0, shm_size_cfg, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  // void *read_top = mmap(config_top, shm_size_rd, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  void *read_top = config_top + MAX_SENSORS * sizeof(SensorConfig);

  /* Read the sensor config and then set it valid */
  SensorConfig read_config;
  memset(&read_config, 0, sizeof(SensorConfig));
  // memcpy(&read_config, (SensorConfig *)config_top + index * sizeof(SensorConfig), sizeof(SensorConfig));
  read_config.valid = 1;
  memcpy((SensorConfig *)config_top + index, &read_config, sizeof(SensorConfig));
  index++;
  // SensorConfig read_config2;
  // memset(&read_config2, 0, sizeof(SensorConfig));
  // memcpy(&read_config2, (SensorConfig *)config_top + index, sizeof(SensorConfig));
  // read_config2.valid = 1;
  memcpy((SensorConfig *)config_top + index, &read_config, sizeof(SensorConfig));

  while (1) {
    for (int index = 0; index < MAX_SENSORS; index++) {
      SensorRead read_current;
      memcpy(&read_current, (SensorRead *)read_top + index, sizeof(SensorRead));
      printf("%d) %ld %f \n", index, read_current.time.tv_nsec, read_current.x);
    }
  }

  shm_unlink(shm_name);
}
