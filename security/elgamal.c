#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <err.h>
#include "elgamal.h"
#include "tools.h"
#include "huffman.h"

#define MIN_GAMAL 511
#define MAX_GAMAL 512

// Key generators.
void large_keygen(mpz_t lower, mpz_t upper, mpz_t res)
{
    // Create random number generator, initialize and seed.
    gmp_randstate_t state;
    gmp_randinit_mt(state);

    gmp_randseed_ui(state, time(NULL));

    // Initialise and generate random number.
    mpz_t tmp;
    mpz_init(tmp);

    // Ensure that result is between lower and upper bounds.
    mpz_sub(tmp, upper, lower);
    // gmp_printf("Upper - lower: %Zd\n", tmp);
    mpz_urandomm(res, state, tmp);
    mpz_add(res, res, lower);

    // Free memory.
    mpz_clear(tmp);
    gmp_randclear(state);

}

void coprime_key(mpz_t q, mpz_t res)
{
    // Initialize and set GMP numbers.
    mpz_t low, tmp;
    mpz_init(low);
    mpz_init(tmp);
    mpz_ui_pow_ui(low, 2, MIN_GAMAL);

    large_keygen(low, q, res);
    // mpz_set(res, large_keygen(low, q));
    mpz_gcd(tmp, res, q);

    // Check if values are coprime.
    while (mpz_cmp_ui(tmp, 1) != 0)
    {
        large_keygen(low, q, res);
        mpz_gcd(tmp, res, q);
    }

    // Free memory.
    mpz_clear(low);
    mpz_clear(tmp);

}

// Encryption and decryption.
void encrypt_gamal(char *msg, publicKey *receiverKeys, cyphers *en_data)
{
    // Number variables.
    mpz_t q, g, h, k, s, p;

    // Public keys.
    mpz_init(q);
    mpz_init(g);
    mpz_init(h);
    
    string_largenum(receiverKeys -> q, q);
    string_largenum(receiverKeys -> g, g);
    string_largenum(receiverKeys -> h, h);

    // Encrypted message initialization.
    size_t len = strlen(msg);
    en_data->size = len;
    mpz_t encryption[len];

    // Generate sender's private key.
    mpz_init(k);
    mpz_init(s);
    mpz_init(p);
    
    coprime_key(q, k);

    mpz_powm(s, h, k, q);
    mpz_powm(p, g, k, q);
    
    en_data -> p = largenum_string(p);

    // Initialize temporary large number.
    mpz_t tmpLarge;
    mpz_init(tmpLarge);

    for (size_t i = 0; i < len; i++)
    {
        int tmp = (int) msg[i];

        // Set temporary large number.
        mpz_mul_ui(tmpLarge, s, tmp);

        mpz_init(encryption[i]);
        mpz_set(encryption[i], tmpLarge);
    }

    // Save string conversion of encryption.
    en_data->en_msg = toString(encryption, len);

    // Free memory.
    for (size_t i = 0; i < len; i++)
        mpz_clear(encryption[i]);

    mpz_clear(q);
    mpz_clear(g);
    mpz_clear(h);
    mpz_clear(k);
    mpz_clear(s);
    mpz_clear(p);
    mpz_clear(tmpLarge);

}

char *decrypt_gamal(cyphers *en_data, privateKey *privkey)
{
    // Convert encrypted message to array of GMP numbers.
    size_t len = en_data->size;
    mpz_t data[len];
    fromString(en_data->en_msg, len, data);

    // Decrypted message initialisation.
    char *res = malloc((len+1) * sizeof(char));

    // Initialize GMP integers.
    mpz_t p, key, q, h;
    mpz_init(p);
    mpz_init(key);
    mpz_init(q);
    mpz_init(h);
    
    string_largenum(en_data->p, p);
    mpz_set(key, privkey->a);
    mpz_set(q, privkey->q);

    mpz_powm(h, p, key, q);

    mpz_t tmp_mpz;
    mpz_init(tmp_mpz);
    for (size_t i = 0; i < len; i++)
    {
        // Added the != 0 verification: SERGIO
        if (mpz_sgn(data[i]))
        {
            mpz_tdiv_q(tmp_mpz, data[i], h);

            char tmp = (char) (mpz_get_ui(tmp_mpz));
            res[i] = tmp;
        }
        // Clear data at current index.
        mpz_clear(data[i]);
    }

    // Null terminate result.
    res[len] = '\0';

    // Free data.
    mpz_clear(p);
    mpz_clear(key);
    mpz_clear(q);
    mpz_clear(h);
    mpz_clear(tmp_mpz);

    return res;
}

