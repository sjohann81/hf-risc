/****************************************************************************
Classification: U//FOUO
*//**
	@file   Aes.c
	@brief  Handles AES computation.
	@author Andrew Gorczyca
	@date   2012/5/25

	Based on axtls implementation, by Cameron Rich. Improvements made mostly
	to cbc handling, reducing unnecessary copying and intermediary buffers.
	Added code to generate both Rijndael S-box lookups in memory. Reduced
	Round constants to ones actally used. mul2 from Dr. Gladman. I've only 
	personally tested this on little Endian machines, but it might 
	*possibly* work on a big endian platform.
****************************************************************************/


//#include <stdio.h>
//#include <stdint.h>
//#include <string.h>
#include <hf-risc.h>
#include "aes.h"

#define rot1(x) (((x) << 24) | ((x) >> 8))
#define rot2(x) (((x) << 16) | ((x) >> 16))
#define rot3(x) (((x) <<  8) | ((x) >> 24))

#define mt  0x80808080
#define mh  0xfefefefe
#define mm  0x1b1b1b1b
#define mul2(x,t)	((t)=((x)&mt), ((((x)+(x))&mh)^(((t)-((t)>>7))&mm)))

#define inv_mix_col(x,f2,f4,f8,f9) (\
			(f2)=mul2(x,f2), \
			(f4)=mul2(f2,f4), \
			(f8)=mul2(f4,f8), \
			(f9)=(x)^(f8), \
			(f8)=((f2)^(f4)^(f8)), \
			(f2)^=(f9), \
			(f4)^=(f9), \
			(f8)^=rot3(f2), \
			(f8)^=rot2(f4), \
			(f8)^rot1(f9))

static uint8_t aes_sbox[256];		/** AES S-box  */
static uint8_t aes_isbox[256];	/** AES iS-box */

/** AES round constants */
static const uint8_t Rcon[]= 
{
	0x01,0x02,0x04,0x08,0x10,0x20,
	0x40,0x80,0x1b,0x36,0x6c,0xd8
};

/* ----- static functions ----- */
static void AES_encrypt(const AES_CTX *ctx, uint32_t *data);
static void AES_decrypt(const AES_CTX *ctx, uint32_t *data);
static void AES_hncpy32(uint32_t * dst, uint32_t * src);
static uint8_t AES_xtime(uint32_t x);
static AES_display(uint8_t * data,size_t len);
static void AES_generateSBox(void);

/* Perform doubling in Galois Field GF(2^8) using the irreducible polynomial
   x^8+x^4+x^3+x+1 */
static uint8_t AES_xtime(uint32_t x)
{
	return (x&0x80) ? (x<<1)^0x1b : x<<1;
}

/** Set up AES with the key/iv and cipher size. */
void AES_set_key(AES_CTX *ctx, const uint8_t *key, const uint8_t *iv, AES_MODE mode)
{
	int i, words;
	uint32_t tmp, tmp2;
	const uint8_t * rtmp = Rcon;

	switch (mode)
	{
	case AES_MODE_128:
		ctx->rounds = 10;
		words = 4;
		break;
	case AES_MODE_256:
		ctx->rounds = 14;
		words = 8;
		AES_hncpy32((uint32_t *)((ctx->ks)+4), (uint32_t *)(key + AES_BLOCKSIZE));
		break;
	default:        /* fail silently */
		return;
	}

	AES_hncpy32((uint32_t *)(ctx->ks), (uint32_t *)key);
	ctx->key_size = words;
	for (i = words; i <  4 * (ctx->rounds+1); i++)
	{
		tmp = ctx->ks[i-1];

		if ((i % words) == 0)
		{
			tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]<< 8;
			tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<<16;
			tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<24;
			tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ];
			tmp=tmp2^(((uint32_t)*rtmp)<<24);
			rtmp++;
		}

		if ((words == 8) && ((i % words) == 4))
		{
			tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]    ;
			tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<< 8;
			tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<16;
			tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ]<<24;
			tmp=tmp2;
		}

		ctx->ks[i]=ctx->ks[i-words]^tmp;
	}

	/* copy the iv across */
	memcpy(ctx->iv, iv, AES_BLOCKSIZE);
}

/** Change a key for decryption. */
void AES_convert_key(AES_CTX *ctx)
{
	int i;
	uint32_t *k,w,t1,t2,t3,t4;

	k = ctx->ks;
	k += 4;

	for (i= ctx->rounds*4; i > 4; i--)
	{
		w= *k;
		w = inv_mix_col(w,t1,t2,t3,t4);
		*k++ =w;
	}
}

static void AES_hncpy32(uint32_t * dst, uint32_t * src)
{
	dst[0] = htonl(src[0]);
	dst[1] = htonl(src[1]);
	dst[2] = htonl(src[2]);
	dst[3] = htonl(src[3]);
}

