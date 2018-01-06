/*
 * s0.c:
 * Wait for Interrupts on all GPIOs, increase counters accodringly
 *
 * Test:
 * gpio mode 0 up ; gpio mode 0 down
 *
 * Uses wiringPi: https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 ***********************************************************************
 */


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

#include "udp_broadcast.h"
#include "gpio.h"
#include "time.h"


//#define DEBUGGING



/******************************
 * Initialization routines
 */

int init_all(void) {
  gpio_config_initialize();
  if (gpio_config_read("s0.ini")) {
    printf("Something went wrong with the configuration file: \"s0.ini\"\n");
    return 1;
  }

  printf("initializing...\n");
  if (gpio_init_wiring())
  {
    printf("Something went wrong with the port initialization\n");
    return 1;
  }

  printf("done.\n");
  return 0;
}



/******************************
 *  main function
 */
int main (void) {
  if (init_all())
    return 1;

  static long int count = 0;
  for (;;) {
    while (count == gpio_get_count_global())
      delay (100);

    count = gpio_get_count_global();

    char outstr[500] = {0};
    unsigned char gpio;

    // TESTING //
    volatile double now = time_get_precision();
    printf("now: %f\n", now);


    for (gpio = 0; gpio < 8 ; gpio++) {
      t_gpio_pin *current = gpio_get_status(gpio);
      // printf("GPIO.%d -> C=%lu  E=%e  TL=%e  dT=%e  PL=%e\n", gpio, current->counter, current->energy, current->time_last, current->period_last, current->power_last);
      char countstr[30];
      snprintf(countstr, 29, " %9.3fkWh", (float)(current->counter) / (float)(current->pulses_per_kwh));
      strcat(outstr, countstr);
      snprintf(countstr, 29, " %5luW", (unsigned long int)(current->power_last * 1000));
      strcat(outstr, countstr);
    }
    printf("%s\n", outstr);
  }

  return 0;
}


