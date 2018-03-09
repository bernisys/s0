/*
 * s0.c:
 *
 * handles the S0 impulse output values of power meters
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


#define MAX_UDP_STRING 100
#define MAX_CLI_STRING 400

#define FILENAME "/var/run/s0.last"
#define FILENAME_INI "s0.ini"

#define GPIO_COUNT 8

unsigned int pulses[GPIO_COUNT];
unsigned char wiringPi_number[GPIO_COUNT];



void gpio_get_entry_string(unsigned char gpio, char * outstr, unsigned int length, unsigned char type);
int gpio_config_read (char* filename);
int gpio_last_values_read (char* filename);


/******************************
 * Initialization routines
 */

int init_all(void) {
  gpio_config_initialize();

  if (gpio_config_read(FILENAME_INI)) {
    printf("Something went wrong with the configuration file: \""FILENAME_INI"\"\n");
    return 1;
  }

  printf("initializing...\n");
  if (gpio_last_values_read (FILENAME))
  {
    printf("Could not read last values.\n");
  }


  if (gpio_init_wiring())
  {
    printf("Something went wrong with the port initialization\n");
    return 1;
  }

  printf("done.\n");
  return 0;
}





/******************************
 *  calculations
 */
double s0_get_energy(unsigned char gpio) {
  unsigned long int counter = gpio_pin_get_counter(gpio);
  double energy = (double)counter / pulses[gpio];
  return energy;
}

double s0_get_power(unsigned char gpio) {
  double period = gpio_pin_get_period_last(gpio);
  double power = 0;
  if (period != 0)
    power = 3600.0 / (period * pulses[gpio]);
  return power;
}



/******************************
 *  main function
 */
int main (void) {
  if (init_all())
    return 1;

  static long int count = 0;
  for (;;) {
    while (count == gpio_global_get_counter())
      delay (100);

    count = gpio_global_get_counter();
    char outstr_cli[MAX_CLI_STRING] = {0};

    FILE *fh = fopen(FILENAME".new", "w");

    double now = time_get_precision();
    snprintf(outstr_cli, MAX_CLI_STRING-1, "%0.3lf", now);

    unsigned char gpio;
    for (gpio = 0; gpio < GPIO_COUNT ; gpio++) {
      // TODO: retrieve pulses config!

      double energy = s0_get_energy(gpio);
      double power = s0_get_power(gpio);

      char tempstr[32];
      snprintf(tempstr, 30, " %9.3lfkWh %5.0lfW", energy, power * 1000);
      strncat(outstr_cli, tempstr, MAX_CLI_STRING-1);

      // write last state into file (use a ramdisk location!)
      char outstr[MAX_UDP_STRING] = {0};
      gpio_get_entry_string(gpio, outstr, MAX_UDP_STRING-1, 0);
      fprintf(fh, "%s\n", outstr);

      unsigned char changed = gpio_pin_get_changed(gpio);
      if (changed != 0) {
        changed = 0;
        double time_last = gpio_pin_get_time_last(gpio);
        snprintf(outstr, MAX_UDP_STRING-1, "%f: %d %lf %0.3lf", time_last, gpio, energy, power);
        sendBroadcastPacket("10.11.8.255", 22222, outstr);
      }
    }
    fclose(fh);
    rename(FILENAME".new", FILENAME);

    printf("%s\n", outstr_cli);
  }

  return 0;
}


int gpio_config_read (char* filename) {
  FILE *ptr_file = 0;

  ptr_file = fopen(filename, "r");
  if (! ptr_file)
    return 1;

  int return_value = 0;
  while(return_value != EOF) {
    unsigned int gpio = 0;
    unsigned int pin = 0;
    unsigned int pulses_per_kwh = 0;

    return_value = fscanf(ptr_file, "#%d,%d,%d\n", &gpio, &pin, &pulses_per_kwh);
    #ifdef DEBUGGING
    printf("%d / ", return_value);
    #endif
    if (return_value > 0) {
      if (gpio < GPIO_COUNT) {
        #ifdef DEBUGGING
        printf("GPIO.%u is #%u with %u/kWh\n", gpio, pin, pulses_per_kwh);
        #endif
        gpio_pin_set_wiring(gpio, pin);
        wiringPi_number[gpio] = pin;
        pulses[gpio] = pulses_per_kwh;
      }
    } else {
      char * line;
      line = malloc(1024);
      if (line != NULL) {
        return_value = fscanf(ptr_file, "%s\n", line);
        free(line);
      } else {
        printf("ERROR: Cannot allocate RAM!\n");
        fclose(ptr_file);
        return -1;
      }
    }
  }
  fclose(ptr_file);
  return 0;
}


int gpio_last_values_read (char* filename) {
  FILE *ptr_file = 0;

  ptr_file = fopen(filename, "r");
  if (! ptr_file)
    return 1;

  unsigned int gpio = 0;
  unsigned long int counter;
  double time_last;
  double period_last;

  int return_value;
  while((return_value = fscanf(ptr_file, "%u: %lu %lf %lf", &gpio, &counter, &time_last, &period_last)) != EOF) {
    #ifdef DEBUGGING
    printf("%d / ", return_value);
    #endif
    if (gpio < GPIO_COUNT) {
      #ifdef DEBUGGING
      printf("GPIO.%d last values: C=%lu T=%lf P=%f\n", gpio, counter, time_last, period_last);
      #endif
      gpio_pin_set_values(gpio, counter, time_last, period_last);
    }
  }
  return 0;
}


void gpio_get_entry_string(unsigned char gpio, char * outstr, unsigned int length, unsigned char type) {
  if (gpio < GPIO_COUNT) {
    unsigned long int counter = gpio_pin_get_counter(gpio);
    double time_last = gpio_pin_get_time_last(gpio);
    double period_last = gpio_pin_get_period_last(gpio);

    if (type == 0)
      snprintf(outstr, length, "%u: %lu %lf %lf", gpio, counter, time_last, period_last);
    else {
      double energy = s0_get_energy(gpio);
      double power_last = s0_get_power(gpio);
      snprintf(outstr, length, "%u: %u %u | %lu | %lf %lf = %lf %lf", gpio, wiringPi_number[gpio], pulses[gpio], counter, time_last, period_last, energy, power_last);
    }
  }
}


