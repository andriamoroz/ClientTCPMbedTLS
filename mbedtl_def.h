#pragma once
#include <cstdint>
#ifndef MBEDTLS_DEF_H
#define MBEDTLS_DEF_H


#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for error constants. */
#define ERR_OK          0   /* No error, everything OK. */
#define ERR_MEM        -1   /* Out of memory error.     */
#define ERR_BUF        -2   /* Buffer error.            */
#define ERR_TIMEOUT    -3   /* Timeout.                 */
#define ERR_ARG        -4	/* Illegal argument.        */
#define ERR_VAL        -5   /* Illegal value.           */
#define ERR_FATAL      -6   /* Fatal error.	            */

#define AUTH_ALGO_INVAL			0
#define AUTH_ALGO_MD5			1
#define AUTH_ALGO_SHA			2

#define PRIV_ALGO_INVAL			0
#define PRIV_ALGO_DES			1
#define PRIV_ALGO_AES			2

#define PRIV_MODE_DECRYPT		0
#define PRIV_MODE_ENCRYPT		1

/* According to RFC 3411 */
#define MAX_ENGINE_ID_LENGTH	32
#define MAX_USER_LENGTH			32
#define MAX_PASSWORD_LENGTH		32

#define MAX_AUTH_PARAM_LENGTH	20
#define MAX_PRIV_PARAM_LENGTH	8

#define AUTH_FLAG				0x01
#define PRIV_FLAG				0x02
#define REPORT_FLAG				0x04

#define MD5_LEN					16
#define SHA_LEN					20

#define LWIP_ARRAYSIZE(x)		(sizeof(x)/sizeof((x)[0]))

typedef uint8_t err_t;

struct pbuf {
    /** next pbuf in singly linked pbuf chain */
    struct pbuf *next;
    /** pointer to the actual data in the buffer */
    uint8_t *payload;
    /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
    uint32_t tot_len;
    /** length of this buffer */
    uint32_t len;
    /** pbuf_type as u8_t instead of enum to save space */
    uint8_t /*pbuf_type*/ type;
    /** misc flags */
    uint8_t flags;
    /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
    uint16_t ref;
};

typedef struct {
    uint8_t user_name[MAX_USER_LENGTH];
    uint8_t engine_id[MAX_ENGINE_ID_LENGTH];
    uint8_t context_name[MAX_ENGINE_ID_LENGTH];
    int8_t security_level;
    uint8_t auth_protocol;
    uint8_t priv_protocol;
    uint8_t auth_password[MAX_PASSWORD_LENGTH];
    uint8_t priv_password[MAX_PASSWORD_LENGTH];
} CryptoSet_t;

typedef struct {
    uint16_t Size;
    //uint8_t AppKey[MAX_AUTH_PARAM_LENGTH];
    uint8_t Data[1024];
} Packet_t;

err_t fw_auth(struct pbuf *stream, const uint8_t* key, uint8_t algo, uint8_t* hmac_out);
err_t fw_crypt(struct pbuf* stream, uint16_t ofs, const uint8_t* key, const uint8_t* priv_param,
               const uint32_t engine_boots, const uint32_t engine_time, uint8_t algo, uint8_t mode);
err_t build_priv_param(uint8_t* priv_param);

void fw_password_to_key_sha(const uint8_t *password, uint8_t passwordlen, const uint8_t *engineID, uint8_t engineLength, uint8_t *key);
void fw_password_to_key_md5(const uint8_t *password, uint8_t passwordlen, const uint8_t *engineID, uint8_t engineLength, uint8_t *key);

err_t prGetUser(const char* username, uint8_t *auth_algo, uint8_t *auth_key, uint8_t *priv_algo, uint8_t *priv_key, CryptoSet_t *CryptoSet);
err_t prEncryptionProc(Packet_t *Packet, uint8_t auth_algo, uint8_t *auth_key, uint8_t priv_algo,
                       uint8_t *priv_key, CryptoSet_t *CryptoSet);
err_t prDecryptionProc(Packet_t *Packet, uint8_t auth_algo, uint8_t *auth_key, uint8_t priv_algo,
                       uint8_t *priv_key, CryptoSet_t *CryptoSet);
#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_DEF_H */
