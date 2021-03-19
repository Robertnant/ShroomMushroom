#include <stdio.h>
#include <math.h>
#include <err.h>
#include "rsa.h"

// Generate encryption key.
size_t generate_e(size_t t)
{
    size_t e = 2;

    while (e != t && gcd(e, t) != 1)
        e++;

    // Return -1 if no result is found.
    return e == t ? (-1) : e;
}

// Extended Euclidean Algorithm.
size_t gcd_extended(size_t a, size_t m, size_t *x, size_t *y)
{
    // Base case.
    if (a == 0)
    {
        *x = 0;
        *y = 1;

        return m;
    }

    int recX;
    int recY;

    // Recursive call.
    m %= a;
    size_t res = gcd_extended(m, a, &recX, &recY);

    // Return results.
    *x = recY - (m / a) * recX;
    *y = recX;

    return gcd;
}

// Generate decryption key.
// (With coprime numbers).
size_t generate_d(size_t e, size_t phi)
{
    // Modular inverse of e under phi.
    size_t x, y;
    size_t gcd = gcd_extended(e, phi, &x, &y);

    // Case no result found.
    if (gcd != 1)
        return -1;

    return (x % phi + phi) % phi;
}

// Generate rsa key using two large random prime numbers.
rkeys *generate_keys_rsa(size_t p, size_t q)
{
    // RSA keys structure for result.
    rkeys *keys;

    // Calculate modulus for public and private keys.
    // (Check for overflow).
    size_t n;
    if (__builtin_mul_overflow(p, q, rsa_keys -> n))
        err(3, "Modulus n generation overflow\n");

    // Calculate totient.
    size_t phi = (p-1) * (q-1);

    // Choose integer e co-prime to totient.
    // (1 < e < phi).
    size_t e = generate_e(phi);

    // Error handling for encryption key.
    if (e == -1)
        err(3, "Encryption key generation failed\n");

    size_t d = generate_d(phi, e);
    
    // Error handling for decryption key.
    if (d == -1)
        err(3, "Decryption key generation failed\n");


    // Assign public and private keys to structure.
    keys -> pubkey -> n = n;
    keys -> pubkey -> e = e;
    keys -> privkey -> n = n;
    keys -> privkey -> d = d;

    return keys;

}

int main(int argc, char argv**)
{

    return 0;
}
