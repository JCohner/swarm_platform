/*
 * comm_packet.c
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */


#include "comm_packet.h"
#include "uart.h"

static uint8_t mach_id;

void set_mach_id(uint8_t id)
{
    mach_id = id;
}
static char buffer[50];
uint16_t get_packet()
{
    return  mach_id << MACH_SHIFT |
            get_target_flag() << TFLAG_SHIFT |
            get_bb_idx() << BBI_SHIFT |
            get_policy() << POL_SHIFT |
            get_xc_state() << STATE_SHIFT;

}

void evaluate_packet(uint16_t packet)
{
    struct Packet info = {.target_flag = (packet & TFLAG_MASK) >> TFLAG_SHIFT,
                          .policy = (packet & POLICY_MASK) >> POL_SHIFT,
                          .bb_idx = (packet & BBI_MASK) >> BBI_SHIFT,
                          .xc_state = (packet & STATE_MASK) >> STATE_SHIFT};

    //if target flag of packet is high& yours is not
    if (info.target_flag && !get_target_flag() && check_near(&info))
    {
        set_neighbor_target_flag(1);
        //set their policy as your own policy
        uint8_t policy = (packet & 0x18) >> 3;
        set_neighbor_target_policy(policy);
    }
}

//checks if sender robot is near enough to communicate
uint8_t check_near(struct Packet * info)
{
    if (!(info->xc_state ^ get_xc_state()) && !(info->bb_idx ^ get_bb_idx()))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
