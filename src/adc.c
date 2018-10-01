/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <errno.h>

#include "adc.h"

#define P_PATH_MAX  256

int adc_open(adc_t *adc, unsigned int pin) {
    char adc_path[P_PATH_MAX];

    /* Open value */
    snprintf(adc_path, sizeof(adc_path), "/sys/devices/platform/c0000000.soc/c0053000.adc/iio:device0/in_voltage%d_raw", pin);
    if ((adc->fd = open(adc_path, O_RDONLY)) < 0) {
      fprintf(stderr, "adc open error");
      return -1;
    }

    return 0;
}

int adc_close(adc_t *adc) {
    if (adc->fd < 0)
        return 0;

    /* Close fd */
    if (close(adc->fd) < 0) {
      fprintf(stderr, "adc close error");
      return -1;
    }
    adc->fd = -1;

    return 0;
}

int adc_read(adc_t *adc, int *value) {
    char buf[8];

    memset(buf, 0, sizeof(buf));
    /* Read fd */
    if (read(adc->fd, buf, sizeof(buf)) < 0) {
      fprintf(stderr, "adc read error");
      return -1;
    }

    /* Rewind */
    if (lseek(adc->fd, 0, SEEK_SET) < 0) {
      fprintf(stderr, "adc seek error");
      return -1;
    }
    *value = atoi(buf);
    return 0;
}

int adc_poll(adc_t *adc, int timeout_ms) {
    struct pollfd fds[1];
    char buf[1];
    int ret;

    /* Dummy read before poll */
    if (read(adc->fd, buf, 1) < 0) {
        fprintf(stderr, "Reading GPIO 'value'");
        return -1;
    }

    /* Seek to end */
    if (lseek(adc->fd, 0, SEEK_END) < 0) {
        fprintf(stderr, "Seeking to end of GPIO 'value'");
        return -1;
    }

    /* Poll */
    fds[0].fd = adc->fd;
    fds[0].events = POLLPRI | POLLERR;
    if ((ret = poll(fds, 1, timeout_ms)) < 0) {
      fprintf(stderr, "Polling GPIO 'value'");
      return -1;
    }

    /* GPIO edge interrupt occurred */
    if (ret) {
        /* Rewind */
        if (lseek(adc->fd, 0, SEEK_SET) < 0) {
          fprintf(stderr, "Rewinding GPIO 'value'");
          return -1;
        }
        return 1;
    }
    /* Timed out */
    return 0;
}

unsigned int adc_pin(adc_t *adc) {
    return adc->pin;
}

int adc_fd(adc_t *adc) {
    return adc->fd;
}
