/*
 * color_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "color_track.h"
#include "uart.h"
#define NUM_COLORS 3

static struct ColorTrack graphite = {.low_bound = GREY_LOW, .high_bound = GREY_HIGH, .idx = 0,
                                     .curr_state = 0, .accum = 0, .stash_val = 0, .detect_thresh = 7};

static struct ColorTrack purp_left = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .idx = 0,
                                      .curr_state = 0, .accum = 0, .stash_val = 0, .detect_thresh = 6};

static struct ColorTrack purp_right = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .idx = 0,
                                       .curr_state = 0, .accum = 0, .stash_val = 0, .detect_thresh = 6};

void detect_poi(uint32_t * vals)
{
    reinit_stash_and_accum();

    if (vals[3] > purp_left.low_bound && vals[3] < purp_left.high_bound && vals[5] > purp_left.low_bound + 20 && vals[5] < purp_left.high_bound + 20)
    {
        purp_left.stash_val = 1;
    }
    if (vals[2] > purp_left.low_bound && vals[2] < purp_left.high_bound && vals[4] > purp_left.low_bound + 20 && vals[4] < purp_right.high_bound + 20)
    {
        purp_right.stash_val = 1;
    }
    graphite.prev_vals[graphite.idx] = graphite.stash_val;
    purp_left.prev_vals[purp_left.idx] = purp_left.stash_val;
    purp_right.prev_vals[purp_right.idx] = purp_right.stash_val;
    int i;
    for (i = 0; i < NUM_PREV_VALS; i++)
    {
        graphite.prev_vals_ave += graphite.prev_vals[i];

        purp_left.prev_vals_ave += purp_left.prev_vals[i];
        purp_right.prev_vals_ave += purp_right.prev_vals[i];
    }

//    sprintf(buffer, "ave: %u\r\n", purp.prev_vals_ave);
//    WriteUART0(buffer);

    if (graphite.prev_vals_ave > 5 && graphite.curr_state == 0)
    {
        GPIO_toggleDio(BLED0);
        graphite.curr_state = 1;
    }
    else
    {
        graphite.curr_state = 0;
    }


    check_state();

    purp_left.idx = (purp_left.idx + 1) % NUM_PREV_VALS;
    purp_right.idx = (purp_right.idx + 1) % NUM_PREV_VALS;
    graphite.idx = (graphite.idx + 1) % NUM_PREV_VALS;

    return;
}

char buffer[50];
/// \brief updates state of state flagss
void check_state()
{
    sprintf(buffer, "prev vals ave: %u, %u\r\n", purp_left.prev_vals_ave, purp_right.prev_vals_ave);
    WriteUART0(buffer);
    uint8_t prev_flag = get_flags();
    uint8_t actuation_flag = get_actuation_flag();

    //if we are currently actuating, do not overwite prev flag and do not set a new flag
    if (!actuation_flag){
        set_prev_flags(prev_flag);
    }
    else
    {
        return;
    }

    if (purp_left.prev_vals_ave >= purp_left.detect_thresh && prev_flag == NO_DETECT)
    {
        set_flags(DETECT_0);
    }
    else if (purp_right.prev_vals_ave >= purp_right.detect_thresh && prev_flag == DETECT_0)
    {
        set_flags(DETECT_1);
    }
    else if (purp_left.prev_vals_ave >= purp_left.detect_thresh && prev_flag == DETECT_1)
    {
        //need to check return policy here
        set_return_flag(1);
        set_flags(DETECT_0);
    } else if (purp_right.prev_vals_ave >= purp_right.detect_thresh && prev_flag == DETECT_0 && get_return_flag())
    {
        set_flags(NO_DETECT);
        set_return_flag(0);
    }
}

void reinit_stash_and_accum()
{
    purp_left.accum = 0;
    purp_left.stash_val = 0;

    purp_right.accum = 0;
    purp_right.stash_val = 0;

    graphite.accum = 0;
    graphite.stash_val = 0;

    graphite.prev_vals_ave = 0;
    purp_left.prev_vals_ave = 0;
    purp_right.prev_vals_ave = 0;
}
