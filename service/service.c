/*
 * @Author: you@you.you
 * @Date:   Tuesday February 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Tuesday February 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
// TODO: clean up header includes
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../headers/HTSensors.h"
#include "../headers/HTReadScheduler.h"
#include <string.h>
#include <wiringPi.h>
#include "../drivers/bsh/bno055_support.h"
#include "../drivers/bsh/nxp_support.h"
#include "../drivers/bsh/bno055_support.h"


void toggle_e_magnet(int on) {
  if (on == 1) {
    digitalWrite(E_MAG_PIN, 1);
  } else {
    digitalWrite(E_MAG_PIN, 0);
  }
}

void read_sensor(SensorRead *read) {
  struct sensor sensor;
  switch (read->sensor->sensor_type) {
    case Gyr:
      switch (read->sensor->driver_library) {
        case BSH:

          break;
        case LSM:
          break;
        case NXP:
          break;
        default:
          return;
      }
      break;
    case Acc:
      switch (read->sensor->driver_library) {
        case BSH:
          break;
        case LSM:
          break;
        case NXP:
          break;
        default:
          return;
      }
      break;
    case Mag:
      toggle_e_magnet(1);
      switch (read->sensor->driver_library) {
        case BSH:
          break;
        case LSM:
          break;
        case NXP:
          break;
        default:
          return;
      }
      toggle_e_magnet(0);
      break;
    default:
      return;
  }

  current_rd.x += 1;
  current_rd.y += 1;
  current_rd.z += 1;
  clock_gettime(CLOCK_REALTIME, &current_rd.time);
}


void handle_timer (union sigval arg) {
  /* Read arg ptr into values */
  unsigned int index = ((HTSigVal*)arg.sival_ptr)->index;
  unsigned int index_copy = index;
  void *cfg_top = ((HTSigVal*)arg.sival_ptr)->config_top;
  void *rd_top = ((HTSigVal*)arg.sival_ptr)->read_top;
  
  /* Copy sensor config from shared mem */
  SensorConfig current_cfg;
  memcpy(&current_cfg, (SensorConfig *)cfg_top + index, sizeof(SensorConfig));
  
  /* Loop until we get a valid sensor */
  while (current_cfg.addr == 0) {
    index = (index + 1) % MAX_SENSORS;
    /* If no sensors are valid dont loop infinitely */
    if (index == index_copy) break;
    memcpy(&current_cfg, (SensorConfig *)cfg_top + index, sizeof(SensorConfig));
  }

  //TODO: Make this work with more than one of each board
  struct sensor sensor;
  /* Set up sensor if it is not set up yet */
  if (current_cfg.valid == 0) {
    switch (current_cfg.driver_library) {
      case BSH:
        Configure_bsh(&sensor);
        break;
      case LSM:
        Configure_lsm(&sensor);
        break;
      case NXP:
        Configure_nxp(&sensor);
        break;
      default:
        current_cfg.addr = 0;
        break;
    }
    memcpy((SensorConfig *)cfg_top + index, &current_cfg, sizeof(SensorConfig));
    if (current_cfg.addr == 0) {
      return;
    }
  }
  
  /* Continue processing sensor read */
  SensorRead current_rd;
  memcpy(&current_rd, (SensorRead *)rd_top + index, sizeof(SensorRead));
  read_sensor(&current_rd);
  memcpy((SensorRead *)rd_top + index, &current_rd, sizeof(SensorRead));

  /* Increase read index */
  ((HTSigVal*)arg.sival_ptr)->index = (index + 1) % MAX_SENSORS;
}

int main (int argc, char **argv) {

  /* Set up electromagnet pin for output */
  if (wiringPiSetup () == -1)
    return -1;
  pinMode (E_MAG_PIN, OUTPUT);

  /* Open shared memory and get variables ready */
  int shm_fd;
  const int shm_size_cfg = (sizeof(SensorConfig) * MAX_SENSORS);
  const int shm_size_rd = (sizeof(SensorRead) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size_cfg + shm_size_rd);

  /* Create structure for timer event arg */
  HTSigVal event_arg;
  event_arg.index = 0;
  event_arg.config_top = mmap(0, shm_size_cfg, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  event_arg.read_top = event_arg.config_top + MAX_SENSORS * sizeof(SensorConfig);
  // event_arg.read_top = mmap(event_arg.config_top, shm_size_rd, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  
  if (event_arg.config_top == MAP_FAILED) {
    printf("config_top failed\n");
    return -1;
  }
  if (event_arg.read_top == MAP_FAILED) {
    printf("read_top failed\n");
    return -1;
  }

  /* Create default sensor config and read structs */
  for (int i = 0; i < MAX_SENSORS; i++) {
    SensorConfig default_config;
    memset(&default_config, 0, sizeof(SensorConfig));
    memcpy((SensorConfig *)event_arg.config_top + i, &default_config, sizeof(SensorConfig));
    SensorRead default_read;
    memset(&default_read, 0, sizeof(SensorRead));
    memcpy((SensorConfig *)event_arg.read_top + i, &default_read, sizeof(SensorRead));
  }

  /* Define timer signal event */
  struct sigevent timer_fired;
  memset(&timer_fired, 0, sizeof(struct sigevent));
  timer_fired.sigev_notify = SIGEV_THREAD;
  timer_fired.sigev_notify_function = handle_timer;
  timer_fired.sigev_notify_attributes = NULL;
  timer_fired.sigev_value.sival_ptr = &event_arg;

  /* Define timer interval */
  struct itimerspec timer_interval;
  memset(&timer_interval, 0, sizeof(struct itimerspec));
  timer_interval.it_interval.tv_sec = 0;
  timer_interval.it_interval.tv_nsec = READ_INTERVAL;
  timer_interval.it_value.tv_nsec = 1;
  timer_interval.it_value.tv_sec = 0;



  /* initialize timer */
  timer_t timer_id;
  if (timer_create(CLOCK_REALTIME, &timer_fired, &timer_id)) {
    return errno;
  }
  if (timer_settime(timer_id, 0, &timer_interval, NULL)) {
    return errno;
  }

  /* Loop forever */
  while(1){}

  // Probably should capture sigterm unlink this
  shm_unlink(shm_name);
  return 0;
}