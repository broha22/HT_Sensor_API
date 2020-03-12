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
  //initialize the HTSensor Library
  init_ht_api();

  //Clear any sensors the service may have been tracking previously
  //then load the new sensors, see readme for config details
  clear_sensors()
  load_sensors("/home/pi/HT_Sensor_API/example/config.txt");

  while (1) { //continuous loop over reading sensor values
    
    //initialize memory space for storing sensor count and 
    //each read config/reading
    SensorConfig *configs;
    int count;

    //read sensors into variables
    configs = read_sensors(&count);

    //Loop through sensor readings and print their respective values
    for (int index = 0; index < count; index++) {
      char driver[255];
      char type[255];
      
      //Parse sensor config driver library enum to string
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

      //Parse sensor config sensor type enum to string
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

      //convert timespec to double of seconds
      double time;
      time = configs[index].last_read.time.tv_sec;
      time += configs[index].last_read.time.tv_nsec / 1000000000.0;
      
      //print a formatted string with the sensor values
      printf("%s %s T: %f X: %f Y: %f Z: %f\n", driver, type, time, configs[index].last_read.x, configs[index].last_read.y, configs[index].last_read.z);
    }

    //free the array since read_sensors allocates mem space on the heap
    free(configs);
    usleep(50000); //sleep for awhile
  }
}
