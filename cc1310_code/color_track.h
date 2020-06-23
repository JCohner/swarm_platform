/*
 * color_track.h
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#ifndef COLOR_TRACK_H_
#define COLOR_TRACK_H_

#include <stdint.h>
#include "helpful.h"
#include "state_track.h"
#define PURP_LOW 100
#define PURP_HIGH 140

#define GREY_LOW 175
#define GREY_HIGH 220


#define NUM_PREV_VALS 10
struct ColorTrack {
    char curr_state;
    char prev_state;
    uint8_t accum;
    uint8_t stash_val;
    uint16_t prev_vals[NUM_PREV_VALS];
    uint8_t prev_vals_ave;
    uint8_t idx;

    uint8_t detect_thresh;

    uint16_t high_bound;
    uint16_t low_bound;

};
void check_state();
void detect_poi(uint32_t * vals);
struct ColorTrack * get_color(uint8_t color);
void reinit_stash_and_accum();

#endif /* COLOR_TRACK_H_ */
