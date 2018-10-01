/*
 * c-periphery
 * https://github.com/vsergeev/c-periphery
 * License: MIT
 */

#ifndef _PERIPHERY_ADC_H
#define _PERIPHERY_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>


typedef struct adc_handle {
    unsigned int pin;
    int fd;

    struct {
        int c_errno;
        char errmsg[96];
    } error;
} adc_t;



/* Primary Functions */
int adc_open(adc_t *adc, unsigned int pin);
int adc_read(adc_t *adc, int *value);
int adc_poll(adc_t *adc, int timeout_ms);
int adc_close(adc_t *adc);

/* Miscellaneous */
unsigned int adc_pin(adc_t *adc);
int adc_fd(adc_t *adc);

#ifdef __cplusplus
}
#endif

#endif

