#include <stdlib.h>
#include <stdint.h>
#include "error.h"

#ifndef HTAB_H
#define HTAB_H

#define AVG_LEN_MAX 2

typedef struct htab {
    size_t size;                 // number of items in the table
    size_t arr_size;             // bucket count
    struct htab_item **arr_ptr;
} htab_t;

typedef const char *htab_key_t;
typedef unsigned htab_value_t;

typedef struct htab_pair {
    htab_key_t key;
    htab_value_t value;
} htab_pair_t; 

typedef struct htab_item {
    struct htab_item *next;
    htab_pair_t data;
} htab_item_t;

/**
 * Initialize the table
 * 
 * @param n the initial bucket count
 * 
 */
htab_t* htab_init(size_t n);

/**
 * Adds the item with the given key to the table, else (if already exists) increments is value by one
 */
void htab_lookup_add(htab_t *t, htab_key_t key);

/**
 * Prints all hash table items to STDOUT
 */
void htab_print_all(htab_t *t);

/**
 * Delete and dealloc all table items
 */
void htab_clear(htab_t *t);

/**
 * Deletes the table
 */
void htab_free(htab_t *t);

#endif
/* end of "htab.h" */