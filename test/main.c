#include <unistd.h>
#include "../headers/HTSensors.h"

int main () {

  clear_sensors();
  load_sensors("config.txt");


  int sensor_count;

  for (int i = 0; i < 600; i++) {
    read_sensors(&sensor_count, sensor_readings)

    for (int j = 0; j < sensor_count; j++) {
      printf("%llu, %llu, %d, %d, %d\n", sensor_readings[j].time, sensor_readings[j]->sensor.id,sensor_readings[j].x, sensor_readings[j].y, sensor_readings[j].z);
    }

    sleep(0.1);
  }

  return 0;
}
