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

typedef struct {
  // GPIO related values
  unsigned char wiringPi_number;
  unsigned char state_last;

  // power meter related values
  unsigned int pulses_per_kwh;  // translation factor pulses <-> kWh

  // runtime values
  unsigned long int counter;
  double energy;
  volatile double time_last;
  volatile double period_last;
  double power_last;
} t_gpio_pin;


int gpio_config_read (char* filename);
void gpio_config_initialize(void);
int gpio_init_wiring(void);
unsigned long int gpio_get_count_global(void);
t_gpio_pin * gpio_get_status(unsigned int gpio);


#endif /* #ifndef _gpio_h */

