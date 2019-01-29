#include <hf-risc.h>

/*
crc16 implementation:
width=16 poly=0x1021 init=0xffff refin=false refout=false xorout=0x0000 check=0x29b1 name="CRC-16/CCITT-FALSE"
*/
uint16_t crc16(uint8_t *data, uint32_t len){
	uint16_t crc = 0xffff, i;

	while(len--){ 
		crc ^= (uint16_t)*data++ << 8; 

		for(i = 0; i < 8; ++i) 
			crc = crc << 1 ^ (crc & 0x8000 ? 0x1021 : 0x0000); 
	}
	return crc;
}

/*
crc32 implementation:
width=32 poly=0x04c11db7 init=0xffffffff refin=false refout=false xorout=0x00000000 check=0x0376e6e7 name="CRC-32/MPEG-2"
*/
uint32_t crc32(uint8_t *data, uint32_t len){
	uint32_t crc = ~0, i;

	while(len--){
		crc ^= (uint32_t)*data++ << 24; 

		for(i = 0; i < 8; ++i)
			crc = crc << 1 ^ (crc & 0x80000000 ? 0x04c11db7 : 0x00000000); 
	}
	return crc;
}

/*
crc64 implementation:
width=64 poly=0x42f0e1eba9ea3693 init=0x0000000000000000 refin=false refout=false xorout=0x0000000000000000 check=0x6c40df5f0b497347 name="CRC-64"
*/
uint64_t crc64(uint8_t *data, uint32_t len){
	uint64_t crc = 0;
	uint32_t i;

	while(len--){
		crc ^= (uint64_t)*data++ << 56; 

		for(i = 0; i < 8; ++i)
			crc = crc << 1 ^ (crc & 0x8000000000000000 ? 0x42f0e1eba9ea3693 : 0x0000000000000000); 
	}
	return crc;
}

uint32_t xz_crc32_table[256];

void xz_crc32_init(void)
{
	const uint32_t poly = 0xEDB88320;

	uint32_t i;
	uint32_t j;
	uint32_t r;

	for (i = 0; i < 256; ++i) {
		r = i;
		for (j = 0; j < 8; ++j)
			r = (r >> 1) ^ (poly & ~((r & 1) - 1));

		xz_crc32_table[i] = r;
	}

	return;
}

uint32_t xz_crc32(const uint8_t *buf, uint32_t size, uint32_t crc)
{
	crc = ~crc;

	while (size != 0) {
		crc = xz_crc32_table[*buf++ ^ (crc & 0xFF)] ^ (crc >> 8);
		--size;
	}

	return ~crc;
}

void main(void){
	uint8_t *str = "123456789";
	int32_t time;
	uint16_t crc1;
	uint32_t crc2, ops;
	uint64_t crc3;

	time = TIMER0;
	crc1 = crc16(str, strlen(str));
	time = TIMER0 - time;
	printf("crc16: %8x, %d cycles\n", crc1, time);
	time = TIMER0;
	crc2 = crc32(str, strlen(str));
	time = TIMER0 - time;
	printf("crc32: %8x, %d cycles\n", crc2, time);
	time = TIMER0;
	crc3 = crc64(str, strlen(str));
	time = TIMER0 - time;
	printf("crc64: %8x%8x, %d cycles\n", (uint32_t)(crc3 >> 32), (uint32_t)(crc3 & 0xffffffff), time);

	xz_crc32_init();
	time = TIMER0;
	crc2 = xz_crc32(str, strlen(str), 0);
	time = TIMER0 - time;
	printf("crc32 (table): %8x, %d cycles\n", crc2, time);

	return;
}

