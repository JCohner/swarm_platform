/*
 * rtc.h
 *
 *  Created on: Jul 5, 2020
 *      Author: jambox
 */

#ifndef RTC_H_
#define RTC_H_

#include <devices/cc13x0/driverlib/aon_rtc.h>
#include <stdint.h>

void InitRTC();
struct RTCVal GetRTCVal();
struct RTCVal GetDeltaTime(struct RTCVal time_prev, struct RTCVal time_curr);


struct RTCVal {
    uint32_t sec;
    uint32_t frac;
};


#endif /* RTC_H_ */
