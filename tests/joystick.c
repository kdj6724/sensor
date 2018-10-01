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

struct joystickPin {
  adc_t x;   // adc
  adc_t y;    // adc
  gpio_t z;  // gpio
};

int Joystick_Init(struct joystickPin* joystick, 
    int xAxis, int yAxis, int zAxis) {

  if (adc_open(&(joystick->x), xAxis) != 0) {
    return -1;
  }
  if (adc_open(&(joystick->y), yAxis) != 0) {
    adc_close(&(joystick->x));
    return -1;
  }
  if (gpio_open(&(joystick->z), zAxis, GPIO_DIR_IN) != 0) {
    adc_close(&(joystick->x));
    adc_close(&(joystick->y));
    return -1;
  }
  gpio_set_direction(&(joystick->z), GPIO_DIR_IN);
  return 0;
}

void Joystick_Deinit(struct joystickPin* joystick) {
  adc_close(&(joystick->x));
  adc_close(&(joystick->y));
  gpio_close(&(joystick->z));
}

void joysticktest_loopback(struct joystickPin* joystick) {
  int xVal = 0;
  int yVal = 0;
  bool zVal = 0;
  while(1) {
    adc_read(&(joystick->x), &xVal);
    adc_read(&(joystick->y), &yVal);
    gpio_read(&(joystick->z), &zVal);
    printf("x:%04d, y:%04d, z:%d\n", xVal, yVal, zVal);
    usleep(500000);
  }
}

int main(int argc, char *argv[]) {
  struct joystickPin joystick;
  int xAxis;
  int yAxis;
  int zAxis;

  if (argc != 4) {
      fprintf(stderr, "Usage: %s <X pin #1> <Y pin #2> <Z pin #3>\n\n", argv[0]);
      exit(1);
  }

  xAxis = strtoul(argv[1], NULL, 10);
  yAxis = strtoul(argv[2], NULL, 10);
  zAxis = strtoul(argv[3], NULL, 10);

  if (Joystick_Init(&joystick, xAxis, yAxis, zAxis) < 0) {
    fprintf(stderr, "Joy Stick open error\n");
    return -1;
  }
  joysticktest_loopback(&joystick); 
  Joystick_Deinit(&joystick); 
  return 0;
}

