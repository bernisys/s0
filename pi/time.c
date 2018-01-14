/***********************************************************************
 *
 * time.c:
 * time handling helper functions
 *
 ***********************************************************************
 */


#include "time.h"
#include <sys/time.h>

#include <stdio.h>

volatile double time_get_precision(void) {
  struct timeval tv;
  gettimeofday(&tv, 0);
  volatile double tval;

  tval  = tv.tv_sec + (tv.tv_usec / 1000000.0);
  #ifdef DEBUGGING
  printf("%lu %lu  %f\n", tv.tv_sec, tv.tv_usec, tval);
  #endif
  return tval;
}

