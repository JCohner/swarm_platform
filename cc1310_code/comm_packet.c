/*
 * comm_packet.c
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */


#include "comm_packet.h"

static uint8_t mach_id;

void set_mach_id(uint8_t id)
{
    mach_id = id & 0x3;
}

uint8_t get_packet()
{
    //target bit

    uint8_t stator = get_return_flag() << 2|get_xc_state();
    return  mach_id << 6 | get_target_flag() << 5 | get_policy() << 3 | stator;

}

void interpret_packet(uint8_t packet)
{
    //if target flag of packet is high& yours is not
    if (packet & 0x20 && !get_target_flag())
    {
        //set their policy as your own policy
        uint8_t policy = packet & 0x18;
        set_policy(policy);
    }
}
