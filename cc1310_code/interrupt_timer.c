/*
 * interupt_timer.c
 *
 *  Created on: Jul 5, 2020
 *      Author: jambox
 */

#include <interrupt_timer.h>

#include "gpio.h"
#include "zumo_moves.h"
#include "zumo_rf.h"
#include "comm_packet.h"
#include "state_track.h"
#include "ir_sense.h"
#include "color_track.h"
#include "leds.h"
#include "dist_sense.h"

void TimerInt()
{
    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
    TimerIntClear(GPT1_BASE, TIMER_TIMB_TIMEOUT);
}

void pack_and_transmit()
{
//    GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
    rf_main();
    WriteRFPacket(get_packet());
    TimerIntClear(GPT3_BASE, TIMER_TIMA_TIMEOUT);
}
static uint32_t adc_vals[8];
void main_loop()
{
    TimerIntClear(GPT1_BASE, TIMER_TIMB_TIMEOUT);
//    WriteUART0("dinga\r\n");
    if (get_enable_flag())
    {
    ReadIR(adc_vals);

    //ODD IS LEFT
//          sprintf(buffer,"%u, %u, %u, %u, %u, %u\r\n", adc_vals[5], adc_vals[3], adc_vals[1],
//                            adc_vals[0], adc_vals[2], adc_vals[4]);
//          WriteUART0(buffer);
    float IR_val = read_line(adc_vals);
    uint32_t Dist_val = ReadDist();
//          sprintf(buffer, "DIST VAL: %f\r\n", Dist_val);
//          WriteUART0(buffer);
    drive_line(IR_val, Dist_val, adc_vals);

    detect_poi(adc_vals);
    detect_xc(adc_vals);
    inc_state();
    manage_intersection();

//    manage_leds();
    }
    else
    {
        setMotor(M2, 0, 0);
        setMotor(M1, 0, 0);
    }
}



void InterTimerEnable()
{

    //enable peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER1);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER3);
    PRCMLoadSet();
    while ( !PRCMLoadGet() );

    TimerDisable(GPT1_BASE,TIMER_A);
    TimerDisable(GPT1_BASE,TIMER_B);


    TimerConfigure(GPT1_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC)); //
    TimerPrescaleSet(GPT1_BASE, TIMER_A, 0xFF); // this doesnt work because it thinks its in 32 bit mode
    TimerLoadSet(GPT1_BASE, TIMER_A, 0xFFFF / 256);
//    TimerMatchSet(GPT1_BASE, TIMER_A, 0xFFFF / 4);
//
    TimerPrescaleSet(GPT1_BASE, TIMER_B, 0xFF);
//    TimerPrescaleMatchSet(GPT1_BASE, TIMER_B, 0xFF);
    TimerLoadSet(GPT1_BASE, TIMER_B, 0xFFFF / 16);
//    TimerMatchSet(GPT1_BASE, TIMER_B, 0xFFFF / 2);


    TimerConfigure(GPT3_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC));
    TimerPrescaleSet(GPT3_BASE, TIMER_A, 0xFF);
    TimerLoadSet(GPT3_BASE, TIMER_A, 0xFFFF / 32); // /512

    IntMasterEnable();
    TimerIntRegister(GPT1_BASE, TIMER_A, openloop_turn); //openloop_turn
    TimerIntEnable(GPT1_BASE, TIMER_TIMA_TIMEOUT);
    IntPrioritySet(INT_GPT1A, INT_PRI_LEVEL3);
    IntEnable(INT_GPT1A);
//
    TimerIntRegister(GPT1_BASE, TIMER_B, main_loop);//pack_and_transmit
    TimerIntEnable(GPT1_BASE, TIMER_TIMB_TIMEOUT);
    IntPrioritySet(INT_GPT1B, INT_PRI_LEVEL4);
    IntEnable(INT_GPT1B);

    TimerIntRegister(GPT3_BASE, TIMER_A, pack_and_transmit);//pack_and_transmit
    TimerIntEnable(GPT3_BASE, TIMER_TIMA_TIMEOUT);
    IntPrioritySet(INT_GPT3A, INT_PRI_LEVEL2);
    IntEnable(INT_GPT3A);


    TimerEnable(GPT1_BASE,TIMER_A);
    TimerEnable(GPT1_BASE,TIMER_B);
    TimerEnable(GPT3_BASE,TIMER_A);
}


