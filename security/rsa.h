#ifndef RSA_H
#define RSA_H

// Structure of RSA cryptosystem keys.
typedef struct publicKey
{
    // Modulus for public and private keys.
    size_t n;

    // Encryption key.
    size_t e;

} publicKey;

typedef struct privateKey
{
    // Modulus for public and private keys.
    size_t n;

    // Decryption key.
    size_t d;

} privateKey;

// Main RSA structure.
typedef struct rkeys
{
    // Public key.
    publicKey *pubkey;

    // Private key.
    privateKey *privkey;

} rkeys;


// Keys generation functions.
size_t generate_keys_rsa(size_t p, size_t q);
size_t generate_e(size_t t);
size_t generate_d(size_t e, size_t phi);
size_t gcd_extended(size_t a, size_t m, size_t *x, size_t *y);
