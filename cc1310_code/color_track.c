/*
 * color_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "color_track.h"
#include "uart.h"

#define NUM_COLORS 3

static struct ColorTrack graphite = {.low_bound = GREY_LOW, .high_bound = GREY_HIGH, .detect_thresh = 5};

static struct ColorTrack purp = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .detect_thresh = 1};


char buffer[50];
void detect_xc(uint32_t * vals)
{
//    //check for white traingle
    if (vals[0] < 500 && vals[1] < 500 && vals[2] < 500 && vals[3] < 500)
    {
        GPIO_toggleDio(BLED1);
        set_intersection_flag(1);
    }
//    return;

//    purp.left_accum = 0;
//    purp.left_stash_val = 0;

//    purp.right_accum = 0;
//    purp.right_stash_val = 0;


    int i;
    uint8_t jarm = 1;
    for (i = 0; i < 4; i++)
    {
        jarm = jarm && (vals[i+1] > purp.low_bound && vals[i+1] < purp.high_bound);
        sprintf(buffer, "jarm: %u\r\n", jarm);
        WriteUART0(buffer);
    }

    if (jarm)
    {
        GPIO_toggleDio(BLED1);
        set_intersection_flag(1);
    }
return;

//    purp.left_prev_vals_ave = 0;
////    purp.right_prev_vals_ave = 0;
//
//
//    //just using center val
//    purp.left_prev_vals[purp.idx] =
//            (vals[2] > purp.low_bound && vals[2] < purp.high_bound) \
//            + (vals[3] > purp.low_bound && vals[3] < purp.high_bound);
//
//
//    int i;
//    for (i = 0; i < NUM_PREV_VALS; i++)
//    {
//        purp.left_prev_vals_ave += purp.left_prev_vals[i];
//    }
//
//    purp.idx = (purp.idx + 1) % NUM_PREV_VALS;
//
//    if(purp.left_prev_vals_ave > purp.detect_thresh)
//    {
//        GPIO_toggleDio(BLED1);
//        set_intersection_flag(1);
//        for (i = 0; i < NUM_PREV_VALS; i++)
//        {
//            purp.left_prev_vals[i] = 0;
//        }
//    }

    return;

}

void detect_poi(uint32_t * vals)
{
    graphite.left_accum = 0;
    graphite.left_stash_val = 0;

    graphite.right_accum = 0;
    graphite.right_stash_val = 0;

    graphite.left_prev_vals_ave = 0;
    graphite.right_prev_vals_ave = 0;

    graphite.left_prev_vals[graphite.idx] =
            (vals[3] > graphite.low_bound && vals[3] < graphite.high_bound) \
            + (vals[5] > graphite.low_bound + 20 && vals[5] < graphite.high_bound + 20);
    graphite.right_prev_vals[graphite.idx] =
            (vals[2] > graphite.low_bound && vals[2] < graphite.high_bound) \
            + (vals[4] > graphite.low_bound + 20 && vals[4] < graphite.high_bound + 20);


    int i;
    for (i = 0; i < NUM_PREV_VALS; i++)
    {
        graphite.left_prev_vals_ave += graphite.left_prev_vals[i];
        graphite.right_prev_vals_ave += graphite.right_prev_vals[i];
    }

    graphite.idx = (graphite.idx + 1) % NUM_PREV_VALS;

    if (!get_detect_flag() && !get_actuation_flag())
    {
        if (graphite.left_prev_vals_ave + graphite.right_prev_vals_ave > graphite.detect_thresh)
        {
            set_detect_flag(1);
            GPIO_toggleDio(BLED0);
            for (i = 0; i < NUM_PREV_VALS; i++)
            {
                graphite.left_prev_vals[i] = 0;
                graphite.right_prev_vals[i] = 0;
            }
        }
    }
    return;
}

