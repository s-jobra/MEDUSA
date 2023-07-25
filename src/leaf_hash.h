#include <stdint.h>

#ifndef LEAF_HASH_H
#define LEAF_HASH_H

/**
 * Hash function for 64bit integers.
 * 
 * Taken from: https://stackoverflow.com/a/12996028, author: Thomas Mueller (https://stackoverflow.com/users/382763/thomas-mueller)
 */
uint64_t my_hash(uint64_t x);

/**
 * Hash value and combine it with an already existing hash
 * 
 * Taken from: http://www.boost.org/doc/libs/1_64_0/boost/functional/hash/hash.hpp
 */
#define MY_HASH_COMB(val, data) ( (val) ^ (my_hash(mpz_get_ui(data)) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )

/**
 * Hash value and combine it with an already existing hash for symbolic representation (general pointers)
 */
#define MY_HASH_COMB_SYMB(val, data) ( (val) ^ (my_hash((uint64_t)data) + 0x9e3779b9 + ((val)<<6) + ((val)>>2)) )

#endif
/* end of "leaf_hash.h" */