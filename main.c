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


#include "usb_broadcast"
#include "s0"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

//#define DEBUGGING


/******************************
 *  main function
 */
int main (void) {
  if (init_all())
    return 1;

  static long int count = 0;
  for (;;) {
    while (count == count_clobal)
      delay (100);

    count = count_clobal;

    char outstr[500] = {0};
    char gpio;
    for (gpio = 0; gpio < 8 ; gpio++) {
      t_gpio_pin *current = &gpio_pins[gpio];
      char countstr[30];
      snprintf(countstr, 29, " %9.3fkWh", (float)(current->counter) / (float)(current->pulses_per_kwh));
      strcat(outstr, countstr);
      snprintf(countstr, 29, " %5uW", (unsigned long int)(current->power_last * 1000));
      strcat(outstr, countstr);
    }
    printf("%s\n", outstr);
  }

  return 0;
}



