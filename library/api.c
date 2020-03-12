/*
 * @Author: you@you.you
 * @Date:   Saturday February 15th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Saturday February 15th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
#include "../headers/HTSensors.h"
char *shm_name;
void *shm_top;
int shm_size;

void init_ht_api() {
  int shm_fd;
  shm_size = (sizeof(SensorConfig) * MAX_SENSORS);
  shm_name = SHM_NAME;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
  ftruncate(shm_fd, shm_size);
  shm_top = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

void dealloc_ht_api() {
  shm_unlink(shm_name);
}

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

int check_index(SensorConfig config) {
  SensorConfig c_config;
  int open_index = -1;
  for (int i = 0; i < MAX_SENSORS; i++) {
    memcpy(&c_config, (SensorConfig *)shm_top + i, sizeof(SensorConfig));
    if (c_config.addr) {
      if (c_config.driver_library == config.driver_library) {
        if (c_config.driver_library == BSH) {
          if (c_config.addr != config.addr) {
            if (i < MAX_SENSORS / 2) {
              i = MAX_SENSORS / 2 - 1;
            } else {
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
      open_index = i;
    }
    if (open_index >= 0) {
      break;
    }
  }
  return open_index;
}

void write_config(SensorConfig config) {
  SensorConfig c_config;
  memcpy(&c_config, (SensorConfig *)shm_top + config.index, sizeof(SensorConfig));
  while (c_config.command != 0 && c_config.command != HTC_WAIT) {
    memcpy(&c_config, (SensorConfig *)shm_top + config.index, sizeof(SensorConfig));
  }
  memcpy((SensorConfig *)shm_top + config.index, &config, sizeof(SensorConfig));
}

void clear_sensors() {
  SensorConfig config;
  for (int index = 0; index < MAX_SENSORS; index++) {
    memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
    if (config.valid == 1) {
      config.command = HTC_DELETE;
      write_config(config);
      while (config.command != 0) {
        memcpy(&config, (SensorConfig *)shm_top + index, sizeof(SensorConfig));
      }
    }
  }
}


void load_sensors(const char *configFile) {
  FILE *f_ptr;
  if ((f_ptr = fopen(configFile, "r")) == NULL) {
    printf("Failed to open file \n");
    return;
  }
  char *buffer = NULL;
  size_t len = 0;
  while (getline(&buffer, &len, f_ptr) != -1) {
    char *token;
    buffer[strlen(buffer) - 1] = '\0';
    token = strtok(buffer, ",");
    SensorConfig config;
    memset(&config, 0, sizeof(SensorConfig));
    int count_2 = 0;
    while (token != NULL) {
      unsigned int addr;
      int x,y,z;
      switch (count_2) {
        case 0:
          if (strcmp(token, "BSH") == 0) {
            config.driver_library = BSH;
          } else if (strcmp(token, "LSM") == 0) {
            config.driver_library = LSM;
          } else if (strcmp(token, "NXP") == 0) {
            config.driver_library = NXP;
          }
          break;
        case 1:
          if (strcmp(token, "Gyr") == 0) {
            config.sensor_type = Gyr;
          } else if (strcmp(token, "Acc") == 0) {
            config.sensor_type = Acc;
          } else if (strcmp(token, "Mag") == 0) {
            config.sensor_type = Mag;
          }
          break;
        case 2:
          sscanf(token, "%x", &addr);
          config.addr = addr;
          break;
        case 3:
          sscanf(token, "%d", &x);
          config.x_offset = x;
          break;
        case 4:
          sscanf(token, "%d", &y);
          config.y_offset = y;
          break;
        case 5:
          sscanf(token, "%d", &z);
          config.z_offset = z;
          break;
        default:
          printf("Invalid Config");
          break;
      }
      count_2++;
      token = strtok(NULL, ",");
    }
    if (count_2 != 6) {
      printf("Invalid config file \n");
    }
    int index = check_index(config);
    if (index == -1) {
      printf("An Index wsnt found, clear sensors before loading a new config");
    }
    config.index = index;
    write_config(config);
  }
  free(buffer);
}
