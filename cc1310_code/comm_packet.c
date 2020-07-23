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
    mach_id = id & 0x3;
}

uint8_t get_packet()
{
    //target bit

    uint8_t stator = get_xc_state();
    return  mach_id << 6 | get_target_flag() << 5 | get_policy() << 3 | stator;

}

void evaluate_packet(uint8_t packet)
{
    struct Packet info = {.target_flag = (packet & 0x20) >> 5,
                          .policy = (packet & 0x18) >> 3,
                          .xc_state = (packet & 0x07)};

    //if target flag of packet is high& yours is not
    if (packet & 0x20 && !get_target_flag() && check_near(&info))
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
    //check the handedness of the robots path
    if ((info->policy & 0b01) ==(get_policy() & 0b01))
    {

    }




}
