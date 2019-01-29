#include <hf-risc.h>

/*
XTEA encryption algorithm

based on reference code released into the public domain by David Wheeler and Roger Needham
the code takes 64 bits of data in v[0] and v[1] and 128 bits of key in key[0] - key[3]

recommended number of rounds is 32 (2 Feistel-network rounds are performed on each iteration).
*/

const uint32_t xtea_key[4] = {0xf0e1d2c3, 0xb4a59687, 0x78695a4b, 0x3c2d1e0f};

void encipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;

	for (i = 0; i < num_rounds; i++){
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}

void decipher(uint32_t num_rounds, uint32_t v[2], uint32_t const key[4]){
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * num_rounds;

	for (i = 0; i < num_rounds; i++){
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}
	v[0] = v0; v[1] = v1;
}

int main(void){
	uint32_t msg[2] = {0x12345678, 0x90123456};
	uint32_t cycles;

	printf("message: %8x%8x\n", msg[0], msg[1]);
	cycles = TIMER0;
	encipher(32, msg, xtea_key);
	cycles = TIMER0 - cycles;
	printf("encipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);
	cycles = TIMER0;
	decipher(32, msg, xtea_key);
	cycles = TIMER0 - cycles;
	printf("decipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	printf("message: %8x%8x\n", msg[0], msg[1]);

	XTEA_CONTROL = 0x2;
	XTEA_KEY0 = xtea_key[0];
	XTEA_KEY1 = xtea_key[1];
	XTEA_KEY2 = xtea_key[2];
	XTEA_KEY3 = xtea_key[3];

	cycles = TIMER0;
	XTEA_IN0 = msg[0];
	XTEA_IN1 = msg[1];
	XTEA_CONTROL = 0x3;
	while (!(XTEA_CONTROL & 0x4));
	XTEA_CONTROL = 0x0;
	cycles = TIMER0 - cycles;

	msg[0] = XTEA_OUT0;
	msg[1] = XTEA_OUT1;

	printf("encipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	cycles = TIMER0;

	XTEA_IN0 = msg[0];
	XTEA_IN1 = msg[1];
	XTEA_CONTROL = 0x1;
	while (!(XTEA_CONTROL & 0x4));
	XTEA_CONTROL = 0x0;

	cycles = TIMER0 - cycles;

	msg[0] = XTEA_OUT0;
	msg[1] = XTEA_OUT1;

	printf("decipher: %8x%8x, %d cycles\n", msg[0], msg[1], cycles);

	return 0;
}
