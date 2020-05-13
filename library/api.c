/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTSensors.h"
#include "../headers/HTReadScheduler.h"

/*
  Initialize global variables

  shm_name stores string representation of shared mem space
  shm_top stores pointer to top of HTSensor shared mem space
  shm_size stores the size in bytes of the HTSensors shared mem space
*/
char *shm_name;
void *shm_top;
int shm_size;


//this function is used to turn off/on gpio connecting
//the emag relay
void toggle_e_magnet(int on) {
  if (on == 1) {
    digitalWrite(E_MAG_PIN, 1);
  } else {
    digitalWrite(E_MAG_PIN, 0);
  }
}

//this function serves an input for the emag read timer
void handle_timer_mag (union sigval arg) {
  void *shm_top = ((HTSigVal*)arg.sival_ptr)->shm_top;
  //make sure another timer event isnt running
  while (((HTSigVal*)arg.sival_ptr)->busy == 1) { usleep(10); }
  ((HTSigVal*)arg.sival_ptr)->busy = 1; //set this event loop to process
  SensorConfig current_cfg;

  //turn on the emag
  toggle_e_magnet(1); 
  usleep(500); //add extra time

  //do a read update for each magnetic sensor connected
  for (int i = 0; i < MAX_SENSORS; i++) {
    memcpy(&current_cfg, (SensorConfig *)shm_top + i, sizeof(SensorConfig));
    if (current_cfg.sensor_type == Mag && current_cfg.fd != 0) { //check for valid mag sensors
      
      //send the read command and wait for completition
      current_cfg.command = HTC_READ;
      memcpy((SensorConfig *)shm_top + i, &current_cfg, sizeof(SensorConfig));
      while (current_cfg.command != HTC_WAIT) {
        memcpy(&current_cfg, (SensorConfig *)shm_top + i, sizeof(SensorConfig));
      }
    }
  }
 usleep(500); //add extra time
  toggle_e_magnet(0);
  ((HTSigVal*)arg.sival_ptr)->busy = 0; //release event lock loop
}

