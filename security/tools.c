#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "tools.h"

int largenum_len(mpz_t x)
{
    return (int) mpz_sizeinbase(x, 10);
}

// Convert mpz_t to string.
char *largenum_string(mpz_t x)
{
    return mpz_get_str(NULL, 10, x);
}

// Convert single string to mpz_t.
void string_largenum(char *str, mpz_t res)
{
    mpz_set_str(res, str, 10);
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
    int max_length = 80;
    // Initialize res array.
    // mpz_t *res = malloc(finalLen * sizeof(mpz_t));
    for (size_t i = 0; i < finalLen; i++)
        mpz_init(res[i]);

    size_t len = strlen(enc);

    // Counter for current substrings index.
    size_t subIndex = 0;

    // Get each substring.
    // (40 digits maximum for large number types).
    //char *tmp = calloc(80, sizeof(char));
    char tmp[max_length];

    size_t count = 0;

    // Initialize memory for each number from string.
    mpz_t current;
    mpz_init(current);
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
                mpz_set_ui(current, 0);

                for (size_t c = 0; c < count; c++)
                {
                    mpz_mul_ui(current, current, 10);
                    mpz_add_ui(current, current, tmp[c] - '0');
                    // current = current * 10 + (tmp[c] - '0');
                }

                mpz_set(res[subIndex], current);
                // res[subIndex] = current;
                subIndex++;

                // Reset data for next substring.
                for (int i = 0; i < max_length; i++)
                    tmp[i] = '\0';

                count = 0;
            }
        }
    }

    // Free memory.
    mpz_clear(current);
    //free(tmp);

}

/*
// Convert array of large numbers to array of strings.
char **toStringArr(mpz_t *data, size_t len)
{
    char **res = malloc(len * sizeof(char*));

    for (size_t i = 0; i < len; i++)
    {
        res[i] = largenum_string(data[i]);
    }

    return res;
}

// Convert array of strings to array of large numbers.
mpz_t *fromStringArr(char **arr, size_t len)
{
    mpz_t *res = malloc(len * sizeof(mpz_t));

    for (size_t i = 0; i < len; i++)
    {
        mpz_t current = 0;
        for (size_t c = 0; c < strlen(arr[i]); c++)
        {
            current = current * 10 + (arr[i][c] - '0');
        }

        res[i] = current;
    }

    return res;
}
*/
