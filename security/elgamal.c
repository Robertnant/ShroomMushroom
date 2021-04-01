#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "elgamal.h"

// GCD function.
uint128_t gcd(uint128_t a, uint128_t b)
{
    if (a < b)
        return gcd(b, a);
    
    if (a % b == 0)
        return b;
    
    return gcd(b, a % b);
}

// Key generators.
uint128_t large_keygen(uint128_t lower, uint128_t upper)
{
    // To get a different value at each
    // function call.
    srand(time(0));

    uint128_t a = (rand() % (upper - lower + 1)) + lower;

    return a;
}

uint128_t coprime_key(double q)
{
    uint128_t a = large_keygen(pow(10,20), q);

    // Check if values are coprime.
    while (gcd(a, q) != 1)
        a = large_keygen(pow(10,20), q);

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
    uint128_t q = receiverKeys -> q;
    uint128_t g = receiverKeys -> g;
    uint128_t h = receiverKeys -> h;

    // Encrypted message initialization.
    size_t len = strlen(msg);
    en_data->size = len;
    en_data->en_msg = malloc(len * sizeof(uint128_t));

    // Generate sender's private key.
    uint128_t k = coprime_key(q);

    uint128_t s = mod_power(h, k, q);
    uint128_t p = mod_power(g, k, q);
    en_data -> p = p;

    for (size_t i = 0; i < len; i++)
    {
        int tmp = (int) msg[i];
        en_data->en_msg[i] = s * tmp;
    }

    // Save string conversion of encryption.
    en_data->dataString = toString(en_data->en_msg, len);
}

char *decrypt_gamal(cyphers *en_data, privateKey *privkey)
{
    // Decrypted message initialisation.
    size_t len = en_data->size;
    char *res = malloc((len+1) * sizeof(char));

    uint128_t p = en_data->p;
    uint128_t key = privkey->a;
    uint128_t q = privkey->q;

    uint128_t h = mod_power(p, key, q);

    for (size_t i = 0; i < len; i++)
    {
        char tmp = (char) ((uint128_t) (en_data->en_msg[i] / h));
        res[i] = tmp;
    }

    // Null terminate result.
    res[len] = '\0';

    return res;
}

// Print a uint128 number.
void print_largenum(uint128_t x) 
{
    if (x > 9)
        print_largenum(x / 10);
    
    putchar(x % 10 + '0');

    // Idea: instead of putchar, continuously add
    // number to a buffer.
    // asprintf. Take size returned by asprint and
    // use &(buff+sizeReturned) to asprintf to that part
    // of the string.
}

int largenum_len(uint128_t x)
{
    uint128_t tmp = x;

    // Do conversion and get length of result.
    int len = 0;
    while (tmp)
    {
        tmp /= 10;

        len++;
    }

    return len;
}

// Convert uint128_t to string.
char *largenum_string(uint128_t x)
{
    // Result should have maximum 40 digits.
    char *res = calloc(40, sizeof(char));

    uint128_t tmp = x;

    // Do conversion and get length of result.
    int len = 0;
    while (tmp)
    {
        res[len] = (tmp % 10 + '0');
        tmp /= 10;

        len++;
    }

    // Reverse result.
    char *begin = res;
    char *end = res + len - 1;
    char tmpC;

    while (begin < end)
    {
        tmpC = *begin;
        *begin = *end;
        *end = tmpC;

        begin++;
        end--;
    }

    return res;
}

// Convert array of large numbers to string.
char *toString(uint128_t *data, size_t len)
{
    // Get size of each uint128 in encrypted message.
    size_t en_count = 0;
    for (size_t i = 0; i < len; i++)
    {
        en_count += largenum_len(data[i]);
    }

    // Final string.
    char *res = calloc(en_count + 1, sizeof(char));

    // Convert each uint128 and append to final string.
    en_count = 0;
    for (size_t i = 0; i < len; i++)
    {
        char *msg_part = largenum_string(data[i]);
        strcat(res, msg_part);

        // Free string.
        free(msg_part);
    }
    
    return res;

}

// Public and private keys generation for receiver.
void generateKeys(publicKey *pubKey, privateKey *privKey)
{
    // Receiver's public keys: q, g and h.
    uint128_t q = large_keygen(pow(10,20), pow(10,38));
    uint128_t g = large_keygen(2, q);

    // Receiver's private key.
    uint128_t key = coprime_key(q);

    uint128_t h = mod_power(g, key, q);

    // Save receiver keys to structs.
    pubKey -> q = q;
    pubKey -> g = g;
    pubKey -> h = h;
    privKey -> a = key;
    privKey -> q = q;

}

// Free encrypted data memory.
void freeCyphers(cyphers *data)
{
    free(data->en_msg);
    free(data->dataString);

    // Free entire struct.
    free(data);
}

int main()
{
    char *msg = "figaro is lit";
    printf("Original message : %s\n", msg);

    // Receiver and data structures.
    struct publicKey *receiver_pubkey = malloc(sizeof(struct publicKey));
    struct privateKey *receiver_privkey = malloc(sizeof(struct privateKey));
    struct cyphers *dataCyphers = malloc(sizeof(struct cyphers));
    
    // Generate receiver keys.
    generateKeys(receiver_pubkey, receiver_privkey);

    // Encrypt message and print.
    encrypt_gamal(msg, receiver_pubkey, dataCyphers);
    printf("\nEncryption: \n%s\n", dataCyphers -> dataString);

    // Decrypt data and print.
    char *dr_msg = decrypt_gamal(dataCyphers, receiver_privkey);
    printf("\nDecrypted message: %s\n", dr_msg);

    // Free memory space.
    free(dr_msg);
    free(receiver_pubkey);
    free(receiver_privkey);
    freeCyphers(dataCyphers);

    return 0;

}

