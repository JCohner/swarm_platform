/*
 * rtc.c
 *
 *  Created on: Jul 5, 2020
 *      Author: jambox
 */

#include "rtc.h"

//static struct RTCVal time_prev;
//static struct RTCVal time_curr;


void InitRTC()
{
    AONRTCEnable();
    //must also enable individual channels
//    AONRTCChannelEnable(AON_RTC_CH0);

    while(!AONRTCActive());

    return;
}

struct RTCVal GetRTCVal()
{
    struct RTCVal ret_val;
    uint32_t val = AONRTCCurrentCompareValueGet();
    ret_val.sec = (val & 0xFF00 >> 16);
    ret_val.frac = (val & 0x00FF);

    return ret_val;
}

struct RTCVal GetDeltaTime(struct RTCVal time_prev, struct RTCVal time_curr)
{
    struct RTCVal ret_val;
    ret_val.sec = time_curr.sec - time_prev.sec;
    ret_val.frac = time_curr.frac - time_prev.frac;
    return ret_val;



}
