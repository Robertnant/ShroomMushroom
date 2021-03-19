#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

// GCD function.
int gcd(int a, int b)
{
    if (b == 0)
        return a;

    return gcd(b, a % b); 
}

// Key generators.
size_t large_keygen(uint128_t lower, uint128_t upper)
{
    // To get a different value at each
    // function call.
    srand(time(0));

    size_t a = (rand() % (upper - lower + 1)) + lower;

    return a;
}

size_t coprime_key(size_t q)
{
    size_t a = large_keygen(pow(10,20), q);

    // Check if values are coprime.
    while (gcd(a, q) != 1)
        a = large_keygen(pow(10,20), q);

    return a;
}

// Modular exponential.
size_t mod_power(size_t a, size_t b, size_t m)
{
    // Result initialization.
    size_t res = 1;

    a %= m;

    // Check if a divisible by m.
    if (a == 0)
        return 0;

    while (b > 0)
    {
        // Case if b is odd.
        if ((b%2) != 0)
            res = (res * a) % m;

        // Make b even.
        b /= 2;
        a = (a *a ) % m;
    }

    return res;
}

// Encryption and decryption.
char *encrypt_gamal(char *msg, size_t q, size_t a, size_t g, size_t *p)
{
    // Encrypted message initialization.
    size_t len = strlen(msg);
    char res[len];

    // Generate sender key.
    size_t k = coprime_key(q);
    size_t s = mod_power(h, k, q);
    *p = mod_power(g, k, q);

    for (size_t i = 0; i < len; i++)
    {
        int tmp = msg[i] - '0';
        res[i] = s * tmp;
    }

    return res;
}

char *decrypt_gamal(char *en_msg, size_t p, size_t key, size_t q)
{
    // Decrypted message initialisation.
    size_t len = strlen(en_msg);
    char res[len+1];

    size_t h = mod_pow(p, key, q);

    for (size_t i = 0; i < len; i++)
    {
        char tmp = (char) ((int) (en_msg[i] / h));
        res[i] = tmp;
    }

    // Null terminate result.
    res[len] = '\0';

    return res;
}

int main()
{
    char msg = "encryption";
    printf("Original message : %s\n", msg);
    
    size_t q = large_keygen(pow(10,20), pow(10,50));
    size_t g = large_keygen(2, q);

    // Receiver's private key.
    size_t key = coprime_key(q);
    size_t h = mod_pow(g, key, q);

    size_t p;

    char *en_msg = encrypt_gamal(msg, q, h, gi, p);
    printf("Encrypted message: %s\n", en_msg);

    char *dr_msg = decrypt_gamal(en_msg, p, key, q);
    printf("Decrypted message: %s\n", dr_msg);

    return 0;
}
