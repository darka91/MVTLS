/**
 *	SSL/TLS Project
 *	\file HandshakeConstants.h
 *
 * 	This file contains a set of constants used in
 * 	the handshake protocol and function implementation
 *	to manage the supported cipher suite.
 *
 *	\date Created on 24/12/15.
 *	\copyright Copyright © 2015 Alessandro Melloni, Andrea Francesco Vinci. All rights reserved.
 *
 */

#ifndef HandshakeConstants_h
#define HandshakeConstants_h

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/hmac.h>

/**	\def REV16
 *	Rotational byte for uint16
 */
#define REV16(value)({(value & 0x00FFU) << 8 | (value & 0xFF00U) >> 8;})

/**	\def REV32
 *	Rotational byte for uint32
 */
#define REV32(value)({(value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |(value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;})

/**
 * \enum TLS_version
 * Identify the TLS version
 */
typedef enum{
    /**SSL 3*/
    SSL3_0 = 0x0300,
    /**TLS 1.0*/
    TLS1_0 = 0x0301,
    /**TLS 1.1*/
    TLS1_1 = 0x0302,
    /**TLS 1.2*/
    TLS1_2 = 0x0303
}TLS_version;

/**
 * \enum key_exchange_algorithm
 * Different types of key exchange algorithm
 */
typedef enum{
    /** No algorithm */
    NONE_KX = -1,
    /** RSA algorithm */
    RSA_KX = 1,
    /** Ephemeral Diffie Hellman*/
    DHE_KX = 2,
    /** Ephemeral Diffie Hellman over elliptic curve*/
    ECDHE_KX = 3
}key_exchange_algorithm;

/**
 *	\enum authentication_algorithm
 *	Different types of authentication algorithm
 */
typedef enum{
    /** No algorithm*/
    NONE_AU = -1,
    /** RSA signature */
    RSA_AU = 1,
    /** DSA signature */
    DSS_AU = 2,
    /** ECDSA signature */
    ECDSA_AU = 3
}authentication_algorithm;

/**
 *	\enum hash_algorithm
 *	Different types of hash algorithm
 */
typedef enum{
    /** No hash */
    NONE_H = 0,
    /** MD5 */
    MD5_H = 1,
    /** SHA 1*/
    SHA1_H = 2,
    /** SHA 224 */
    SHA224_H = 3,
    /** SHA 256 */
    SHA256_H = 4,
    /** SHA 384 */
    SHA384_H = 5,
    /** SHA 512 */
    SHA512_H = 6
}hash_algorithm;

/**
 *	\enum channel_mode
 *	Enum used to set the packet mode
 *	e.g. server_hello, client_hello
 */
typedef enum{
    SERVER_MODE,
    CLIENT_MODE
}channel_mode;

/**
 *	Types of handshake message
 */
enum {
	HELLO_REQUEST		= 0x00,
	CLIENT_HELLO			= 0x01,
	SERVER_HELLO			= 0x02,
	CERTIFICATE				= 0x0B,
	SERVER_KEY_EXCHANGE= 0x0C,
	CERTIFICATE_REQUEST	= 0x0D,
	SERVER_DONE			= 0x0E,
	CERTIFICATE_VERIFY	= 0x0F,
	CLIENT_KEY_EXCHANGE	= 0x10,
	FINISHED					= 0x14
};

/**
 * \struct cipher_suite_t
 *	This contains a single cipher suite
 *	with details about itself.
 */
typedef struct{
	/** Cipher suite id*/
	uint16_t cipher_id;
	/** Cipher suite name */
	char *name;
	/** Key exchange algorithm used*/
	key_exchange_algorithm kx;
	/** Authentication algorithm*/
	authentication_algorithm au;
	/** The key size for the symmetric cryptography */
	uint16_t key_size;
	/** Hash algorithm */
	hash_algorithm hash;
}cipher_suite_t;


/** The number of supported cipher suites */
extern const int NUM_CIPHER_SUITE;

/** \struct connection_parameters
 *	This struct contains all details about connection.
 *	It also contains data to complete the handshake.
 */
typedef struct{

	/** The TLS version*/
	uint16_t tls_version;

	/** Store the previous state in the handshake*/
	uint16_t previous_state;

	/** The cipher suite used chosen in the handshake*/
	cipher_suite_t cipher_suite;

	/** Client random, include the UNIX time stamp */
	unsigned char client_random[32];

	/** Server random, include the UNIX time stamp */
	unsigned char server_random[32];

	/** Server key exchange message */
	void *server_key_ex;

	/** Session master secret */
	unsigned char *master_secret;

	/** Master secret length */
	int master_secret_len;

	/** The backup of handshake messages exchanged during handshake*/
	unsigned char *handshake_messages;

	/** Backup stream length */
	int handshake_messages_len;

	/** The server certificate */
	X509 *server_certificate;

	/** The private key for the key exchange */
	BIGNUM *private_key;

}handshake_parameters_t;

/**
 * \struct handshake_t
 * Handshake protocol struct.
 * Model fields of handshake messages.
 */
typedef struct{
	/** Handshake type:
	HELLO_REQUEST(0x00), CLIENT_HELLO(0x01), SERVER_HELLO(0x02),
	CERTIFICATE(0x0B), SERVER_KEY_EXCHANGE(0x0C), CERTIFICATE_REQUEST(0x0D),
	SERVER_DONE(0x0E), CERTIFICATE_VERIFY(0x0F), CLIENT_KEY_EXCHANGE(0x10),
	FINISHED(0x14)
	*/
	uint8_t type;

	/** Message length*/
	uint32_t length;

	/** Handshake binary message */
	unsigned char *message;
}handshake_t;

/**
 * Get the hash algorithm starting from the hash id.
 *
 *	\param h: the hash algorithm used
 *	\return an EVP_MD struct used to compute digest
 */
const EVP_MD *get_hash_function(hash_algorithm h);

/**
 * Given the id of a cipher suite return the cipher suite struct.
 *
 *	\param id: cipher suite id
 *	\return cipher suite struct with id id
 */
cipher_suite_t get_cipher_suite_by_id(uint16_t id);

/**
 * Given the cipher suite name return the cipher suite struct.
 *
 *	\param name: cipher suite name
 *	\return cipher suite struct with name name
 */
cipher_suite_t get_cipher_suite_by_name(char *name);

/**
 * Fill array[] with all cipher suites that has key exchange kx,
 * hash algorithm h and authentication algorithm au.
 *
 *
 *	\param kx: the key exchange algorithm (if NONE_KX the function consider all key exchange algorithms)
 *	\param h: the hash algorithm (if NONE_H the function consider all hash algorithms)
 * 	\param au: the authentication algorithm (if NONE_AU the function consider all authentication algorithms)
 *	\param array[]: an empty array of size NUM_CIPHER_SUITE.
 *	\return the number of cipher suites loaded in array[]
 */
int get_cipher_suites(key_exchange_algorithm kx, hash_algorithm h, authentication_algorithm au, cipher_suite_t array[]);

#endif
