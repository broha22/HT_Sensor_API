#ifndef HT_RS
#define HT_RS

// #define READ_INTERVAL 1000000 // 1mS
#define READ_INTERVAL 500000000 // 500mS
#define E_MAG_PIN 11

typedef struct HTSigVal {
	unsigned int index;
  void *config_top;
  void *read_top;
} HTSigVal;

#endif