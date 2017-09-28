/****************************************************************************
Classification: U//FOUO
*//**
    @file   Aes.h
    @brief  Header for Aes.c
****************************************************************************/

#define AES_MAXROUNDS   14  /**< @brief max potential rounds */
#define AES_BLOCKSIZE   16  /**< @brief Blocksize of aes encryption - 128bit */
#define AES_IV_SIZE     16  /**< @brief Initalization vector size */

#define size_t int32_t

/** @brief Specifies keylength */
typedef enum
{
    AES_MODE_128,
    AES_MODE_256
} AES_MODE;

/** @brief This structure holds the AES key context */
typedef struct aes_key_st 
{
    uint16_t rounds;                    /**< @brief # of rounds - based on keysize */
    uint16_t key_size;                  /**< @brief keysize int bits */
    uint32_t ks[(AES_MAXROUNDS+1)*8];   /**< @brief hold the different keystates */
    uint8_t  iv[AES_IV_SIZE];           /**< @brief holds the initilaization vector */
} AES_CTX;

/****************************************************************************
AES_set_key
*//**
    @brief  Setup the AES key context

    @param[in]  ctx     AES context structure
    @param[in]  key     The buffer containing the key to set
    @param[in]  iv      Initlaization vector data
    @param[in]  mode 	Enum of supported AES keysizes
****************************************************************************/
void AES_set_key(AES_CTX *ctx, const uint8_t *key, const uint8_t *iv, AES_MODE mode);

/****************************************************************************
AES_convert_key
*//**
    @brief  Converts the AES key context from encryption to decryption

    @param[in]  ctx     AES context structure
****************************************************************************/
void AES_convert_key(AES_CTX *ctx);

/****************************************************************************
AES_cbc_encrypt
*//**
    @brief  Performs CBC encryption

    Performs CBC encryption on the msg buffer, output needs to be a valid
    allocated memory location of approporaite length. The output buffer can
    be the same as the input buffer in which the data will be directly 
    encrypted.

    @param[in]  ctx     Initialized AES context structure
    @param[in]  msg     The buffer to perform encryption on
    @param[out] out     The buffer to put the encryption data on
    @param[in]  length  The length of the msguffer in bytes, must be evenly
                        divisible the AES blocksize of 16.
****************************************************************************/
void AES_cbc_encrypt(AES_CTX *ctx, const uint8_t *msg, uint8_t *out, int length);

/****************************************************************************
AES_cbc_decrypt
*//**
    @brief  Performs CBC decryption

    Performs CBC decryption on the msg buffer, output needs to be a valid
    allocated memory location of approporaite length. The output buffer can
    be the same as the input buffer in which the data will be directly 
    decrypted.

    @param[in]  ctx     Initialized AES context structure
    @param[in]  msg     The buffer to perform decryption on
    @param[out] out     The buffer to put the decryption data on
    @param[in]  length  The length of the msguffer in bytes, must be evenly
                        divisible the AES blocksize of 16.
****************************************************************************/
void AES_cbc_decrypt(AES_CTX *ctx, const uint8_t *msg, uint8_t *out, int length);
