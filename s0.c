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
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>

// #define DEBUGGING

typedef struct {
  unsigned char wiringPi_number;
  unsigned char state_last;
  unsigned long int counter;
  unsigned int pulses_per_kwh;
  unsigned long long time_last;
} t_gpio_pin;

t_gpio_pin gpio_pins[8];
unsigned long int count_clobal = 0;



void myInterrupt (void) {
  ++count_clobal;

#ifdef DEBUGGING
  printf("INT:");
#endif
  // check which GPIO pin triggered the interrupt (rising edge)
  unsigned char gpio;
  for (gpio = 0; gpio < 8 ; gpio++) {
    int state = digitalRead(gpio_pins[gpio].wiringPi_number);

#ifdef DEBUGGING
    printf(" %d", state); 
#endif
    if ((state == 1) && (gpio_pins[gpio].state_last == 0)) {
      gpio_pins[gpio].counter++;
    }
    gpio_pins[gpio].state_last = state;
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

  unsigned char gpio;
  unsigned char pin;
  unsigned int pulses_per_kwh;

  int return_value;
  while(return_value = fscanf(ptr_file, "%d,%d,%d\n", &gpio, &pin, &pulses_per_kwh) != EOF) {
    if (gpio < 8) {
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
    t_gpio_pin *current;
    current = &gpio_pins[gpio];
    unsigned int number = current->wiringPi_number;
    if (number != 255) {
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
    t_gpio_pin *current;
    current = &gpio_pins[gpio];
    current->wiringPi_number = 255;
    current->state_last = 0;
    current->counter = 0;
    current->pulses_per_kwh = 0;
    current->time_last = time(NULL);
  }
}


int main (void)
{
  gpio_config_initialize();
  /* TODO: unfuck this function: */
  if (gpio_config_read("s0.ini")) {
    printf("Something went wrong with the configuration file: \"s0.ini\"\n");
    return 1;
  }


  gpio_pins[0].wiringPi_number = 0; gpio_pins[0].pulses_per_kwh = 1000;
  gpio_pins[1].wiringPi_number = 1; gpio_pins[1].pulses_per_kwh = 1000;
  gpio_pins[2].wiringPi_number = 2; gpio_pins[2].pulses_per_kwh = 1000;
  gpio_pins[3].wiringPi_number = 3; gpio_pins[3].pulses_per_kwh = 1000;
  gpio_pins[4].wiringPi_number = 4; gpio_pins[4].pulses_per_kwh = 1000;
  gpio_pins[5].wiringPi_number = 5; gpio_pins[5].pulses_per_kwh = 1000;
  gpio_pins[6].wiringPi_number = 6; gpio_pins[6].pulses_per_kwh = 1000;
  gpio_pins[7].wiringPi_number = 7; gpio_pins[7].pulses_per_kwh = 1000;

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

    char outstr[250];
    strcpy(outstr, "counters:");
    char gpio;
    for (gpio = 0; gpio < 8 ; gpio++) {
      char countstr[20];
      snprintf(countstr, 29, " %d", gpio_pins[gpio].counter);
      strcat(outstr, countstr);
    }

    printf("%s\n", outstr);
  }

  return 0;
}

