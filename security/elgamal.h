#ifndef ELGAMAL_H
#define ELGAMAL_H

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

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
    uint128_t a;
    uint128_t q;

} privateKey;

// Structure for Elgamal encrypted message.
typedef struct cyphers 
{
    char *en_msg;
    char *p;
    size_t size;
    
} cyphers;

// Tools for key generation.
uint128_t gcd(uint128_t a, uint128_t b);
uint128_t large_keygen(uint128_t lower, uint128_t upper);
uint128_t coprime_key(double q);
uint128_t mod_power(uint128_t a, uint128_t b, uint128_t m);

// Tools to convert between strings and public key structures
char * pubtoString(publicKey* key);
publicKey* stringtoPub(char* string);


// Encryption and decryption.
void encrypt_gamal(char *msg, publicKey *receiverKeys, cyphers *en_data);
char *decrypt_gamal(cyphers *en_data, privateKey *privkey);

#endif
