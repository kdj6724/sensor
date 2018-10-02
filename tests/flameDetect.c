/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#include "test.h"

#include <stdlib.h>
#include <errno.h>

#include <sys/wait.h>
#include <unistd.h>

#include "../src/gpio.h"
#include "../src/adc.h"

struct flameDetectPin {
  gpio_t dIn;  // digital input
  adc_t aIn;  // analog input
};

int flamedetect_init(struct flameDetectPin* pins, 
    int digital, int analog) {

  if (gpio_open(&(pins->dIn), digital, GPIO_DIR_IN) != 0) {
    return -1;
  }
  if (adc_open(&(pins->aIn), analog) != 0) {
    gpio_close(&(pins->dIn));
    return -1;
  }
  return 0;
}

void flamedetect_deinit(struct flameDetectPin* pins) {
  gpio_close(&(pins->dIn));
  adc_close(&(pins->aIn));
}

void flamedetect_loop(struct flameDetectPin* pins) {
  bool dVal = 0;
  int aVal = 0;
  while(1) {
    gpio_read(&(pins->dIn), &dVal);
    adc_read(&(pins->aIn), &aVal);
    fprintf(stdout, "%d(%d)\n", aVal, dVal);
    usleep(500000);
  }
}

int main(int argc, char *argv[]) {
  struct flameDetectPin flamedetect;
  int gpio;
  int adc;

  if (argc != 3) {
      fprintf(stderr, "Usage: %s <digital input pin #1> <analog out pin #2> \n\n", argv[0]);
      exit(1);
  }

  gpio = strtoul(argv[1], NULL, 10);
  adc = strtoul(argv[2], NULL, 10);

  if (flamedetect_init(&flamedetect, gpio, adc) < 0) {
    fprintf(stderr, "Joy Stick open error\n");
    return -1;
  }
  flamedetect_loop(&flamedetect); 
  flamedetect_deinit(&flamedetect); 
  return 0;
}

