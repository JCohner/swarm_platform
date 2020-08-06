#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t reverse_bits(uint8_t bits_in, uint8_t len)
{
    uint8_t bits_out = 0;
    int i;
    for (i = len; i > 0; i--)
    {
        bits_out = bits_out | ((bits_in & (0b1 << (i - 1))) >> (i -1))  << (len - i) ;

    }
    return bits_out;

}

int main (int argc, char * argv[])
{
	uint8_t len = atoi(argv[2]);
	uint8_t bits_in = atoi(argv[1]);

	// printf("bits: %X, len:%u\r\n", bits_in, len);

	uint8_t ret = reverse_bits(bits_in, len);
	printf("bitous out: %X\r\n", ret);
}