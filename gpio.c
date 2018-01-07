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


#include "gpio.h"
#include "time.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>



/******************************
 *  global variables
 */
t_gpio_pin gpio_pins[8];
unsigned long int count_global = 0;
unsigned char changed_global = 0;



int gpio_config_read (char* filename) {
  FILE *ptr_file = 0;

  ptr_file = fopen(filename, "r");
  if (! ptr_file)
    return 1;

  unsigned int gpio = 0;
  unsigned int pin = 0;
  unsigned int pulses_per_kwh = 0;

  int return_value;
  while((return_value = fscanf(ptr_file, "%d,%d,%d\n", &gpio, &pin, &pulses_per_kwh)) != EOF) {
    #ifdef DEBUGGING
    printf("%d / ", return_value);
    #endif
    if (gpio < 8) {
      #ifdef DEBUGGING
      printf("GPIO.%u is #%u with %u/kWh\n", gpio, pin, pulses_per_kwh);
      #endif
      gpio_pins[gpio].wiringPi_number = pin;
      gpio_pins[gpio].pulses_per_kwh = pulses_per_kwh;
    }
  }
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
    if (gpio < 8) {
      #ifdef DEBUGGING
      printf("GPIO.%d last values: C=%lu T=%lf P=%f\n", gpio, counter, time_last, period_last);
      #endif
      gpio_pins[gpio].counter = counter;
      gpio_pins[gpio].time_last = time_last;
      gpio_pins[gpio].period_last = period_last;
    }
  }
  return 0;
}


#undef DEBUGGING

void myInterrupt(void) {
  // retrieve current time for all operations iduring this IRQ
  volatile double now = time_get_precision();

  #ifdef DEBUGGING
  printf("%f INT:", now);
  #endif

  // check which GPIO pins triggered an interrupt on rising edge
  unsigned char gpio;
  for (gpio = 0; gpio < 8 ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    int state = digitalRead(current->wiringPi_number);
    #ifdef DEBUGGING
    printf(" G.%d=%d", gpio, state); 
    #endif
    if ((state == 1) && (current->state_last == 0)) {
      // update last timestamp
      current->period_last = (now - current->time_last);
      current->time_last = now;

      // update all readings
      current->counter++;
      current->energy = 1.0 * current->counter / current->pulses_per_kwh;
      current->power_last = 3600.0 / (current->period_last * current->pulses_per_kwh);
      current->changed = 1;

      count_global++;
      changed_global = 1;

      #ifdef DEBUGGING
      printf(" (E=%f dT=%f P=%f)", current->energy, current->period_last, current->power_last*1000);
      #endif
    }
    current->state_last = state;
  }
#ifdef DEBUGGING
  printf("\n"); 
#endif
}



void gpio_config_initialize(void) {
  unsigned char gpio;
  for(gpio = 0; gpio < 8 ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    current->wiringPi_number = 255;
    current->state_last = 0;
    current->pulses_per_kwh = 0;

    current->counter = 0;
    current->energy = 0;
    current->period_last = 0;
    current->power_last = 0;
    current->changed = 0;
  }
}


int gpio_init_wiring(void) {
  if (wiringPiSetup () < 0) {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return 1;
  }

  unsigned char gpio;
  for (gpio = 0; gpio < 8 ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    unsigned int number = current->wiringPi_number;
    if (number != 255) {
      #ifdef DEBUGGING
      printf("set GPIO.%d to pin %d\n", gpio, number);
      #endif
      pinMode(number, INPUT);
      pullUpDnControl(number, PUD_DOWN);
      if (wiringPiISR (number, INT_EDGE_BOTH, &myInterrupt) < 0) {
        fprintf (stderr, "Unable to setup ISR for GPIO.%d: %s\n", gpio, strerror (errno));
        return 1;
      } else {
        current->state_last = digitalRead(number);
      }
    }
  }
  return 0;
}


t_gpio_pin * gpio_get_status(unsigned int gpio) {
  return &(gpio_pins[gpio]);
}

unsigned char gpio_get_changed_global(void) {
  return changed_global;
}

unsigned long int gpio_get_count_global(void) {
  return count_global;
}

void gpio_get_entry_string(unsigned char gpio, char * outstr, unsigned int length, unsigned char type) {
  if (gpio < 8) {
    t_gpio_pin *current = &gpio_pins[gpio];
    if (type == 0)
      snprintf(outstr, length, "%u: %lu %lf %lf", gpio, current->counter, current->time_last, current->period_last);
    else
      snprintf(outstr, length, "%u: %u %u | %lu | %lf %lf = %lf %lf",
        gpio,
	current->wiringPi_number,
	current->pulses_per_kwh,
	current->counter,
	current->time_last,
	current->period_last,
        current->energy,
        current->power_last
      );

  }
}

