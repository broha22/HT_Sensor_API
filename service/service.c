/*
 * @Author: you@you.you
 * @Date:   Tuesday February 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Tuesday February 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
// #define HT_DRIVER_PROCESS "./driver_process"
#include "../headers/HTSensors.h"
#include "../headers/HTReadScheduler.h"

void toggle_e_magnet(int on) {
  if (on == 1) {
    digitalWrite(E_MAG_PIN, 1);
  } else {
    digitalWrite(E_MAG_PIN, 0);
  }
}

void handle_timer_mag (union sigval arg) {
  void *shm_top = ((HTSigVal*)arg.sival_ptr)->shm_top;
  while (((HTSigVal*)arg.sival_ptr)->busy == 1) { usleep(10); }
  ((HTSigVal*)arg.sival_ptr)->busy = 1;
  SensorConfig current_cfg;
  toggle_e_magnet(1);
  for (int i = 0; i < MAX_SENSORS; i++) {
    memcpy(&current_cfg, (SensorConfig *)shm_top + i, sizeof(SensorConfig));
    if (current_cfg.sensor_type == Mag && current_cfg.fd != 0) {
      current_cfg.command = HTC_READ;
      memcpy((SensorConfig *)shm_top + i, &current_cfg, sizeof(SensorConfig));
    }
  }
  toggle_e_magnet(0);
  ((HTSigVal*)arg.sival_ptr)->busy = 0;
}

void handle_timer (union sigval arg) {
  if (((HTSigVal*)arg.sival_ptr)->busy == 1) {
    return;
  }
  ((HTSigVal*)arg.sival_ptr)->busy = 1;
  /* Read arg ptr into values */
  unsigned int index = ((HTSigVal*)arg.sival_ptr)->index;
  unsigned int index_copy = index;
  void *shm_top = ((HTSigVal*)arg.sival_ptr)->shm_top;
  
  /* Copy sensor config from shared mem */
  SensorConfig current_cfg;
  memcpy(&current_cfg, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
  // printf("addr %d\n", current_cfg.addr);
  /* Loop until we get a valid sensor */
  while (current_cfg.addr == 0) {
    index = (index + 1) % MAX_SENSORS;
    /* If no sensors are valid dont loop infinitely */
    if (index == index_copy) break;
    memcpy(&current_cfg, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    // printf("addr %d\n", current_cfg.addr);
  }
  if (current_cfg.addr == 0) {
    ((HTSigVal*)arg.sival_ptr)->busy = 0;
    return;
  }
  
  pid_t pid;
  /* Set up sensor if it is not set up yet */
  if (current_cfg.valid == 0) {
    current_cfg.command = HTC_SETUP;
    memcpy((SensorConfig *)shm_top + index, &current_cfg, sizeof(SensorConfig));
    // pid = fork();
    // if (pid == 0) {
    //   // child
    //   char str[3]; // 99 sensors
    //   sprintf(str, "%d", current_cfg.index);
    //   if (execl(HT_DRIVER_PROCESS, HT_DRIVER_PROCESS, str, (char*)0) == -1) {
    //     printf("Failed to spwan child %d\n", errno);
    //   }
    // } else {
    //   // parent
    while (current_cfg.command != HTC_WAIT) {
      memcpy(&current_cfg, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    }
    // }
  }
  if (current_cfg.fd == 0) {
    printf("Failed to open sensor fd %d\n", current_cfg.index);
    return;
  }
  
  /* Continue processing sensor read */
  if (current_cfg.command == HTC_WAIT && current_cfg.sensor_type != Mag) {
    current_cfg.command = HTC_READ;
    memcpy((SensorConfig *)shm_top + index, &current_cfg, sizeof(SensorConfig));
    while (current_cfg.command != HTC_WAIT && current_cfg.command != 0) {
      memcpy(&current_cfg, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    }
  }
  /* Increase read index */
  ((HTSigVal*)arg.sival_ptr)->index = (index + 1) % MAX_SENSORS;
  ((HTSigVal*)arg.sival_ptr)->busy = 0;
}

int main (int argc, char **argv) {

  /* Set up electromagnet pin for output */
  if (wiringPiSetup () == -1)
    return -1;
  pinMode (E_MAG_PIN, OUTPUT);

  /* Open shared memory and get variables ready */
  int shm_fd;
  const int shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  const char *shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);

  /* Create structure for timer event arg */
  HTSigVal event_arg;
  event_arg.index = 0;
  event_arg.shm_top = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  
  if (event_arg.shm_top == MAP_FAILED) {
    printf("shm_top failed\n");
    return -1;
  }

  /* Create default sensor config and read structs */
  for (int i = 0; i < MAX_SENSORS; i++) {
    SensorConfig default_config;
    memset(&default_config, 0, sizeof(SensorConfig));
    default_config.index = i;
    memcpy((SensorConfig *)event_arg.shm_top + i, &default_config, sizeof(SensorConfig));
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

  struct sigevent timer_fired_mag;
  memset(&timer_fired_mag, 0, sizeof(struct sigevent));
  timer_fired_mag.sigev_notify = SIGEV_THREAD;
  timer_fired_mag.sigev_notify_function = handle_timer_mag;
  timer_fired_mag.sigev_notify_attributes = NULL;
  timer_fired_mag.sigev_value.sival_ptr = &event_arg;

  /* Define timer interval */
  struct itimerspec timer_interval_mag;
  memset(&timer_interval_mag, 0, sizeof(struct itimerspec));
  timer_interval_mag.it_interval.tv_sec = MAG_TRIGGER_INT;
  timer_interval_mag.it_interval.tv_nsec = 0;
  timer_interval_mag.it_value.tv_nsec = 1;
  timer_interval_mag.it_value.tv_sec = 0;

  timer_t timer_id_mag;
  if (timer_create(CLOCK_REALTIME, &timer_fired_mag, &timer_id_mag)) {
    return errno;
  }
  if (timer_settime(timer_id_mag, 0, &timer_interval_mag, NULL)) {
    return errno;
  }
  /* Max sensors 18 if two childs specified */
  for (int i = 0; i < MAX_CHILD_DRIVERS; i++) {
    int pid = fork();
    if (pid == 0) {
      // child
      char str[1]; // 9 sensors
      sprintf(str, "%d", i * MAX_SENSORS / 2);
      if (execl(HT_DRIVER_PROCESS, HT_DRIVER_PROCESS, str, (char*)0) == -1) {
        printf("Failed to spwan child %d\n", errno);
        return -1;
      }
    }
  }
  /* Loop forever */
  while(1){
    usleep(10);
  }

  // Probably should capture sigterm unlink this
  shm_unlink(shm_name);
  return 0;
}