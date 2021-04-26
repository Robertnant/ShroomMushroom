#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <err.h>
#include <gmp.h>
#include "elgamal.h"
#include "tools.h"

// TODO: Create a function to free mpz_t(s) in structures.


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
    mpz_ui_pow_ui(low, 2, 65);

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

// Modular exponential.
/*
mpz_t mod_power(mpz_t a, mpz_t b, mpz_t m)
{
    // Result initialization.
    mpz_t res = 1;

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
*/

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
    // mpz_set(q, string_largenum(receiverKeys -> q));
    // mpz_set(g, string_largenum(receiverKeys -> g));
    // mpz_set(h, string_largenum(receiverKeys -> h));

    // Encrypted message initialization.
    size_t len = strlen(msg);
    en_data->size = len;
    mpz_t encryption[len];
    // TODO: Will probably need to modify sizeof(mpz_t) to sizeof(2**70) 
    // if fails.

    // Generate sender's private key.
    mpz_init(k);
    mpz_init(s);
    mpz_init(p);
    
    coprime_key(q, k);
    // mpz_set(k, coprime_key(q));

    mpz_powm(s, h, k, q);
    mpz_powm(p, g, k, q);
    // mpz_set(s, mod_power(h, k, q));
    // mpz_set(p, mod_power(g, k, q));
    
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
    // mpz_set(h, mod_power(p, key, q));

    mpz_t tmp_mpz;
    mpz_init(tmp_mpz);
    for (size_t i = 0; i < len; i++)
    {
        mpz_tdiv_q(tmp_mpz, data[i], h);

        char tmp = (char) (mpz_get_ui(tmp_mpz));
        res[i] = tmp;

        // Clear data at current index.
        mpz_clear(data[i]);
    }

    // Null terminate result.
    res[len] = '\0';

    // Free data.
    // free(data);
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
    mpz_ui_pow_ui(low1, 2, 65);
    mpz_ui_pow_ui(high, 2, 70);
    mpz_set_ui(low2, 2);

    large_keygen(low1, high, q);
    large_keygen(low2, q, g);

    // mpz_set(q, large_keygen(pow(2,65), pow(2,70)));
    // mpz_set(g, large_keygen(2, q));

    // Receiver's private key.
    coprime_key(q, key);
    gmp_printf("Found coprime key to q: %Zd\n", key);
    // Verify that keys are coprime.
    mpz_t gcd_res;
    mpz_init(gcd_res);
    mpz_gcd(gcd_res, q, key);
    if(mpz_cmp_ui(gcd_res, 1) == 0)
        printf("Keys are coprime.\n");
    mpz_clear(gcd_res);
    // mpz_set(key, coprime_key(q));

    mpz_powm(h, g, key, q);
    // mpz_set(h, mod_power(g, key, q));

    // Save receiver keys to structs.
    pubKey -> q = largenum_string(q);
    pubKey -> g = largenum_string(g);
    pubKey -> h = largenum_string(h);
    
    printf("Saved public keys.\n");
    printf("g -> %s\n", pubKey -> g);
    printf("q -> %s\n", pubKey -> q);
    printf("h -> %s\n", pubKey -> h);

    mpz_init(privKey -> a);
    mpz_init(privKey -> q);
    mpz_set(privKey -> a, key);
    mpz_set(privKey -> q, q);

    printf("Saved private keys.\n");
    gmp_printf("a -> %Zd\n", privKey -> a);
    gmp_printf("q -> %Zd\n", privKey -> q);
    
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
    char *a = largenum_string(receiver_privkey->a);
    printf("a -> %s\n", a);

    // Free memory space.
    free(dr_msg);
    freeKeys(receiver_pubkey);
    free(receiver_privkey);
    freeCyphers(dataCyphers);
    free(a);

    return 0;

}


