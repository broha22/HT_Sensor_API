/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTSensors.h"
#include "../headers/HTReadScheduler.h"

//runs config command for sensor based on
//driver library
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

//read sensor for type and library
void read_sensor (SensorConfig *config) {
  double* reads; //reads is allocated in sensor lib by spec
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
      break;
    default:
      break;
  }
  //put reads into sensor config
  config->last_read.x = reads[0];
  config->last_read.y = reads[1];
  config->last_read.z = reads[2];
  free(reads); //reads is allocated in sensor lib by spec
  clock_gettime(CLOCK_REALTIME, &config->last_read.time);
}
int main (int argc, char** argv) {
  /* Look at argv for shared memory name of sensor config */
  if (argc != 2) {
    printf("Missing index %d \n", argc);
    return -1;
  }
  //Convert string to integer
  char *index_string = argv[1];
  int index = strtol(index_string, NULL, 10);
  int max_index = index + MAX_SENSORS / 2;

  //open up the shared memory stuff
  int shm_fd;
  const int shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);
  void *config_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  SensorConfig config;
  /* Loop continuously */
  int loop = 1;
  while (loop == 1) {
    for (int i = index; i < max_index; i++) { //loop through shm from spec index (0-8 or 9-18)
      memcpy(&config, (SensorConfig *)config_ptr + i, sizeof(SensorConfig)); //copy in current sensor
      int index_copy;
      switch (config.command) { //switch through sensor commands
        case HTC_READ: //sensor read
          read_sensor(&config);
          //reset the command flag
          config.command = HTC_WAIT;
          memcpy((SensorConfig *)config_ptr + i, &config, sizeof(SensorConfig));
          break;
        case HTC_DELETE: //remove the sensor from the shm
          index_copy = config.index;
          memset(&config, 0, sizeof(SensorConfig)); //clear our config
          config.index = index_copy; //set the index back and write it to shm
          memcpy((SensorConfig *)config_ptr + i, &config, sizeof(SensorConfig));
          break;
        case HTC_SETUP: //set up the sensor
            if (setup_sensor(&config) != 0) {
              printf("Invalid sensor config \n");
              config.command = HTC_DELETE; //something bad happened and we should remove the invalid sensor
            } else {
              //set the sensor valid and reset command
              config.valid = 1;
              config.command = HTC_WAIT;
            }
            memcpy((SensorConfig *)config_ptr + i, &config, sizeof(SensorConfig));
          break;
        default:
          break;
      }
    }
  }
  //unreachable
  shm_unlink(shm_name);
  return 0;
}
