/*
 * comm_packet.h
 *
 *  Created on: Jul 13, 2020
 *      Author: jambox
 */

#ifndef COMM_PACKET_H_
#define COMM_PACKET_H_

#include "state_track.h"

//struct Packet {
//    uint8_t target_flag : 1;
//    uint8_t policy : 2;
//    uint8_t state : 3;
//};

uint8_t get_packet();
void set_mach_id(uint8_t id);

#endif /* COMM_PACKET_H_ */