//handle the event timer for non magnetic sensors
void handle_timer (union sigval arg) {
  if (((HTSigVal*)arg.sival_ptr)->busy == 1) {
    return;
  }
  ((HTSigVal*)arg.sival_ptr)->busy = 1; //lock to this timer
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
    while (current_cfg.command != HTC_WAIT) {
      memcpy(&current_cfg, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    }
  }
  if (current_cfg.fd == 0) {
    printf("Failed to open sensor fd %d\n", current_cfg.index);
    return;
  }
  
  /* Continue processing sensor read */
  //do not read mag sensors
  //make sure sensor is not busy
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

/*
  init_ht_api

  properly sets the global variables for the library
*/
int init_ht_api() {
  int shm_fd;
  shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  shm_name = SHM_NAME;

  //Create or open the shared memory space, truncate to size needed
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);

  //map pointer to shared memory location
  shm_top = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

  /* Set up electromagnet pin for output */
  if (wiringPiSetup () == -1)
    return -1;
  pinMode(E_MAG_PIN, OUTPUT);

  HTSigVal event_arg;
  event_arg.index = 0;
  event_arg.shm_top = shm_top;

  /* Create default sensor config and read structs */
  for (int i = 0; i < MAX_SENSORS; i++) {
    SensorConfig default_config;
    memset(&default_config, 0, sizeof(SensorConfig));
    default_config.index = i;
    memcpy((SensorConfig *)event_arg.shm_top + i, &default_config, sizeof(SensorConfig));
  }

  int pid = fork(); //Need to make this a child process;
  if (pid == 0) {
    if (event_arg.shm_top == MAP_FAILED) {
      printf("shm_top failed\n");
      return -1;
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

    /* Define magnet sensor timer interval */
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
    while (1) {
      usleep(100);
    }
  }
}

/*
  dealloc_ht_api

  unlinks the shared memory space
*/
void dealloc_ht_api() {
  shm_unlink(shm_name);
}

/*
  read_sensors

  reads the sensors, returns a pointer to an array of SensorConfigs
  allocated on the heap, input argument is set to the # of 
  sensors read
*/
SensorConfig* read_sensors(int *count) {
  int index = 0;
  SensorConfig config;
  memcpy(&config, (SensorConfig *)shm_top, sizeof(SensorConfig));
  while (config.addr != 0 && index < MAX_SENSORS) {
    index++;
    memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
  }
  *count = index;
  SensorConfig *r = malloc(sizeof(SensorConfig) * *count);
  memcpy(r, (SensorConfig *)shm_top, sizeof(SensorConfig) * *count);
  return r;
}

/*
  check_index

  used to properly split sensors into two unique spaces in
  shared memory space. A proper index for sensor location in
  the shared memory array is returned

  Heres more information on why this is a thing:

  Each driver library at most can support one breakout
  board but our system is designed to handle up to two.
  In cases where we are using two breakout boards it is
  necessarry to run two seperate processes each containing
  all the sensor libraries. This mitigates the issues of
  global variables being overwritten.

  In the future these libraries can be better written to not
  rely on the global variables.
*/
int check_index(SensorConfig config) {
  SensorConfig c_config;
  int open_index = -1; //returns -1 for index not found satisfying constraint
  //This operation must check every sensor spot to see if
  //a conflicting sensor exists in a portion
  for (int i = 0; i < MAX_SENSORS; i++) {
    memcpy(&c_config, (SensorConfig *)shm_top + i, sizeof(SensorConfig));
    /*
      Next 50 lines are a mess and super confusing so
      here is the basic principle on how this works...

      The shared memory space is divided into two processes
      each covering 9 sensorconfigs. 0-8 cover one process and
      9-17 cover the other. It is possible to present a config
      that if written in sequential order to shared mem would result in
      a sensor library issue. So these functions do the reordering for
      the user. 

      If a possible collision is detected the algorithm sets the looping index to
      start at the next process (unless its already at the last process)

      possible collisions are as follows:
      - For Bosch:
          * A Bosch system is found in the space with a diff
            i2c address then the one we are trying to add
      - For LSM:
          * A LSM Gyro or Accel. system is found in the space with a diff
            i2c address then the one we are trying to add
          * A LSM Mag system is found in the space with a diff
            i2c address then the one we are trying to add
      - For NXP:
          * A NXP Mag or Accel. system is found in the space with a diff
            i2c address then the one we are trying to add
          * A LSM Gyro system is found in the space with a diff
            i2c address then the one we are trying to add

      * In simple terms:
          - a bosch boards need to be in seperate spaces
          - a LSM Gyro and Accelerometer combination needs to be in seperate space
          - a LSM Mag needs to be in seperate space
          - a NXP Mag and Accelerometer combination needs to be in seperate space
          - a NXP Gyro needs to be in seperate space

    */
    if (c_config.addr) {
      if (c_config.driver_library == config.driver_library) {
        if (c_config.driver_library == BSH) {
          if (c_config.addr != config.addr) {
            if (i < MAX_SENSORS / 2) {
              // -1 because the i will increment by 1 at the end of the loop
              i = MAX_SENSORS / 2 - 1;
            } else {
              // we looped through both processes spaces and found a collision
              // something is wrong with the config and we should stop
              break;
            }
          }
        } else if (c_config.driver_library == LSM) {
          if ((c_config.sensor_type == Acc || c_config.sensor_type == Gyr) && (config.sensor_type == Acc || config.sensor_type == Gyr)) {
            if (c_config.addr != config.addr) {
              if (i < MAX_SENSORS / 2) {
                i = MAX_SENSORS / 2 - 1;
              } else {
                break;
              }
            }
          } else if (c_config.sensor_type == Mag && config.sensor_type == Mag) {
            if (c_config.addr != config.addr) {
              if (i < MAX_SENSORS / 2) {
                i = MAX_SENSORS / 2 - 1;
              } else {
                break;
              }
            }
          }
        } else if (c_config.driver_library == NXP) {
          if ((c_config.sensor_type == Mag || c_config.sensor_type == Acc) && (config.sensor_type == Mag || config.sensor_type == Acc)) {
            if (c_config.addr != config.addr) {
              if (i < MAX_SENSORS / 2) {
                i = MAX_SENSORS / 2 - 1;
              } else {
                break;
              }
            }
          } else if (c_config.sensor_type == Gyr && config.sensor_type == Gyr) {
            if (c_config.addr != config.addr) {
              if (i < MAX_SENSORS / 2) {
               i = MAX_SENSORS / 2 - 1;
              } else {
               break;
              }
            }
          }
        }
      }
    } else {
      // we didnt find any collisions so we can put the sensor here
      open_index = i;
    }
    if (open_index >= 0) {
      break;
    }
  }
  return open_index;
}

/*
  write_config

  helper function to write the config struct into shared memory
*/
void write_config(SensorConfig config) {
  //Load config currently stored
  SensorConfig c_config;
  memcpy(&c_config, (SensorConfig *)shm_top + config.index, sizeof(SensorConfig));
  
  //Wait unit the current sensor is not busy doing a read or setup
  while (c_config.command != 0 && c_config.command != HTC_WAIT) {
    memcpy(&c_config, (SensorConfig *)shm_top + config.index, sizeof(SensorConfig));
  }

  //write the new config information
  memcpy((SensorConfig *)shm_top + config.index, &config, sizeof(SensorConfig));
}

/*
  clear_sensors

  function properly tears down sensors and empties their contents
*/
void clear_sensors() {
  SensorConfig config;
  for (int index = 0; index < MAX_SENSORS; index++) {
    
    //copy current config of each sensor
    memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    
    if (config.valid == 1) { //check if it has been initialized
      
      //send the delete command to the sensor
      config.command = HTC_DELETE;
      write_config(config);
      
      //wait for the sensor to tear its self down
      while (config.command != 0) {
        memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
      }
    }
  }
}

/*
  load_sensors

  parses a config file and stores that information
  into the sensor shared memory space
*/
void load_sensors(const char *configFile) {

  //open the file passed in
  FILE *f_ptr;
  if ((f_ptr = fopen(configFile, "r")) == NULL) {
    printf("Failed to open file \n");
    return;
  }

  char *buffer = NULL;
  size_t len = 0;

  //read the file line by line
  while (getline(&buffer, &len, f_ptr) != -1) {
    char *token;
    buffer[strlen(buffer) - 1] = '\0';
    
    //seperate each line by commas
    token = strtok(buffer, ",");
    SensorConfig config;
    memset(&config, 0, sizeof(SensorConfig));
    int count_2 = 0; //column index
    while (token != NULL) {
      unsigned int addr;
      int x,y,z;
      
      //switch through each column
      switch (count_2) {
        case 0: //first column sets library
          if (strcmp(token, "BSH") == 0) {
            config.driver_library = BSH;
          } else if (strcmp(token, "LSM") == 0) {
            config.driver_library = LSM;
          } else if (strcmp(token, "NXP") == 0) {
            config.driver_library = NXP;
          }
          break;
        case 1: //second column sets type
          if (strcmp(token, "Gyr") == 0) {
            config.sensor_type = Gyr;
          } else if (strcmp(token, "Acc") == 0) {
            config.sensor_type = Acc;
          } else if (strcmp(token, "Mag") == 0) {
            config.sensor_type = Mag;
          }
          break;
        case 2: //third column sets i2c address
          sscanf(token, "%x", &addr);
          config.addr = addr;
          break;
        case 3: //fourth coulmn sets x axis degree offset
          sscanf(token, "%d", &x);
          config.x_offset = x;
          break;
        case 4: //fifth coulmn sets x axis degree offset
          sscanf(token, "%d", &y);
          config.y_offset = y;
          break;
        case 5: //sixth coulmn sets x axis degree offset
          sscanf(token, "%d", &z);
          config.z_offset = z;
          break;
        default: //too many columns
          printf("Invalid Config");
          break;
      }
      count_2++;
      token = strtok(NULL, ",");
    }
    //too little columns (there is a bug here and you need a space at the end of the last line in a config)
    if (count_2 != 6) {
      printf("Invalid config file \n");
    }

    //get non colliding index
    int index = check_index(config);
    if (index == -1) {
      printf("An Index wasnt found, clear sensors before loading a new config");
    }

    //set the found index and write the config
    config.index = index;
    write_config(config);
  }
  free(buffer);
}
