#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

// Print a uint128 number.
void print_largenum(uint128_t x) 
{
    if (x > 9)
        print_largenum(x / 10);
    
    putchar(x % 10 + '0');
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

    // Final string with len delimiters.
    char *res = calloc(en_count + len + 1, sizeof(char));

    // Convert each uint128 and append to final string.
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

// Cpnvert string to array of large numbers.
uint128_t *fromString(char *enc, size_t finalLen)
{
    uint128_t *res = malloc(finalLen * sizeof(uint128_t));

    size_t len = strlen(enc);

    // Counter for current substrings index.
    size_t subIndex = 0;

    // Get each substring.
    // (40 digits maximum for large number types).
    char *tmp = calloc(40, sizeof(char));
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
                // Convert substring to uint128_t.
                uint128_t current = 0;
                for (size_t c = 0; c < count; c++)
                {
                    current = current * 10 + (tmp[c] - '0');
                }

                res[subIndex] = current;
                subIndex++;

                // Reset data for next substring.
                for (int i = 0; i < 40; i++)
                    tmp[i] = '\0';

                count = 0;
            }
        }
    }

    // Free memory.
    free(tmp);

    return res;

}

// Convert array of large numbers to array of strings.
char **toStringArr(uint128_t *data, size_t len)
{
    char **res = malloc(len * sizeof(char*));

    for (size_t i = 0; i < len; i++)
    {
        res[i] = largenum_string(data[i]);
    }

    return res;
}

// Convert array of strings to array of large numbers.
uint128_t *fromStringArr(char **arr, size_t len)
{
    uint128_t *res = malloc(len * sizeof(uint128_t));

    for (size_t i = 0; i < len; i++)
    {
        uint128_t current = 0;
        for (size_t c = 0; c < strlen(arr[i]); c++)
        {
            current = current * 10 + (arr[i][c] - '0');
        }

        res[i] = current;
    }

    return res;
}
