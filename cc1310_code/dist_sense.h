/*
 * dist_sense.h
 *
 *  Created on: Jul 23, 2020
 *      Author: jambox
 */

#ifndef DIST_SENSE_H_
#define DIST_SENSE_H_

#include "adc.h"
#include <stdint.h>

uint32_t ReadDistForward();

uint32_t ReadDist45();
#endif /* DIST_SENSE_H_ */
