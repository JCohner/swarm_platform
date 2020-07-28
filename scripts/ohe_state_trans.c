#include <stdio.h>
#include <stdint.h>

uint8_t mask_mask(uint8_t bb)
{

	uint8_t mask_mask = 0;
	for (int i = bb - 1; i >= 0; i--)
	{
		mask_mask |= 0b1 << i;
		// printf("mask: %X\r\n", mask);
	}

	return mask_mask;
}


uint8_t get_mask(uint8_t bb)
{
	uint8_t mask = ~(((0b1 << (bb - 1)))) & mask_mask(bb);
	return mask;
}


#define NUM_BRANCHS 2;

int main()
{
	uint8_t xcs = 0b001;
	uint8_t prev_xcs = xcs;

	uint8_t bbs[2] = {3,4};
	uint8_t bb_idx = 0;

	uint8_t bb = bbs[bb_idx];
	uint8_t ret = 0;

	uint8_t mask = get_mask(bb);
	printf("mask: %X\r\n", mask);
	for (int i = 0; i < 12; i++)
	{
		printf("xc: %X\r\n", xcs);
		xcs = (xcs & mask) << 1;
		xcs = xcs % mask;
		xcs = xcs | ret << (bb - 1);
		ret = (((xcs & (0b1 << (bb - 2)))) >> (bb - 2)) ^ ret;
		if (xcs < prev_xcs)
		{
			bb_idx = (bb_idx + 1) % NUM_BRANCHS;
			bb = bbs[bb_idx];
			mask = get_mask(bb);
		}

		prev_xcs = xcs;
	}
}