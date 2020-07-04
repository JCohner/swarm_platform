/*
 * color_track.c
 *
 *  Created on: Jun 22, 2020
 *      Author: jambox
 */

#include "color_track.h"
#include "uart.h"

#define NUM_COLORS 3

//static struct ColorTrack graphite = {.low_bound = GREY_LOW, .high_bound = GREY_HIGH, .detect_thresh = 4};

//static struct ColorTrack purp = {.low_bound = PURP_LOW, .high_bound = PURP_HIGH, .detect_thresh = 7};


char buffer[50];
void detect_xc(uint32_t * vals)
{
    //check for white traingle
    if (vals[0] < 150 && vals[1] < 150 && vals[2] < 150 && vals[3] < 150)
    {
        GPIO_toggleDio(BLED1);
        set_intersection_flag(1);
    }
    return;
}



//void detect_poi(uint32_t * vals)
//{
//    reinit_stash_and_accum();
//
//    //if sensor evaulates within boundary increment handed thresh val
////    purp.left_stash_val = (vals[3] > purp.low_bound && vals[3] < purp.high_bound) + (vals[5] > purp.low_bound + 20 && vals[5] < purp.high_bound + 20);
////    purp.right_stash_val = (vals[2] > purp.low_bound && vals[2] < purp.high_bound) + (vals[4] > purp.low_bound + 20 && vals[4] < purp.high_bound + 20);
//
//    graphite.left_stash_val = (vals[3] > graphite.low_bound && vals[3] < graphite.high_bound) +  (vals[5] > graphite.low_bound + 20 && vals[5] < graphite.high_bound + 20);
//    graphite.right_stash_val = (vals[2] > graphite.low_bound && vals[2] < graphite.high_bound) + (vals[4] > graphite.low_bound + 20 && vals[4] < graphite.high_bound + 20);
//
//    //set the current iteration of the moving average to stash val
////    purp.left_prev_vals[purp.idx] = purp.left_stash_val;
////    purp.right_prev_vals[purp.idx] = purp.right_stash_val;
//
//    graphite.left_prev_vals[graphite.idx] = graphite.left_stash_val;
//    graphite.right_prev_vals[graphite.idx] = graphite.right_stash_val;
//
//    int i;
//    for (i = 0; i < NUM_PREV_VALS; i++)
//    {
//        graphite.left_prev_vals_ave += graphite.left_prev_vals[i];
//        graphite.right_prev_vals_ave += graphite.right_prev_vals[i];
//
////        purp.left_prev_vals_ave += purp.left_prev_vals[i];
////        purp.right_prev_vals_ave += purp.right_prev_vals[i];
//    }
//
////    purp.idx = (purp.idx + 1) % NUM_PREV_VALS;
//    graphite.idx = (graphite.idx + 1) % NUM_PREV_VALS;
//
////    sprintf(buffer, "purp prev vals ave: %u, %u\r\n", purp.left_prev_vals_ave, purp.right_prev_vals_ave);
////    WriteUART0(buffer);
//    sprintf(buffer, "graph prev vals ave: %u, %u\r\n", graphite.left_prev_vals_ave, graphite.right_prev_vals_ave);
//    WriteUART0(buffer);
//
//
//    //check for white traingle
//    if (vals[0] < 150 && vals[1] < 150 && vals[2] < 150 && vals[3] < 150)
//    {
//        GPIO_toggleDio(BLED1);
//        set_intersection_flag(1);
//        return;
//    }
//
//    if (!get_detect_flag())
//    {
////        if (purp.left_prev_vals_ave > purp.detect_thresh || purp.right_prev_vals_ave > purp.detect_thresh)
////        {
////            set_detect_flag(1);
//////            GPIO_toggleDio(BLED0);
////        }
//        if (graphite.left_prev_vals_ave + graphite.right_prev_vals_ave > graphite.detect_thresh)
//        {
//            set_detect_flag(2);
//            GPIO_toggleDio(BLED1);
//            for (i = 0; i < NUM_PREV_VALS; i++)
//            {
//                graphite.left_prev_vals[i] = 0;
//                graphite.right_prev_vals[i] = 0;
//            }
//
//        }
//        else
//        {
//            set_detect_flag(0);
//        }
//
//    }
//    else
//    {
//        set_detect_flag(0);
//    }
//    return;
//}
//
//void reinit_stash_and_accum()
//{
//    purp.left_accum = 0;
//    purp.left_stash_val = 0;
//
//    purp.right_accum = 0;
//    purp.right_stash_val = 0;
//
//    purp.left_prev_vals_ave = 0;
//    purp.right_prev_vals_ave = 0;
//
//    graphite.left_accum = 0;
//    graphite.left_stash_val = 0;
//
//    graphite.right_accum = 0;
//    graphite.right_stash_val = 0;
//
//    graphite.left_prev_vals_ave = 0;
//    graphite.right_prev_vals_ave = 0;
//}
