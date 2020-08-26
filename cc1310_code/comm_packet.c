/*
 * comm_packet.c
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */


#include "comm_packet.h"
#include "uart.h"
#include "gpio.h"

static uint16_t mach_id;

void set_mach_id(uint16_t id)
{
    mach_id = id;
}

uint16_t get_mach_id()
{
    return mach_id;
}
static char buffer[50];
uint32_t get_packet()
{
    return  get_mach_id() << MACH_SHIFT |
            get_target_flag() << TFLAG_SHIFT |
            get_bb_idx() << BBI_SHIFT |
            get_policy() << POL_SHIFT |
            get_xc_state() << STATE_SHIFT;

}

void evaluate_packet(uint32_t packet)
{
    sprintf(buffer, "packet: %X\r\n", packet);
    WriteUART0(buffer);

    uint32_t send_id = (packet & MACH_MASK) >> MACH_SHIFT;
//    sprintf(buffer, "send_id: %X\r\n", send_id);
//    WriteUART0(buffer);
    if (send_id == mach_id || send_id == UNIV_ID)
    {
        evaluate_command(packet);
        return;
    }

    struct Packet info = {.mach_id = (packet & MACH_MASK) >> MACH_SHIFT,
                          .target_flag = (packet & TFLAG_MASK) >> TFLAG_SHIFT,
                          .policy = (packet & POLICY_MASK) >> POL_SHIFT,
                          .bb_idx = (packet & BBI_MASK) >> BBI_SHIFT,
                          .xc_state = (packet & STATE_MASK) >> STATE_SHIFT};


    //if target flag of packet is high& yours is not
    if (info.target_flag && !get_target_flag() && check_near(&info))
    {
        set_neighbor_target_flag(1);
        //set their policy as your own policy
        set_neighbor_target_policy(info.policy);
    }
}

//checks if sender robot is near enough to communicate
uint8_t check_near(struct Packet * info)
{
    //needs to be expanded for circulatory system case
    //could also be wrapped into a bitwise machine, just dont have time rn
    if ((info->xc_state == get_xc_state()) && (info->bb_idx == get_bb_idx()))
    {
        if(info->xc_state == 6 || info->xc_state == 0xC)
        {
            return 1;
        }
        else if ( (info->xc_state == 1 || info->xc_state == 5 || info->xc_state == 0xA)
                && ( (info->policy & 0b01) == (get_policy() & 0b01)))
        {
            return 1;
        } else if ((info->xc_state == 2 || info->xc_state == 4) && (get_policy() == info->policy))
        {
            return 1;
        }
        else if (info->xc_state == 2 && (info->policy & 0b11) == (get_policy() & 0b11))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//sets internal properties
void evaluate_command(uint32_t packet)
{
    uint8_t val;
    switch ((packet & COMMAND_MASK) >> COMMAND_SHIFT)
    {
    case START_CMD:
        val = (packet & START_BIT_MASK);
        WriteUART0("gotta dinger\r\n");
        set_enable_flag(val);
        GPIO_toggleDio(CC1310_LAUNCHXL_PIN_GLED);
        break;
    case NEW_POLICY_CMD:
        val = (packet & NEW_POLICY_MASK);
        set_new_policy(val); //might have to figure out how this fits in state machine
        set_new_policy_flag(1);
        break;
    default:
        WriteUART0("Command not implemented\r\n");
    }
}


