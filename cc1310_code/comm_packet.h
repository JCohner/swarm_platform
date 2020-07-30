/*
 * comm_packet.h
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */

#ifndef COMM_PACKET_H_
#define COMM_PACKET_H_

#include "state_track.h"

#define POLICY_MASK 0x003E0
#define TFLAG_MASK 0x00400
#define BBI_MASK 0x000010
#define STATE_MASK 0x00000F

#define MACH_SHIFT 11
#define POL_SHIFT 5
#define TFLAG_SHIFT 10
#define BBI_SHIFT 4
#define STATE_SHIFT 0

struct Packet {
    uint8_t target_flag : 1;
    uint8_t policy : 5;
    uint8_t bb_idx : 1;
    uint8_t xc_state : 4;
};

uint16_t get_packet();
void set_mach_id(uint8_t id);
void evaluate_packet(uint16_t packet);
uint8_t check_near(struct Packet * info);
#endif /* COMM_PACKET_H_ */