/** Encrypt a byte sequence (with a block size 16) using the AES cipher. */
void AES_cbc_encrypt(AES_CTX *ctx, const uint8_t *msg, uint8_t *out, int length)
{
	uint32_t buf[4];

	AES_hncpy32(buf,(uint32_t *)(ctx->iv));
	for (length -= AES_BLOCKSIZE; length >= 0; length -= AES_BLOCKSIZE)
	{
	
		buf[0] = ntohl(((uint32_t *)msg)[0])^buf[0];
		buf[1] = ntohl(((uint32_t *)msg)[1])^buf[1];
		buf[2] = ntohl(((uint32_t *)msg)[2])^buf[2];
		buf[3] = ntohl(((uint32_t *)msg)[3])^buf[3];

		AES_encrypt(ctx, buf);

		AES_hncpy32((uint32_t *)out,buf);
		
		msg += AES_BLOCKSIZE;
		out += AES_BLOCKSIZE;
    }
	out-=AES_BLOCKSIZE;

	memcpy(ctx->iv,out,AES_BLOCKSIZE);
}

/** Decrypt a byte sequence (with a block size 16) using the AES cipher. */
void AES_cbc_decrypt(AES_CTX *ctx, const uint8_t *msg, uint8_t *out, int length)
{
	uint32_t tin[4], xor[4], buf[4];
	AES_hncpy32(xor,(uint32_t *)(ctx->iv));
	for (length -= AES_BLOCKSIZE; length >= 0; length -= AES_BLOCKSIZE)
	{
		AES_hncpy32(tin,(uint32_t *)msg);
		memcpy(buf,tin,AES_BLOCKSIZE);
		AES_decrypt(ctx, buf);
		((uint32_t *)out)[0] = htonl(buf[0]^xor[0]);
		((uint32_t *)out)[1] = htonl(buf[1]^xor[1]);
		((uint32_t *)out)[2] = htonl(buf[2]^xor[2]);
		((uint32_t *)out)[3] = htonl(buf[3]^xor[3]);
		memcpy(xor,tin,AES_BLOCKSIZE);

		msg += AES_BLOCKSIZE;
		out += AES_BLOCKSIZE;
	}

	AES_hncpy32((uint32_t *)(ctx->iv),tin);
}

/** Encrypt a single block (16 bytes) of data */
static void AES_encrypt(const AES_CTX *ctx, uint32_t *data)
{
    /* To make this code smaller, generate the sbox entries on the fly.
     * This will have a really heavy effect upon performance.
     */
    uint32_t tmp[4];
    uint32_t tmp1, old_a0, a0, a1, a2, a3, row;
    int curr_rnd;
    int rounds = ctx->rounds; 
    const uint32_t *k = ctx->ks;

    /* Pre-round key addition */
    for (row = 0; row < 4; row++)
        data[row] ^= *(k++);

    /* Encrypt one block. */
    for (curr_rnd = 0; curr_rnd < rounds; curr_rnd++)
    {
        /* Perform ByteSub and ShiftRow operations together */
        for (row = 0; row < 4; row++)
        {
            a0 = (uint32_t)aes_sbox[(data[row%4]>>24)&0xFF];
            a1 = (uint32_t)aes_sbox[(data[(row+1)%4]>>16)&0xFF];
            a2 = (uint32_t)aes_sbox[(data[(row+2)%4]>>8)&0xFF]; 
            a3 = (uint32_t)aes_sbox[(data[(row+3)%4])&0xFF];

            /* Perform MixColumn iff not last round */
            if (curr_rnd < (rounds - 1))
            {
                tmp1 = a0 ^ a1 ^ a2 ^ a3;
                old_a0 = a0;
                a0 ^= tmp1 ^ AES_xtime(a0 ^ a1);
                a1 ^= tmp1 ^ AES_xtime(a1 ^ a2);
                a2 ^= tmp1 ^ AES_xtime(a2 ^ a3);
                a3 ^= tmp1 ^ AES_xtime(a3 ^ old_a0);
            }

            tmp[row] = ((a0 << 24) | (a1 << 16) | (a2 << 8) | a3);
        }

        /* KeyAddition - note that it is vital that this loop is separate from
           the MixColumn operation, which must be atomic...*/ 
        for (row = 0; row < 4; row++)
            data[row] = tmp[row] ^ *(k++);
    }
}

