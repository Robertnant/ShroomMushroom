#ifndef TOOLS_H
#define TOOLS_H

// Include headers.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <gmp.h>
#include <math.h>

// Tools for mpz_t to string conversion (vice-versa) and display.
int largenum_len(mpz_t x);
char *toString(mpz_t *data, size_t len);
char *largenum_string(mpz_t x);
void string_largenum(char *str, mpz_t res);
void fromString(char *enc, size_t finalLen, mpz_t *res);

// Huffman tools.
int binDec(char *binStr);
void decBin(int dec, char res[]);
void printArr(int arr[], int n);
char *toChar(char *encData);

#endif
