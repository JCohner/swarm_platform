/*
 * color_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "color_track.h"
#include "uart.h"
#define NUM_COLORS 3

//static struct ColorTrack graphite = {.low_bound = GREY_LOW, .high_bound = GREY_HIGH, .idx = 0,
//                                     .curr_state = 0, .accum = 0, .stash_val = 0, .detect_thresh = 7};

static struct ColorTrack purp = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .detect_thresh = 5};

//static struct ColorTrack purp_right = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .idx = 0,
//                                       .curr_state = 0, .accum = 0, .stash_val = 0, .detect_thresh = 5};

char buffer[50];
void detect_poi(uint32_t * vals)
{
    reinit_stash_and_accum();

    if (vals[3] > purp.low_bound && vals[3] < purp.high_bound && vals[5] > purp.low_bound + 20 && vals[5] < purp.high_bound + 20)
    {
        purp.left_stash_val = 1;
    }
    if (vals[2] > purp.low_bound && vals[2] < purp.high_bound && vals[4] > purp.low_bound + 20 && vals[4] < purp.high_bound + 20)
    {
        purp.right_stash_val = 1;
    }
//    graphite.prev_vals[graphite.idx] = graphite.stash_val;
    purp.left_prev_vals[purp.idx] = purp.left_stash_val;
    purp.right_prev_vals[purp.idx] = purp.right_stash_val;
//    purp_right.prev_vals[purp_right.idx] = purp_right.stash_val;
    int i;
    for (i = 0; i < NUM_PREV_VALS; i++)
    {
//        graphite.prev_vals_ave += graphite.prev_vals[i];

        purp.left_prev_vals_ave += purp.left_prev_vals[i];
        purp.right_prev_vals_ave += purp.right_prev_vals[i];
    }

    purp.idx = (purp.idx + 1) % NUM_PREV_VALS;
    //    graphite.idx = (graphite.idx + 1) % NUM_PREV_VALS;

    sprintf(buffer, "prev vals ave: %u, %u\r\n", purp.left_prev_vals_ave, purp.right_prev_vals_ave);
    WriteUART0(buffer);

    if (purp.left_prev_vals_ave >= purp.detect_thresh ||
            purp.right_prev_vals_ave >= purp.detect_thresh)
    {
        set_detect_flag(1);
    }
    else
    {
        set_detect_flag(0);
    }

    return;
}

void reinit_stash_and_accum()
{
    purp.left_accum = 0;
    purp.left_stash_val = 0;

    purp.right_accum = 0;
    purp.right_stash_val = 0;

//    graphite.accum = 0;
//    graphite.stash_val = 0;
//
//    graphite.prev_vals_ave = 0;
    purp.left_prev_vals_ave = 0;
    purp.right_prev_vals_ave = 0;
}