/** Decrypt a single block (16 bytes) of data */
static void AES_decrypt(const AES_CTX *ctx, uint32_t *data)
{ 
    uint32_t tmp[4];
    uint32_t xt0,xt1,xt2,xt3,xt4,xt5,xt6;
    uint32_t a0, a1, a2, a3, row;
    int curr_rnd;
    int rounds = ctx->rounds;
    const uint32_t *k = ctx->ks + ((rounds+1)*4);

    /* pre-round key addition */
    for (row=4; row > 0;row--)
        data[row-1] ^= *(--k);

    /* Decrypt one block */
	for (curr_rnd = 0; curr_rnd < rounds; curr_rnd++)
	{
		/* Perform ByteSub and ShiftRow operations together */
		for (row = 4; row > 0; row--)
		{
			a0 = aes_isbox[(data[(row+3)%4]>>24)&0xFF];
			a1 = aes_isbox[(data[(row+2)%4]>>16)&0xFF];
			a2 = aes_isbox[(data[(row+1)%4]>>8)&0xFF];
			a3 = aes_isbox[(data[row%4])&0xFF];

			/* Perform MixColumn iff not last round */
			if (curr_rnd<(rounds-1))
			{
				/* The MDS cofefficients (0x09, 0x0B, 0x0D, 0x0E)
					are quite large compared to encryption; this 
					operation slows decryption down noticeably. */
				xt0 = AES_xtime(a0^a1);
				xt1 = AES_xtime(a1^a2);
				xt2 = AES_xtime(a2^a3);
				xt3 = AES_xtime(a3^a0);
				xt4 = AES_xtime(xt0^xt1);
				xt5 = AES_xtime(xt1^xt2);
				xt6 = AES_xtime(xt4^xt5);

				xt0 ^= a1^a2^a3^xt4^xt6;
				xt1 ^= a0^a2^a3^xt5^xt6;
				xt2 ^= a0^a1^a3^xt4^xt6;
				xt3 ^= a0^a1^a2^xt5^xt6;
				tmp[row-1] = ((xt0<<24)|(xt1<<16)|(xt2<<8)|xt3);
			}
			else
				tmp[row-1] = ((a0<<24)|(a1<<16)|(a2<<8)|a3);
		}
		for (row = 4; row > 0; row--)
			data[row-1] = tmp[row-1] ^ *(--k);
	}
}

void AES_generateSBox(void)
{
	uint32_t t[256], i;
	uint32_t x;
	for (i = 0, x = 1; i < 256; i ++)
	{
		t[i] = x;
		x ^= (x << 1) ^ ((x >> 7) * 0x11B);
	}

	aes_sbox[0]	= 0x63;
	for (i = 0; i < 255; i ++)
	{
		x = t[255 - i];
		x |= x << 8;
		x ^= (x >> 4) ^ (x >> 5) ^ (x >> 6) ^ (x >> 7);
		aes_sbox[t[i]] = (x ^ 0x63) & 0xFF;
	}
	for (i = 0; i < 256;i++)
	{
		aes_isbox[aes_sbox[i]]=i;
	}
}


static AES_display(uint8_t * data, size_t len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		printf("%02x",(uint8_t)data[i]);
	}
	
}


int main(void)
{
	int i;
	AES_CTX	ctx;
	/* NIST SP800-38A: CBC Example Vector F2.1-2 & F2.5-6
		http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf */
	uint8_t key128[] =	{0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
							 	 0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
	uint8_t key256[] = 	{0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
			  					 0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
								 0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
								 0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4};
	uint8_t iv[] =			{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
								 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
	uint8_t nistData[] = {0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
			  					 0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
								 0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,
								 0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,
								 0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,
								 0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,
								 0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,
								 0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10};
	uint8_t crypt[64] = {0};
	uint8_t decode[64] = {0};

	AES_generateSBox();

	/********** Nist dataset test 128bit **********/
	printf("F.2.1  CBC-AES128.Encrypt\n");
	AES_set_key(&ctx, key128, iv, AES_MODE_128);
	printf("Key:\t\t");	AES_display(key128,sizeof(key128));
	printf("\nIV:\t\t");	AES_display(iv,sizeof(iv));

	AES_cbc_encrypt(&ctx, nistData, crypt, AES_BLOCKSIZE * 4);
		
	printf("\nPlainText: \t"); AES_display(nistData,20);
	printf("\nCypherText: \t"); AES_display(crypt,20);

	AES_set_key(&ctx, key128, iv, AES_MODE_128);
	AES_convert_key(&ctx);

	AES_cbc_decrypt(&ctx, crypt, decode, AES_BLOCKSIZE * 4);

	printf("\nDecodeText: \t"); AES_display(decode,20);

	/********** Nist dataset test 256bit **********/
	printf("\nF.2.5  CBC-AES256.Encrypt\n");
	AES_set_key(&ctx, key256, iv, AES_MODE_256);
	printf("Key:\t\t");	AES_display(key256,20);
	printf("\nIV:\t\t");	AES_display(iv,sizeof(iv));

	AES_cbc_encrypt(&ctx, nistData, crypt, AES_BLOCKSIZE * 4);
		
	printf("\nPlainText: \t"); AES_display(nistData,20);
	printf("\nCypherText: \t"); AES_display(crypt,20);

	AES_set_key(&ctx, key256, iv, AES_MODE_256);
	AES_convert_key(&ctx);

	AES_cbc_decrypt(&ctx, crypt, decode, AES_BLOCKSIZE * 4);

	printf("\nDecodeText: \t"); AES_display(decode,20);
	printf("\n");
}
