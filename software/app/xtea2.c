#include <hf-risc.h>

/*
XTEA encryption algorithm

based on reference code released into the public domain by David Wheeler and Roger Needham
the code takes 64 bits of data in v[0] and v[1] and 128 bits of key in key[0] - key[3]

recommended number of rounds is 32 (2 Feistel-network rounds are performed on each iteration).
*/

void xtea_encrypt(uint32_t v[2], const uint32_t key[4], uint32_t num_rounds)
{
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;

	for (i = 0; i < num_rounds; i++){
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}

void xtea_decrypt(uint32_t v[2], const uint32_t key[4], uint32_t num_rounds)
{
	uint32_t i;
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * num_rounds;

	for (i = 0; i < num_rounds; i++){
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}
	v[0] = v0; v[1] = v1;
}


/* XTEA stream cipher, CBC mode
 * CBC mode based on https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */
#define BLOCKLEN	8		// in bytes

void xtea_cbc_encrypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t key[4], const uint32_t iv[2])
{
	uint32_t i, rem, block[2], tiv[2];
	
	rem = len % BLOCKLEN;
	tiv[0] = iv[0];
	tiv[1] = iv[1];
	for (i = 0; i < len; i += BLOCKLEN) {
		memcpy((char *)block, in, BLOCKLEN);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		xtea_encrypt(block, key, 32);
		tiv[0] = block[0];
		tiv[1] = block[1];
		memcpy(out, (char *)block, BLOCKLEN);
		in += BLOCKLEN;
		out += BLOCKLEN;
	}
	if (rem) {
		memcpy((char *)block, in, BLOCKLEN - rem);
		memset((char *)block + rem, 0, BLOCKLEN - rem);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		xtea_encrypt(block, key, 32);
		memcpy(out, (char *)block, BLOCKLEN - rem);
	}
}

void xtea_cbc_decrypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t key[4], const uint32_t iv[2])
{
	uint32_t i, rem, block[2], block2[2], tiv[2];
	
	rem = len % BLOCKLEN;
	tiv[0] = iv[0];
	tiv[1] = iv[1];
	for (i = 0; i < len; i += BLOCKLEN) {
		memcpy((char *)block, in, BLOCKLEN);
		block2[0] = block[0];
		block2[1] = block[1];
		xtea_decrypt(block, key, 32);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		tiv[0] = block2[0];
		tiv[1] = block2[1];
		memcpy(out, (char *)block, BLOCKLEN);
		in += BLOCKLEN;
		out += BLOCKLEN;
	}
	if (rem) {
		memcpy((char *)block, in, BLOCKLEN - rem);
		memset((char *)block + rem, 0, BLOCKLEN - rem);
		tiv[0] = block[0];
		tiv[1] = block[1];
		xtea_decrypt(block, key, 32);
		block[0] ^= tiv[0];
		block[1] ^= tiv[1];
		memcpy(out, (char *)block, BLOCKLEN - rem);
	}
}

/* XTEA stream cipher, CTR mode
 * CTR mode based on https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation
 */
#define BLOCKLEN	8		// in bytes

void xtea_ctr_crypt(uint8_t *out, uint8_t *in, uint32_t len, const uint32_t key[4], const uint32_t nonce[2])
{
	uint32_t i, k, rem;
	uint32_t data[2];
	uint8_t *data2 = (uint8_t *)&data;
	uint64_t counter = 0;
	uint32_t *in_p = (uint32_t *)in;
	uint32_t *out_p = (uint32_t *)out;
	
	rem = len % BLOCKLEN;
	for (i = 0; i < len; i += BLOCKLEN) {
		data[0] = nonce[0] ^ (counter & 0xffffffff);
		data[1] = nonce[1] ^ (counter >> 32);
		xtea_encrypt(data, key, 32);
		out_p[0] = in_p[0] ^ data[0];
		out_p[1] = in_p[1] ^ data[1];
		in_p += 2;
		out_p += 2;
		counter++;
	}
	
	in = (uint8_t *)in_p;
	out = (uint8_t *)out_p;
	
	if (rem) {
		data[0] = nonce[0] ^ (counter & 0xffffffff);
		data[1] = nonce[1] ^ (counter >> 32);		
		xtea_encrypt(data, key, 32);
		for (k = 0; k < rem; k++)
			out[k] = in[k] ^ data2[k];
	}
}

int main(void){
	uint8_t message[] = "the quick brown fox jumps over the lazy dog";
	uint32_t xtea_key[4] = {0xf0e1d2c3, 0xb4a59687, 0x78695a4b, 0x3c2d1e0f};
	uint32_t iv[2] = {0x11223344, 0x55667788};
	
	printf("\nmessage:");
	hexdump((char *)message, sizeof(message));
	
	xtea_cbc_encrypt(message, message, sizeof(message), xtea_key, iv);
	printf("\nencoded message (CBC mode):");
	hexdump((char *)message, sizeof(message));
	xtea_cbc_decrypt(message, message, sizeof(message), xtea_key, iv);
	printf("\ndecoded message (CBC mode):");
	hexdump((char *)message, sizeof(message));

	xtea_ctr_crypt(message, message, sizeof(message), xtea_key, iv);
	printf("\nencoded message (CTR mode):");
	hexdump((char *)message, sizeof(message));
	xtea_ctr_crypt(message, message, sizeof(message), xtea_key, iv);
	printf("\ndecoded message (CTR mode):");
	hexdump((char *)message, sizeof(message));
	
	return 0;
}
