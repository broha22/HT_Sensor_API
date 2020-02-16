/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */

#include "../headers/HTSensors.h"

void read_shm () {
  int shm_fd;
  const int shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);
  void *shm_top = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);


  for (int index = 0; index < MAX_SENSORS; index++) {
    SensorConfig config;
    memcpy(&config, (SensorConfig *)shm_top + index,  sizeof(SensorConfig));
    printf("Valid: %u \n", config.valid);
    printf("Index: %u \n", config.index);
    printf("Address: %u \n", config.addr);
    if (config.driver_library == BSH) {
      printf("Driver: BSH \n");
    } else if (config.driver_library == LSM) {
      printf("Driver: LSM \n");
    } else if (config.driver_library == NXP) {
      printf("Driver: NXP \n");
    } else {
      printf("Driver: Unknown \n");
    }
    if (config.sensor_type == Gyr) {
      printf("Sensor: Gyr \n");
    } else if (config.sensor_type == Acc) {
      printf("Sensor: Acc \n");
    } else if (config.sensor_type == Mag) {
      printf("Sensor: Mag \n");
    } else {
      printf("Sensor: Unknown \n");
    }
    printf("X Offset: %d\n", config.x_offset);
    printf("Y Offset: %d\n", config.y_offset);
    printf("Z Offset: %d\n", config.z_offset);
    printf("\n");
  }
}

int main (int argc, char** argv) {
  init_ht_api();
  const char* config = "../test/config.txt";
  load_sensors(config);
  read_shm();
  int count;
  SensorConfig* reads = read_sensors(&count);
  for (int index = 0; index < count; index++) {
    if (reads[index].driver_library == BSH) {
      printf("Driver: BSH\n");
    } else if (reads[index].driver_library == NXP) {
      printf("Driver: NXP\n");
    } else if (reads[index].driver_library == LSM) {
      printf("Driver: LSM\n");
    }
    if (reads[index].sensor_type == Gyr) {
      printf("Sensor: Gyr\n");
    } else if (reads[index].sensor_type == Acc) {
      printf("Sensor: Acc\n");
    } else if (reads[index].sensor_type == Mag) {
      printf("Sensor: Mag\n");
    }
    printf("X: %f\n", reads[index].last_read.x);
    printf("Y: %f\n", reads[index].last_read.y);
    printf("Z: %f\n", reads[index].last_read.z);
    printf("\n");
  }
  free(reads);
  clear_sensors();
  read_shm();
  dealloc_ht_api();
  return 0;
}