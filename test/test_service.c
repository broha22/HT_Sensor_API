/*
 * @Author: you@you.you
 * @Date:   Wednesday February 12th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday February 12th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTReadScheduler.h"
#include "../headers/HTSensors.h"

int main (int argc, char **argv) {

  /* Open shared memory and get variables ready */
  int shm_fd;
  const int shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);

  /* Initialize vars and pointers */
  unsigned int index = 0;
  void *shm_top = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  // void *read_top = mmap(config_top, shm_size_rd, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  /* Read the sensor config and then set it valid */
  SensorConfig config;
  memset(&config, 0, sizeof(SensorConfig));
  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = BSH;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = LSM;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Acc;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Gyr;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  config.index = index;
  config.addr = index + 1;
  config.sensor_type = Mag;
  config.driver_library = NXP;

  memcpy((SensorConfig *)shm_top + index, &config, sizeof(SensorConfig));
  index++;

  while (1) {
    for (int index = 0; index < MAX_SENSORS; index++) {
      memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
      if (config.valid) {
        printf("%d) %ld %ld %f \n", config.index, config.last_read.time.tv_sec, config.last_read.time.tv_nsec, config.last_read.x);
      }
    }
  }

  shm_unlink(shm_name);
}
