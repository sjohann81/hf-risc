#include <hf-risc.h>

void encode(uint8_t *enc, uint8_t *clear, uint32_t len, uint8_t *key, uint32_t klen)
{
	uint32_t i;
	
	for (i = 0; i < len; i++)
		enc[i] = (clear[i] + key[i % klen]) & 0xff;
}

void decode(uint8_t *clear, uint8_t *enc, uint32_t len, uint8_t *key, uint32_t klen)
{
	uint32_t i;
	
	for (i = 0; i < len; i++)
		clear[i] = (256 + enc[i] - key[i % klen]) & 0xff;
}

int main(void)
{
	uint8_t message[] = "the quick brown fox jumps over the lazy dog";
	uint8_t key[] = "attackatdawn";
	uint8_t key2[] = "attack at dawn";
	uint8_t *pmessage, *pkey, *pkey2;
	
	pmessage = (uint8_t *)0x60000000;
	pkey = (uint8_t *)0x60000100;
	pkey2 = (uint8_t *)0x60000200;
	
	memset(pmessage, 0x55, 1024);	
	memcpy(pmessage, message, strlen(message));
	memcpy(pkey, key, strlen(key));
	memcpy(pkey2, key2, strlen(key2));
	
	
	printf("\nmessage:");
	hexdump(pmessage, sizeof(message));
	encode(pmessage, pmessage, sizeof(message), pkey, sizeof(key));
	printf("\nencoded message (first key):");
	hexdump(pmessage, sizeof(message));
	decode(pmessage, pmessage, sizeof(message), pkey, sizeof(key));
	printf("\ndecoded message (first key):");
	hexdump(pmessage, sizeof(message));
	encode(pmessage, pmessage, sizeof(message), pkey2, sizeof(key2));
	printf("\nencoded message (second key):");
	hexdump(pmessage, sizeof(message));
	decode(pmessage, pmessage, sizeof(message), pkey2, sizeof(key2));
	printf("\ndecoded message (second key):");
	hexdump(pmessage, sizeof(message));
	
	return 0;
}
