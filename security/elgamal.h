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
    // Public key q is added for easier decryption.
    uint128_t a;
    uint128_t q;

} privateKey;

// Structure for Elgamal encrypted message.
typedef struct cyphers 
{
    char **en_msg;
    uint128_t p;
    size_t size;
    
} cyphers;

// Tools for key generation.
uint128_t gcd(uint128_t a, uint128_t b);
uint128_t large_keygen(uint128_t lower, uint128_t upper);
uint128_t coprime_key(double q);
uint128_t mod_power(uint128_t a, uint128_t b, uint128_t m);

// Encryption and decryption.
void encrypt_gamal(char *msg, publicKey *receiverKeys, cyphers *en_data);
char *decrypt_gamal(cyphers *en_data, privateKey *privkey);

// Tools for uint128 to string conversion and display.
int largenum_len(uint128_t x);
void print_largenum(uint128_t x); 
char *toString(uint128_t *data, size_t len);
char *largenum_string(uint128_t x);

// Tools for uint128 to array of strings conversion.
uint128_t *fromStringArr(char **arr, size_t len);
char **toStringArr(uint128_t *data, size_t len);

#endif
