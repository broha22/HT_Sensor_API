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
#include <string.h>

#ifndef HT_SENSORS
#define HT_SENSORS 
/* Name used for shared memory space */
#define SHM_NAME "HT_SENSORS_SHM"
#define MAX_SENSORS 18

#include <time.h>

/*
 *  Specifies the driver library to be used,
 *  important to signifiy the HW model of sensor.
 *  (Bosch, LSM, NXP)
 */

typedef enum DriverLibrary {
	BSH = 3,
	LSM = 1,
	NXP = 2
} DriverLibrary;

/*
 *  Specifies the type of sensor to be:
 *  Gyroscoepe, Magnetometer or Accelerometer
 */

typedef enum SensorType {
	Gyr = 3,
	Acc = 1,
	Mag = 2
} SensorType;

typedef enum HTCommand {
	HTC_WAIT = 4,
  HTC_READ = 1,
	HTC_SETUP = 2,
	HTC_DELETE = 3
} HTCommand;

/* 
 *  Object: SensorRead
 *  
 *  Params:
 *    - x: x axis value of hw sensor read
 *    - y: y axis value of hw sensor read
 *    - z: z axis value of hw sensor read
 *    - time: epoch time (ms since epoch)
 *    - sensor: reference to sensor config that
 *    		has been read
 *
 *  Description:
 *    This object contains information regarding the reading of a specific
 *    hw sensor.
 */

typedef struct SensorRead {
	double x;
	double y;
	double z;
	struct timespec time;
} SensorRead;

/*
 *  Object: SensorConfig
 *  
 *  Params:
 *    - id: unique number of sensor
 *    - fd: file descriptor 
 *    - addr: I2C adress
 *    - sensor_type: specifies acc, gyr or mag type
 *    - driver_library: sensor manufacturer, for driver support
 *    - x_offset: degree tilt on x axis of hw sensor (+90, -90, 0, +180)
 *    - y_offset: degree tilt on y axis of hw sensor (+90, -90, 0, +180)
 *    - z_offset: degree tilt on z axis of hw sensor (+90, -90, 0, +180)
 *
 *  Description:
 *    Specifies HW config of sensor in SW.
 */

typedef struct SensorConfig {
	HTCommand command;
	unsigned int valid;
	unsigned int index;
	unsigned int addr;
	int fd;
	SensorType sensor_type;
	DriverLibrary driver_library;
	int x_offset;
	int y_offset;
	int z_offset;
	SensorRead last_read;
} SensorConfig;

/*
 *  Function: read_sensors
 *  
 *  Params:
 *    - sensor_count: int pointer that will be updated to the number of
 *                    sensors currently configured in the HW setup
 *    - sensor_readings: SensorRead array pointer populated to size
 *                    sensor_count of most recent SensorRead for each
 *                    HW sensor
 *
 *  Description:
 *    Takes in two pointer references and assigns them accordingly. Read
 *    parameter descriptions for each to see how the references are
 *    populated.
 *
 */
SensorConfig* read_sensors(int *count);
void clear_sensors();
void load_sensors(const char* configFile);
void init_ht_api();
void dealloc_ht_api();
#endif
