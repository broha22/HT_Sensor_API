/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTSensors.h"
#include "../headers/HTReadScheduler.h"

void toggle_e_magnet(int on) {
  if (on == 1) {
    digitalWrite(E_MAG_PIN, 1);
  } else {
    digitalWrite(E_MAG_PIN, 0);
  }
}

int setup_sensor (SensorConfig *config) {
  switch (config->driver_library) {
    case BSH:
      configure_bsh(config);
      break;
    case LSM:
      configure_lsm(config);
      break;
    case NXP:
      configure_nxp(config);
      break;
    default:
      return -1;
  }
  return 0;
}

void read_sensor (SensorConfig *config) {
  double* reads;
  switch (config->sensor_type) {
    case Gyr:
      switch (config->driver_library) {
        case BSH:
          reads = read_bsh_gyr(config);
          break;
        case LSM:
          reads = read_lsm_gyr(config);
          break;
        case NXP:
          reads = read_nxp_gyr(config);
          break;
        default:
          break;
      }
    case Acc:
      switch (config->driver_library) {
        case BSH:
          reads = read_bsh_acc(config);
          break;
        case LSM:
          reads = read_lsm_acc(config);
          break;
        case NXP:
          reads = read_nxp_acc(config);
          break;
        default:
          break;
      }
      break;
    case Mag:
      toggle_e_magnet(1);
      switch (config->driver_library) {
        case BSH:
          reads = read_bsh_mag(config);
          break;
        case LSM:
          reads = read_lsm_mag(config);
          break;
        case NXP:
          reads = read_nxp_mag(config);
          break;
        default:
          break;
      }
      toggle_e_magnet(0);
      break;
    default:
      break;
  }
  config->last_read.x = reads[0];
  config->last_read.y = reads[1];
  config->last_read.z = reads[2];
  free(reads);
  clock_gettime(CLOCK_REALTIME, &config->last_read.time);
}
int main (int argc, char** argv) {
  /* Look at argv for shared memory name of sensor config */
  if (argc != 2) {
    printf("Missing index %d \n", argc);
    return -1;
  }
  char *index_string = argv[1];
  int index = strtol(index_string, NULL, 10);

  int shm_fd;
  const int shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);
  void *config_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  /* Copy sensor config struct into memory */
  SensorConfig config;
  memcpy(&config, (SensorConfig *)config_ptr + index, sizeof(SensorConfig));

  /* Run setup function for sensor */
  if (setup_sensor(&config) != 0) {
    printf("Invalid sensor config \n");
    return -1;
  }
  config.valid = 1;
  config.command = HTC_WAIT;
  memcpy((SensorConfig *)config_ptr + index, &config, sizeof(SensorConfig));

  /* Loop continuously */
  int loop = 1;
  while (loop == 1) {
    memcpy(&config, (SensorConfig *)config_ptr + index, sizeof(SensorConfig));
    int index_copy;
    switch (config.command) {
      case HTC_READ:
        read_sensor(&config);
        config.command = HTC_WAIT;
        memcpy((SensorConfig *)config_ptr + index, &config, sizeof(SensorConfig));
        break;
      case HTC_DELETE:
        index_copy = config.index;
        memset(&config, 0, sizeof(SensorConfig));
        config.index = index_copy;
        memcpy((SensorConfig *)config_ptr + index, &config, sizeof(SensorConfig));
        loop = 0;
        return 0;
        break;
      case HTC_SETUP:
        loop = 0;
        break;
      default:
        break;
    }
  }

  shm_unlink(shm_name);
  return 0;
}
