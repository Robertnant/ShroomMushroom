#ifndef ELGAMAL_H
#define ELGAMAL_H

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

// Structures for Elgamal keys.
typedef struct publicKey
{
    uint128_t g;
    uint128_t q;
    uint128_t h;

} publicKey;

typedef struct privateKey
{
    uint128_t a;

} privateKey;

// Structure for Elgamal encrypted message.
typedef struct cyphers 
{
    uint128_t *en_msg;
    uint128_t p;
    
    // Encrypted message in string.
    char *dataString;

} cyphers;

// Tools for key generation.
uint128_t gcd(uint128_t a, uint128_t b);
uint128_t large_keygen(uint128_t lower, uint128_t upper);
uint128_t coprime_key(double q);
uint128_t mod_power(uint128_t a, uint128_t b, uint128_t m);

// Encryption and decryption.
void encrypt_gamal(char *msg, uint128_t q, uint128_t h, uint128_t g, 
        uint128_t *p, uint128_t *res);
void decrypt_gamal(uint128_t *en_msg, size_t len, uint128_t p, 
        uint128_t key, uint128_t q, char *res);


#endif
