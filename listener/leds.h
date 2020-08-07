/*
 * leds.h
 *
 *  Created on: Jul 24, 2020
 *      Author: jambox
 */

#ifndef LEDS_H_
#define LEDS_H_

#include "gpio.h"
//#include "state_track.h"
#include <stdint.h>

#define NUM_INFO_LEDS 3

void manage_leds();

void setup_leds();


#endif /* LEDS_H_ */
