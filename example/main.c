/*
 * @Author: you@you.you
 * @Date:   Sunday February 16th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Sunday February 16th 2020
 * @Copyright:  (c) Oregon State University 2020
 */

#include <HTSensors.h>
#include <string.h>

int main(int argc, char** argv) {
  init_ht_api();
  clear_sensors();
  load_sensors("/home/pi/HT_Sensor_API/example/config.txt");
  while (1) {
    SensorConfig *configs;
    int count;
    configs = read_sensors(&count);
    for (int index = 0; index < count; index++) {
      char driver[255];
      char type[255];
      switch (configs[index].driver_library) {
        case BSH:
          strcpy(driver, "BSH");
          break;
        case LSM:
          strcpy(driver, "LSM");
          break;
        case NXP:
          strcpy(driver, "NXP");
          break;
      }
      switch (configs[index].sensor_type) {
        case Gyr:
          strcpy(type, "GYR");
          break;
        case Acc:
          strcpy(type, "ACC");
          break;
        case Mag:
          strcpy(type, "MAG");
          break;
      }
      double time;
      time = configs[index].last_read.time.tv_sec;
      time += configs[index].last_read.time.tv_nsec / 1000000000.0;
      printf("%s %s T: %f X: %f Y: %f Z: %f\n", driver, type, time, configs[index].last_read.x, configs[index].last_read.y, configs[index].last_read.z);
    }
    free(configs);
    usleep(50000);
  }
}
