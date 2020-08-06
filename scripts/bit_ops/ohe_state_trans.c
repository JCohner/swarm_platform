//By Josh Cohen - 2020 
//The Learning Particles represent their state using a OHE schema that includes 
//	and MSB return bit (because they are making loops, their return path is predetermined 
//	such that the 'flow' pattern is maintained regardless of their actuation)
//This script demonstrates the bitwise logic needed to maintain such a state machine
//	bb = branch bits, number of bits to encode branch = #path_bits + 1 (for return bit)
//	ret = return bit, ie MSB in this schema
//	mask_mask() has to do with specifics of bitwise not (~), operates on 4 bit chunks
//		this means bb values of 0 != (x % 4) cause issues. mask_mask() fixes this with 3 bit
//		bb lengths, 5+ is still problematic. Work to be done here. 
//	Example desired *OHE behavior for bb = 4
//		0 001
//		0 010
//		0 100
//		1 001
//		1 010
//		1 100
//		0 001 

#include <stdio.h>
#include <stdint.h>

uint8_t mask_mask(uint8_t bb)
{

	uint8_t mask_mask = 0;
	//avoiding using bitwise not due to alignment issues
	for (int i = bb - 1; i >= 0; i--)
	{
		mask_mask |= 0b1 << i;
	}

	return mask_mask;
}


uint8_t get_mask(uint8_t bb)
{
	//mask mask takes care of alignment issues for bb = 3
	uint8_t mask = ~(((0b1 << (bb - 1)))) & mask_mask(bb);
	return mask;
}

//we have a branch of bb = 3 and bb = 4
#define NUM_BRANCHS 2;

int main()
{
	uint8_t xcs = 0b1100;
	uint8_t prev_xcs = xcs;

	uint8_t bbs[2] = {3,4};
	uint8_t bb_idx = 1;

	uint8_t bb = bbs[bb_idx];
	uint8_t ret = 0;

	uint8_t mask = get_mask(bb);
	printf("mask: %X\r\n", mask);

	for (int i = 0; i < 24; i++)
	{
		printf("xc: %X\r\n", xcs);
		//OHE encoding of all but MSB
		xcs = (xcs & mask) << 1;
		//Wrap all but MSB
		xcs = xcs % mask;
		//Add MSB back in to reflect if one return path
		xcs = xcs | ret << (bb - 1);
		//Tracks bit before MSB, this XORd with current MSB (ie ret) allows correct trans
		ret = (((xcs & (0b1 << (bb - 2)))) >> (bb - 2)) ^ ret;

		//Deals with switching between branches of different bb
		if (xcs < prev_xcs)
		{
			bb_idx = (bb_idx + 1) % NUM_BRANCHS;
			bb = bbs[bb_idx];
			mask = get_mask(bb);
			printf("mask: %X\r\n", mask);
		}

		prev_xcs = xcs;
	}
}