// Public and private keys generation for receiver.
void generateKeys(publicKey *pubKey, privateKey *privKey)
{
    // Initialize GMP integers.
    mpz_t q;
    mpz_t g;
    mpz_t key;
    mpz_t h;

    mpz_init(q);
    mpz_init(g);
    mpz_init(key);
    mpz_init(h);

    // Receiver's public keys: q, g and h.
    mpz_t low1, low2, high;
    mpz_init(low1);
    mpz_init(low2);
    mpz_init(high);
    mpz_ui_pow_ui(low1, 2, MIN_GAMAL);
    mpz_ui_pow_ui(high, 2, MAX_GAMAL);
    mpz_set_ui(low2, 2);

    large_keygen(low1, high, q);
    large_keygen(low2, q, g);

    // Receiver's private key.
    coprime_key(q, key);

    mpz_powm(h, g, key, q);

    // Save receiver keys to structs.
    pubKey -> q = largenum_string(q);
    pubKey -> g = largenum_string(g);
    pubKey -> h = largenum_string(h);
    
    mpz_init(privKey -> a);
    mpz_init(privKey -> q);
    mpz_set(privKey -> a, key);
    mpz_set(privKey -> q, q);
    
    // Free GMP integers.
    mpz_clear(q);
    mpz_clear(g);
    mpz_clear(key);
    mpz_clear(h);
    mpz_clear(low1);
    mpz_clear(low2);
    mpz_clear(high);

}

// Free encrypted data memory.
void freeCyphers(cyphers *data)
{
    free(data->en_msg);
    free(data->p);

    // Free entire struct.
    free(data);
}

void freeKeys(publicKey *pubkey, privateKey *privkey)
{
    // Free public key.
    free(pubkey -> q);
    free(pubkey -> g);
    free(pubkey -> h);
    free(pubkey);
    
    // Free private key.
    mpz_clear(privkey -> a);
    mpz_clear(privkey -> q);
    free(privkey);
}

void freePriv(privateKey *privkey)
{
    // Free private key.
    mpz_clear(privkey -> a);
    mpz_clear(privkey -> q);
    free(privkey);
}

void freePub(publicKey *pubkey)
{
    // Free public key.
    free(pubkey -> q);
    free(pubkey -> g);
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

void compressElgamal(char *input)
{
    unsigned char *encTree, *encData;
    int treeOffset, dataOffset;
    size_t len = strlen(input);

    printf("To compress: %s\n", input);
    printf("Len: %ld\n", len);
    compress(input, &encTree, &encData, &treeOffset, &dataOffset);

    size_t compressedLen = 0;
    while (encTree[compressedLen] != '\0')
        compressedLen++;
    for (size_t i = 0; encData[i] != '\0'; i++)
        compressedLen++;

    printf("Compressed Tree:\n %s END\n", encTree);
    printf("Compressed Data:\n %s END\n", encData);
    printf("Compressed len: %ld\n", compressedLen);
    
    printf("\nFinished compression\n");

    // Decompression.
    char *res = decompress(encData, dataOffset, encTree, treeOffset);

    printf("\nDecompressed: %s\n", res);

    // Ratio.
    double ratio = (float) len / (float) compressedLen;
    printf("\nConversion ratio: %f\n", ratio);
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

    // char *msg = "Bought a spaceship now I'm a space cadet.";
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

    // Compress encryption with Huffman.
    printf("\nCompressing data.\n");
    compressElgamal(dataCyphers->en_msg);

    // Decrypt data and print.
    char *dr_msg = decrypt_gamal(dataCyphers, receiver_privkey);
    printf("\nDecrypted message: %s\n", dr_msg);

    // Print public and private keys.
    printf("\ng -> %s\n", receiver_pubkey->g);
    printf("q -> %s\n", receiver_pubkey->q);
    printf("h -> %s\n", receiver_pubkey->h);

    // Private key.
    char *a = largenum_string(receiver_privkey->a);
    printf("a -> %s\n", a);

    // Free memory space.
    free(dr_msg);
    freeKeys(receiver_pubkey, receiver_privkey);
    // freeKey(receiver_privkey);
    // free(receiver_privkey);
    freeCyphers(dataCyphers);
    free(a);

    return 0;

}
*/
