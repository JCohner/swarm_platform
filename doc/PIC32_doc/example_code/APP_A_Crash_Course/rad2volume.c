// ***** file:  rad2volume.c *****
#include <math.h>                 // for the function pow
#include "rad2volume.h"           // if the header is in the same directory, use "quotes"

static double cuber(double x) {   // this function is not available externally
  return pow(x,3.0);
}

double radius2Volume(double rad) {  // function definition
  return (4.0/3.0)*MY_PI*cuber(rad);
}
