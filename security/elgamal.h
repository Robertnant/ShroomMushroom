#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <gmp.h>

// Structures for Elgamal keys.
// (Public key is saved as strings for JSON purposes).
typedef struct publicKey
{
    char* g;
    char* q;
    char* h;

} publicKey;

typedef struct privateKey
{
    // Public key q is added for easier decryption.
    mpz_t a;
    mpz_t q;

} privateKey;

// Structure for Elgamal encrypted message.
typedef struct cyphers 
{
    char *en_msg;
    char *p;
    size_t size;
    
} cyphers;

// Tools for key generation.
void large_keygen(mpz_t lower, mpz_t upper, mpz_t res);
void coprime_key(mpz_t q, mpz_t res);

// Tools to convert between strings and public key structures
char * pubtoString(publicKey* key);
publicKey* stringtoPub(char* string);

// Tool to free private key.
void freeKey(privateKey *privkey);

// Key generation
void generateKeys(publicKey *pubKey, privateKey *privKey);

// Encryption and decryption.
void encrypt_gamal(char *msg, publicKey *receiverKeys, cyphers *en_data);
char *decrypt_gamal(cyphers *en_data, privateKey *privkey);

#endif
