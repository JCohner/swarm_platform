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
    uint8_t match_first_pol_bit = ((info->policy & 0b01) == (get_policy() & 0b01));

    switch(info->xc_state ^ get_xc_state())
    {
        case(0b000):
                //x!=2 ^ x!=2 - clear to send
                //2 ^ 2 - must check first policy bit

                //both CTS if pol matches
                if (match_first_pol_bit)
                {
                    return 1;
                }
                //if not 2 CTS
                else if (info->xc_state != 0b010)
                {
                    return 1;
                }
                //NOT CTS if both at 2 but do not share first pol bit
                else
                {
                    return 0;
                }

                break;
        case(0b011):
                // 1 ^ 2 - must check first policy bit
                // 5 ^ 6 - clear to send

                //both CTS if pol matches
                if (match_first_pol_bit)
                {
                    return 1;
                }
                //if info is 5 | 6 CTS
                else if (info->xc_state & 0b100)
                {
                    return 1;
                }
                //NOT CTS - if MSB != 1 (as above) must be 1 ^ 2 with matching LSB pol
                else
                {
                    return 0;
                }

                break;
        case(0b100):
                // 1 ^ 5 - NOT clear to send
                // 2 ^ 6 - NOT clear to send
                return 0;
                break;
        case(0b111):
                //1 ^ 6 - clear to send
                //2 ^ 5 - must check first policy bit
                if (match_first_pol_bit)
                {
                    return 1;
                }
                // 6 - 1 is 5 and CTS
                else if ((info->xc_state - get_xc_state()) == 5)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
                break;
        default:
            while(1);
    }
}
