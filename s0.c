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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

//#define DEBUGGING

typedef struct {
  // GPIO related values
  unsigned char wiringPi_number;
  unsigned char state_last;

  // power meter related values
  unsigned int pulses_per_kwh;  // translation factor pulses <-> kWh

  unsigned long int counter;
  double power_last;
  double time_last;
} t_gpio_pin;

t_gpio_pin gpio_pins[8];
unsigned long int count_clobal = 0;


double time_precision(void) {
  struct timeval tv;
  gettimeofday(&tv, 0);
  volatile double tval = tv.tv_sec + ((double)(tv.tv_usec) / 1000000.0);
  return tval;
}


void myInterrupt (void) {
  double now = time_precision();
  ++count_clobal;

#ifdef DEBUGGING
  printf("%f INT:", now);
#endif
  // check which GPIO pin triggered the interrupt (rising edge)
  unsigned char gpio;
  for (gpio = 0; gpio < 8 ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    int state = digitalRead(current->wiringPi_number);
#ifdef DEBUGGING
    printf(" G.%d=%d", gpio, state); 
#endif
    if ((state == 1) && (current->state_last == 0)) {
      current->counter++;
      double energy = 1.0 * current->counter / current->pulses_per_kwh;
      double delta_time = (now - current->time_last);
      current->power_last = 3600.0 / (delta_time * current->pulses_per_kwh);
#ifdef DEBUGGING
      printf("  E=%f  dT=%f  P=%f\n", energy, delta_time, current->power_last*1000);
#endif
      current->time_last = now;
    }
    else if (state == current->state_last) {
      double delta_time = (now - current->time_last);
      if (delta_time > 0.5)
        current->power_last = 3600.0 / (delta_time * current->pulses_per_kwh);
    }
    current->state_last = state;
  }
#ifdef DEBUGGING
  printf("\n"); 
#endif
}


int gpio_config_read (char* filename) {
  FILE *ptr_file = 0;

  ptr_file = fopen(filename, "r");
  if (! ptr_file)
    return 1;

  unsigned int gpio = 0;
  unsigned int pin = 0;
  unsigned int pulses_per_kwh = 0;

  int return_value;
  while(return_value = fscanf(ptr_file, "%d,%d,%d\n", &gpio, &pin, &pulses_per_kwh) != EOF) {
#ifdef DEBUGGING
    printf("%d / ", return_value);
#endif
    if (gpio < 8) {
#ifdef DEBUGGING
      printf("GPIO.%d is #%d with %d/kWh\n", gpio, pin, pulses_per_kwh);
#endif
      gpio_pins[gpio].wiringPi_number = pin;
      gpio_pins[gpio].pulses_per_kwh = pulses_per_kwh;
    }
  }
  return 0;
}


int init (void) {
  if (wiringPiSetup () < 0) {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return 1;
  }

  char gpio;
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


void gpio_config_initialize(void) {
  unsigned char gpio;
  for(gpio = 0; gpio < 8 ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    current->wiringPi_number = 255;
    current->state_last = 0;
    current->pulses_per_kwh = 0;

    current->counter = 0;
    current->power_last = 0;
    current->time_last = 0;
  }
}


int main (void)
{
  gpio_config_initialize();
  if (gpio_config_read("s0.ini")) {
    printf("Something went wrong with the configuration file: \"s0.ini\"\n");
    return 1;
  }

  printf("initializing...\n");
  if (init())
  {
    printf("Something went wrong with the port initialization\n");
    return 1;
  }

  printf("done.\n");

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

