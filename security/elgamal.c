#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <err.h>
#include "elgamal.h"
#include "tools.h"

// Extended GCD.
uint128_t gcdExtended(uint128_t a, uint128_t b, uint128_t *x, uint128_t *y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return b;
    }

    uint128_t x1, y1;
    uint128_t gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of recursive
    // call
    *x = y1 - (b/a) * x1;
    *y = x1;

    return gcd;
}

// GCD function.
uint128_t gcd(uint128_t a, uint128_t b)
{
    uint128_t x;
    uint128_t y;
    
    return gcdExtended(a, b, &x, &y); 
}



// Key generators.
uint128_t large_keygen(uint128_t lower, uint128_t upper)
{
    // Randomize value at each function call.
    srand(time(0));

    uint128_t a = (rand() % (upper - lower + 1)) + lower;

    return a;
}

uint128_t coprime_key(uint128_t q)
{
    uint128_t a = large_keygen(pow(10,19), q);

    // Check if values are coprime.
    while (gcd(a, q) != 1)
        a = large_keygen(pow(10,19), q);

    return a;
}

// Modular exponential.
uint128_t mod_power(uint128_t a, uint128_t b, uint128_t m)
{
    // Result initialization.
    uint128_t res = 1;

    a %= m;

    // Check if a divisible by m.
    if (a == 0)
        return 0;

    while (b > 0)
    {
        // Case if b is odd.
        if (b % 2 != 0)
            res = (res * a) % m;

        // Make b even.
        b /= 2;
        a = (a * a) % m;
    }

    return res;
}

// Encryption and decryption.
void encrypt_gamal(char *msg, publicKey *receiverKeys, cyphers *en_data)
{
    // Public keys.
    uint128_t q = string_largenum(receiverKeys -> q);
    uint128_t g = string_largenum(receiverKeys -> g);
    uint128_t h = string_largenum(receiverKeys -> h);

    // Encrypted message initialization.
    size_t len = strlen(msg);
    en_data->size = len;
    uint128_t *encryption = malloc(len * sizeof(uint128_t));

    // Generate sender's private key.
    uint128_t k = coprime_key(q);

    uint128_t s = mod_power(h, k, q);
    uint128_t p = mod_power(g, k, q);
    en_data -> p = largenum_string(p);

    for (size_t i = 0; i < len; i++)
    {
        int tmp = (int) msg[i];
        encryption[i] = s * tmp;
    }

    // Save string conversion of encryption.
    en_data->en_msg = toString(encryption, len);

    // Free encryption array.
    free(encryption);
}

char *decrypt_gamal(cyphers *en_data, privateKey *privkey)
{
    // Convert encrypted message to array of uint128 numbers.
    size_t len = en_data->size;
    uint128_t *data = fromString(en_data->en_msg, len);

    // Decrypted message initialisation.
    char *res = malloc((len+1) * sizeof(char));

    uint128_t p = string_largenum(en_data->p);
    uint128_t key = privkey->a;
    uint128_t q = privkey->q;

    uint128_t h = mod_power(p, key, q);

    for (size_t i = 0; i < len; i++)
    {
        char tmp = (char) ((uint128_t) (data[i]/h));
        res[i] = tmp;
    }

    // Null terminate result.
    res[len] = '\0';

    // Free created array.
    free(data);

    return res;
}

// Public and private keys generation for receiver.
void generateKeys(publicKey *pubKey, privateKey *privKey)
{
    // Receiver's public keys: q, g and h.
    uint128_t q = large_keygen(pow(10,19), pow(10,20));
    uint128_t g = large_keygen(2, q);

    // Receiver's private key.
    uint128_t key = coprime_key(q);

    uint128_t h = mod_power(g, key, q);

    // Save receiver keys to structs.
    pubKey -> q = largenum_string(q);
    pubKey -> g = largenum_string(g);
    pubKey -> h = largenum_string(h);
    privKey -> a = key;
    privKey -> q = q;
}

// Free encrypted data memory.
void freeCyphers(cyphers *data)
{
    free(data->en_msg);
    free(data->p);

    // Free entire struct.
    free(data);
}

void freeKeys(publicKey *pubkey)
{
    free(pubkey -> g);
    free(pubkey -> q);
    free(pubkey -> h);

    free(pubkey);
}

char *pubtoString(publicKey* key)
{
    char *res;
    if (asprintf(&res, "%s-%s-%s", key->g, key->q, key->h) < 0)
        errx(1, "Couldn't allocate for public key");
    return res;
}

publicKey* stringtoPub(char *string)
{
    publicKey* key = (publicKey *) malloc(sizeof(publicKey));
    char *token;

    token = strtok(string, "-");
    key->g = token;
    
    token = strtok(NULL, "-");
    key->q = token;

    token = strtok(NULL, "-");
    key->h = token;

    return key;
}

/*
int main()
{
    char *msg = "Black leather gloves, no sequins\n\
                 Buckles on the jacket, it's Alyx ****\n\
                 Nike crossbody, got a piece in it\n\
                 Got a dance, but it's really on some street ****\n\
                 I'ma show you how to get it\n\
                 It go, right foot up, left foot slide\n\
                 Left foot up, right foot slide\n\
                 Basically, I'm saying either way, we 'bout to slide, ayy\n\
                 Can't let this one slide, ayy";

    printf("Original message: %s\n", msg);

    // Receiver and data structures.
    struct publicKey *receiver_pubkey = malloc(sizeof(struct publicKey));
    struct privateKey *receiver_privkey = malloc(sizeof(struct privateKey));
    struct cyphers *dataCyphers = malloc(sizeof(struct cyphers));
    
    // Generate receiver keys.
    generateKeys(receiver_pubkey, receiver_privkey);

    // Encrypt message and print.
    encrypt_gamal(msg, receiver_pubkey, dataCyphers);
    // printf("\nEncryption: %s\n", dataCyphers -> en_msg);

    // Decrypt data and print.
    char *dr_msg = decrypt_gamal(dataCyphers, receiver_privkey);
    printf("\nDecrypted message: %s\n", dr_msg);

    // Print public and private keys.
    printf("g -> %s\n", receiver_pubkey->g);
    printf("q -> %s\n", receiver_pubkey->q);
    printf("h -> %s\n", receiver_pubkey->h);

    // Private key.
    printf("a -> %s\n", largenum_string(receiver_privkey->a));

    // Free memory space.
    free(dr_msg);
    freeKeys(receiver_pubkey);
    free(receiver_privkey);
    freeCyphers(dataCyphers);

    return 0;

}
*/

