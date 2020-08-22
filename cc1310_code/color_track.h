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

#define PURP_LOW 325
#define PURP_HIGH 425

#define GREY_LOW 470
#define GREY_HIGH 750


#define NUM_PREV_VALS 10
struct ColorTrack {
    char curr_state;
    char prev_state;

    uint8_t left_accum;
    uint8_t right_accum;
    uint8_t left_stash_val;
    uint8_t right_stash_val;
    uint16_t left_prev_vals[NUM_PREV_VALS];
    uint16_t right_prev_vals[NUM_PREV_VALS];
    uint8_t left_prev_vals_ave;
    uint8_t right_prev_vals_ave;

    uint8_t idx;

    uint8_t detect_thresh;

    uint16_t high_bound;
    uint16_t low_bound;
};

void detect_xc(uint32_t * vals);
void detect_poi(uint32_t * vals);
struct ColorTrack * get_color(uint8_t color);
void reinit_stash_and_accum();

#endif /* COLOR_TRACK_H_ */
