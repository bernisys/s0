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

#ifndef _s0_h
#define _s0_h

//#define DEBUGGING


/******************************
 *  specific datatypes
 */
typedef struct {
  // GPIO related values
  unsigned char wiringPi_number;
  unsigned char state_last;

  // power meter related values
  unsigned int pulses_per_kwh;  // translation factor pulses <-> kWh

  // runtime values
  unsigned long int counter;
  double energy;
  double time_last;
  double period_last;
  double power_last;
} t_gpio_pin;



/******************************
 * Prototypes
 */
void myInterrupt(void);
void gpio_config_initialize(void);
double time_get_precision(void);


#endif /* #ifndef _s0_h */

