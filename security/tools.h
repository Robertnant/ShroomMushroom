#ifndef TOOLS_H
#define TOOLS_H

// Tools for mpz_t to string conversion (vice-versa) and display.
int largenum_len(mpz_t x);
char *toString(mpz_t *data, size_t len);
char *largenum_string(mpz_t x);
void string_largenum(char *str, mpz_t res);
void fromString(char *enc, size_t finalLen, mpz_t *res);

/*
// Tools for mpz_t to array of strings conversion.
// mpz_t *fromStringArr(char **arr, size_t len);
char **toStringArr(mpz_t *data, size_t len);
*/

#endif
