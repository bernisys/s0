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

#ifndef _gpio_h
#define _gpio_h

//#define DEBUGGING
#define GPIO_PIN_COUNT 8

typedef struct {
  unsigned char wiringPi_number;
  unsigned char state_last;
  unsigned long int counter;
  volatile double time_last;
  volatile double period_last;
  unsigned char changed;
} t_gpio_pin;


/******************************
 *  initialization functions and setters
 */
void gpio_config_initialize(void);
int gpio_pin_set_wiring(unsigned char gpio, unsigned char pin_routing);
int gpio_pin_set_values(unsigned char gpio, unsigned long int counter, double time_last, double period_last);
int gpio_init_wiring(void);

/******************************
 *  getters
 */
unsigned char gpio_global_get_changed(void);
unsigned long int gpio_global_get_counter(void);

unsigned long int gpio_pin_get_counter(unsigned char gpio);
double gpio_pin_get_time_last(unsigned char gpio);
double gpio_pin_get_period_last(unsigned char gpio);
unsigned char gpio_pin_get_changed(unsigned char gpio);

#endif /* #ifndef _gpio_h */

