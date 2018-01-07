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
  unsigned char changed;
} t_gpio_pin;


int gpio_config_read (char* filename);
int gpio_last_values_read (char* filename);
void gpio_config_initialize(void);
int gpio_init_wiring(void);

unsigned long int gpio_get_count_global(void);
unsigned char gpio_get_changed_global(void);
t_gpio_pin * gpio_get_status(unsigned int gpio);
void gpio_get_entry_string(unsigned char gpio, char * outstr, unsigned int length, unsigned char type);

#endif /* #ifndef _gpio_h */

