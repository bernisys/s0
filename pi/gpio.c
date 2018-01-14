/*
 * gpio.c:
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
#include <errno.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

/******************************
 *  private function prototypes
 */
void gpio_interrupt(void);


/******************************
 *  global variables
 */
t_gpio_pin gpio_pins[GPIO_PIN_COUNT];
unsigned long int count_global = 0;
unsigned char changed_global = 0;


/******************************
 *  initialization functions and setters
 */
void gpio_config_initialize(void) {
  volatile double now = time_get_precision();

  unsigned char gpio;
  for(gpio = 0; gpio < GPIO_PIN_COUNT ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    current->wiringPi_number = 255;
    current->state_last = 0;
    current->counter = 0;
    current->time_last = now;
    current->period_last = 0;
    current->changed = 0;
  }
}

int gpio_pin_set_wiring(unsigned char gpio, unsigned char pin_routing) {
  if (gpio >= GPIO_PIN_COUNT)
    return 1;

  gpio_pins[gpio].wiringPi_number = pin_routing;
  return 0;
}

int gpio_pin_set_values(unsigned char gpio, unsigned long int counter, double time_last, double period_last) {
  if (gpio >= GPIO_PIN_COUNT)
    return 1;

  gpio_pins[gpio].counter = counter;
  gpio_pins[gpio].time_last = time_last;
  gpio_pins[gpio].period_last = period_last;
  return 0;
}


// initializes the previously configured wiring for all pins
int gpio_init_wiring(void) {
  if (wiringPiSetup () < 0) {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return 1;
  }

  unsigned char gpio;
  for (gpio = 0; gpio < GPIO_PIN_COUNT ; gpio++) {
    t_gpio_pin *current = &gpio_pins[gpio];
    unsigned int number = current->wiringPi_number;
    if (number != 255) {
      #ifdef DEBUGGING
      printf("set GPIO.%d to pin %d\n", gpio, number);
      #endif
      pinMode(number, INPUT);
      pullUpDnControl(number, PUD_DOWN);
      if (wiringPiISR (number, INT_EDGE_BOTH, &gpio_interrupt) < 0) {
        fprintf (stderr, "Unable to setup ISR for GPIO.%d: %s\n", gpio, strerror (errno));
        return 1;
      } else {
        current->state_last = digitalRead(number);
      }
    }
  }
  return 0;
}



/******************************
 *  getters
 */
unsigned char gpio_global_get_changed(void) {
  return changed_global;
}

unsigned long int gpio_global_get_counter(void) {
  return count_global;
}

unsigned long int gpio_pin_get_counter(unsigned char gpio) {
  if (gpio >= GPIO_PIN_COUNT)
    return 0;
  return gpio_pins[gpio].counter;
}

double gpio_pin_get_time_last(unsigned char gpio) {
  if (gpio >= GPIO_PIN_COUNT)
    return 0;
  return gpio_pins[gpio].time_last;
}

double gpio_pin_get_period_last(unsigned char gpio) {
  if (gpio >= GPIO_PIN_COUNT)
    return 0;
  return gpio_pins[gpio].period_last;
}

unsigned char gpio_pin_get_changed(unsigned char gpio) {
  if (gpio >= GPIO_PIN_COUNT)
    return 0;
  return gpio_pins[gpio].changed;
}



/******************************
 *  interrupt handler
 */
void gpio_interrupt(void) {
  // retrieve current time for all operations iduring this IRQ
  volatile double now = time_get_precision();

  #ifdef DEBUGGING
  printf("%f INT:", now);
  #endif

  // check which GPIO pins triggered an interrupt on rising edge
  unsigned char gpio;
  for (gpio = 0; gpio < GPIO_PIN_COUNT ; gpio++) {
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
      current->changed = 1;

      // update global indicator
      count_global++;
      changed_global = 1;
    }
    current->state_last = state;
  }
  #ifdef DEBUGGING
  printf("\n"); 
  #endif
}

