#include "tools.h"

#define BASE 62
#define MAX_LENGTH 156   // Maximum number of digits in base 512.

int largenum_len(mpz_t x)
{
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

// Convert binary string to decimal.
int binDec(char *binStr)
{
    int dec = 0;
    int bin = atoi(binStr);
 
    for (int i = 0; bin; i++, bin /= 10)
        if (bin % 10)
            dec += pow(2, i);
 
    // printf("%d\n", dec);

    return dec;
}

// Convert decimal to binary string.
// Result will have 8 digits.
void decBin(int dec, char res[])
{
    int i = 7;
    while (i >= 0)
    {
        res[i] = (dec%2) + '0';
        dec /= 2;
        i--;
    }

    // Null terminate string.
    res[8] = '\0';
}

// Convert encoded data from binary to characters.
unsigned char *toChar(char *encData, int *offset, size_t *resSize)
{
    char tmp[9];
    bzero(tmp, 9);

    // Get size of result string (floor division).
    size_t len = strlen(encData);
    *resSize = (size_t) len / 8;

    if (len % 8 != 0)
        *resSize += 1;

    // Allocate 10 extra slots just in case.
    unsigned char *res = calloc((*resSize) + 10, sizeof(unsigned char));
    size_t resIndex = 0;

    size_t i;
    // printf("Enc size: %ld\nAnd Wanted Size: %ld\n", len, *resSize);
    for (i = 0; i < len; i++)
    {
        tmp[i%8] = encData[i];

        // Convert byte set to character.
        if ((i+1) % 8 == 0)
        {
            int dec = binDec(tmp);
            // printf("BinDec values: %d\n", (unsigned char) dec);
            res[resIndex] = (unsigned char) dec;

            // if (res[resIndex] == '\0')
            //    printf("\nWhy tf did I encode a NULL byte %ld?\n", resIndex);
            // TODO Delete.
            // printf("Char: %c\n", res[resIndex]);

            // Reset tmp string.
            bzero(tmp, 8);

            // Increment result string index.
            resIndex++;
        }
    }


    *offset = 0;
    if ((i) % 8 != 0)
    {
        // Pad last set of bits to 8 bits.
        int dec = binDec(tmp);
        // printf("BinDec values: %d\n", (unsigned char) dec);
        res[resIndex] = (unsigned char) dec;
        
        // TODO Delete.
        // printf("Char: %c\n", res[resIndex]);

        // Save offset (number of NULL bytes added).
        *offset = 8 - strlen(tmp);
        // printf("Offset yo: %d\n", *offset);

        resIndex++;
    }

    // printf("Current res index: %ld\n", resIndex);
    // printf("But res size is: %ld\n", resSize);
    res[resIndex] = '\0';
    
    // printf("Current res: %s\n", res);

    return res;
}

// Convert each string character to binary representation.
char *fromChar(unsigned char *data, size_t len, int align)
{
    // printf("Original Len: %ld\n", len);
    // printf("Align: %d\n", align);

    size_t resSize = len * 8 - align;
    
    // printf("Len wanted for fromChar: %ld\n", resSize);
    // Add 10 extra slots just in case.
    char *res = calloc(resSize + 10, sizeof(char));
    size_t c = 0;

    char tmp[9];
    bzero(tmp, 9);

    for (size_t i = 0; i < len - 1; i++)
    {
        // Convert character.
        size_t dec = data[i];
        // printf("Dec is: %ld\n", dec);
        decBin(dec, tmp);
        // printf("Binary: %s\n", tmp);

        // Append converted character.
        for (int j = 0; j < 8; j++)
        {
            res[c] = tmp[j];
            c++;
        }
    }

    // Convert final character.
    size_t dec = data[len-1];
    // printf("Dec is: %ld\n", dec);
    decBin(dec, tmp);

    // Append conversion (exclude extra padding 0s).
    for (int j = align; j < 8; j++)
    {
        res[c] = tmp[j];
        c++;
    }

    return res;
}

// A utility function to print an array of size n
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
 
    printf("\n");
}
