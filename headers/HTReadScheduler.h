#include <wiringPi.h>
#include "../drivers/bsh/bno055_support.h"
#include "../drivers/nxp/nxp_support.h"
#include "../drivers/lsm/lsm9ds1_support.h"

#ifndef HT_RS
#define HT_RS
// #define READ_INTERVAL 10000 // 10uS
#define READ_INTERVAL 1000000 // 1mS
// #define READ_INTERVAL 600000000 // 600mS
//#define READ_INTERVAL 500000000 // 500mS
#define E_MAG_PIN 29

typedef struct HTSigVal {
	unsigned int index;
  void *shm_top;
  unsigned int busy;
} HTSigVal;

#endif