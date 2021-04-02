#ifndef TOOLS_H
#define TOOLS_H

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

// Tools for uint128 to string conversion (vice-versa) and display.
int largenum_len(uint128_t x);
void print_largenum(uint128_t x); 
char *toString(uint128_t *data, size_t len);
char *largenum_string(uint128_t x);
uint128_t *fromString(char *enc, size_t finalLen);

// Tools for uint128 to array of strings conversion.
uint128_t *fromStringArr(char **arr, size_t len);
char **toStringArr(uint128_t *data, size_t len);

#endif
