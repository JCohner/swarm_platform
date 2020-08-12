/*
 * comm_packet.h
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */

#ifndef COMM_PACKET_H_
#define COMM_PACKET_H_

#include "state_track.h"

#define MACH_MASK 0xFFFF0000
#define POLICY_MASK 0x0003E0
#define TFLAG_MASK 0x000400
#define BBI_MASK 0x000010
#define STATE_MASK 0x00000F

#define MACH_SHIFT 16
#define POL_SHIFT 5
#define TFLAG_SHIFT 10
#define BBI_SHIFT 4
#define STATE_SHIFT 0

struct Packet {
    uint16_t mach_id : 16;
    uint8_t pad : 5;
    uint8_t target_flag : 1;
    uint8_t policy : 5;
    uint8_t bb_idx : 1;
    uint8_t xc_state : 4;
};

#define UNIV_ID 0x3F

#define COMMAND_MASK 0x0100
#define START_BIT_MASK 0x0001
#define NEW_POLICY_MASK 0x001F

#define COMMAND_SHIFT 8
#define START_CMD 0
#define NEW_POLICY_CMD 1

uint32_t get_packet();
void set_mach_id(uint16_t id);
uint16_t get_mach_id();
void evaluate_packet(uint32_t packet);
void evaluate_command(uint32_t packet);
uint8_t check_near(struct Packet * info);
#endif /* COMM_PACKET_H_ */
