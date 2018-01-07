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


void output_all(void);


#define MAX_UDP_STRING 100
#define MAX_CLI_STRING 400


#define FILENAME "/var/run/s0.last"



/******************************
 * Initialization routines
 */

int init_all(void) {
  gpio_config_initialize();

  if (gpio_config_read("s0.ini")) {
    printf("Something went wrong with the configuration file: \"s0.ini\"\n");
    return 1;
  }
  #ifdef DEBUGGING
  output_all();
  #endif

  printf("initializing...\n");
  if (gpio_last_values_read (FILENAME))
  {
    printf("Could not read last values.\n");
  }
  #ifdef DEBUGGING
  output_all();
  #endif


  if (gpio_init_wiring())
  {
    printf("Something went wrong with the port initialization\n");
    return 1;
  }
  #ifdef DEBUGGING
  output_all();
  #endif


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
    char outstr_udp[MAX_UDP_STRING] = {0};
    char outstr_cli[MAX_CLI_STRING] = {0};

    FILE *fh = fopen(FILENAME".new", "w");

    double now = time_get_precision();
    printf("%0.3lf", now);
    unsigned char gpio;
    for (gpio = 0; gpio < 8 ; gpio++) {
      t_gpio_pin *current = gpio_get_status(gpio);
      printf(" %9.3fkWh %5luW", (float)(current->counter) / (float)(current->pulses_per_kwh), (unsigned long int)(current->power_last * 1000));

      // write last state into file (use a ramdisk location!)
      gpio_get_entry_string(gpio, outstr_udp, MAX_UDP_STRING-1, 0);
      fprintf(fh, "%s\n", outstr_udp);

      if (current->changed != 0) {
        current->changed = 0;
        snprintf(outstr_udp, MAX_UDP_STRING-1, "%f: %d %lf %0.3lf", current->time_last, gpio, current->energy, current->power_last);
        sendBroadcastPacket("10.11.8.255", 22222, outstr_udp);
      }
    }
    printf("\n");
    fclose(fh);
    rename(FILENAME".new", FILENAME);
  }

  return 0;
}


void output_all(void) {
  printf("all values:\n");
  unsigned char gpio;
  for (gpio = 0; gpio < 8 ; gpio++) {
    char outstr_cli[MAX_CLI_STRING];
    gpio_get_entry_string(gpio, outstr_cli, MAX_CLI_STRING-1, 1);
    printf("%s\n", outstr_cli);
  }
}

