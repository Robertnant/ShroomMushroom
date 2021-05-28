#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "tools.h"

#define BASE 62
#define MAX_LENGTH 156   // Maximum number of digits in base 512.

int largenum_len(mpz_t x)
{
    // printf("Size in base 10: %d\n", mpz_sizeinbase(x,10));
    return (int) mpz_sizeinbase(x, BASE);
}

// Convert mpz_t to string.
char *largenum_string(mpz_t x)
{
    return mpz_get_str(NULL, BASE, x);
}

// Convert single string to mpz_t.
void string_largenum(char *str, mpz_t res)
{
    mpz_set_str(res, str, BASE);
}

// Convert array of large numbers to string.
char *toString(mpz_t *data, size_t len)
{
    // Get size of each mpz_t in encrypted message.
    size_t en_count = 0;
    for (size_t i = 0; i < len; i++)
    {
        en_count += largenum_len(data[i]);
    }

    // Final string with len delimiters.
    char *res = calloc(en_count + len + 1, sizeof(char));

    // Convert each mpz_t and append to final string.
    en_count = 0;
    for (size_t i = 0; i < len; i++)
    {
        char *msg_part = largenum_string(data[i]);
        strcat(res, msg_part);

        // Add delimiter.
        strcat(res, "-");

        // Free string.
        free(msg_part);
    }

    return res;

}

// Convert string to array of large numbers.
void fromString(char *enc, size_t finalLen, mpz_t *res)
{
    for (size_t i = 0; i < finalLen; i++)
        mpz_init(res[i]);

    size_t len = strlen(enc);

    // Counter for current substrings index.
    size_t subIndex = 0;

    // Get each substring.
    char *tmp = calloc(len + 1, sizeof(char));

    size_t count = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (enc[i] != '-')
        {
            tmp[count] = enc[i];
            count++;
        }
        else
        {
            // Process substring if not empty.
            if (count != 0)
            {
                // Convert substring to mpz_t.
                mpz_set_str(res[subIndex], tmp, BASE);
                subIndex++;

                // Reset data for next substring.
                for (int i = 0; i < MAX_LENGTH; i++)
                    tmp[i] = '\0';

                count = 0;
            }
        }
    }

    // Free memory.
    free(tmp);

}

