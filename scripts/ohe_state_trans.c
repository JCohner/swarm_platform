#include <stdio.h>
#include <stdint.h>

uint8_t mask_mask(uint8_t bb)
{
	switch(bb)
	{
		case 3:
		return 0x7;
		break;
		case 4:
		return 0xF;
		break;
	}
}


int main()
{
	uint8_t xcs = 0b001;
	uint8_t bb = 3;
	uint8_t ret = 0;

	uint8_t mask = ~(((0b1 << (bb - 1)))) & mask_mask(bb);
	printf("mask: %X\r\n", mask);
	for (int i = 0; i < 12; i++)
	{
		printf("xc: %X\r\n", xcs);
		xcs = (xcs & mask) << 1;
		xcs = xcs % mask;
		xcs = xcs | ret << (bb - 1);
		ret = (((xcs & (0b1 << (bb - 2)))) >> (bb - 2)) ^ ret;
		// printf("ret: %X\r\n", ret);

	}
}