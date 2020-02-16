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

void write_config(SensorConfig config) {
  memcpy((SensorConfig *)shm_top + config.index, &config, sizeof(SensorConfig));
}

void clear_sensors() {
  SensorConfig config;
  for (int index = 0; index < MAX_SENSORS; index++) {
    memset(&config, 0, sizeof(SensorConfig));
    config.index = index;
    config.command = HTC_DELETE;
    write_config(config);
  }

}


void load_sensors(const char *configFile) {
  FILE *f_ptr;
  if ((f_ptr = fopen(configFile, "r")) == NULL) {
    printf("Failed to open file \n");
    return;
  }
  int count = 0;
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
            printf("%d %s BSH", count, token);
            config.driver_library = BSH;
          } else if (strcmp(token, "LSM") == 0) {
            printf("%d %s LSM", count, token);
            config.driver_library = LSM;
          } else if (strcmp(token, "NXP") == 0) {
            printf("%d %s NXP", count, token);
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
    config.index = count;
    printf("index: %d d: %d", config.index, config.driver_library);
    write_config(config);
    count++;
  }
  free(buffer);
}
