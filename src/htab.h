#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "symexp_list.h"
#include "hash.h"
#include "error.h"

#ifndef HTAB_H
#define HTAB_H

typedef struct htab {            // Hash table
    size_t size;                 // Number of items in the table
    size_t arr_size;             // Bucket count
    struct htab_item **arr_ptr;
} htab_t;

typedef void* htab_key_t;            // Universal hash table data key type
typedef char* htab_m_key_t;          // Key type for measure hash table
typedef symexp_list_t* htab_s_key_t; // Key type for symbolic hash table
typedef unsigned htab_value_t;       // Universal hash table data value type

typedef struct htab_data {       // Data in a hash table item
    htab_key_t key;
    htab_value_t value;
} htab_data_t;

typedef struct htab_item {       // Hash table item
    struct htab_item *next;
    htab_data_t data;
} htab_item_t;

/**
 * Initialize the table
 * 
 * @param n the initial bucket count
 * 
 */
htab_t* htab_init(size_t n);

/**
 * Adds the item with the given symbolic key to the table, else (if already exists) increments its number of references by one.
 * Returns ptr to the key that will be now present in the table.
 */
htab_s_key_t htab_s_lookup_add(htab_t *t, htab_s_key_t key);

/**
 * Decrements the number of references on the given key. If the number of references reaches 0, it also deallocates the item.
 * If the key is not found, the function doesn't do anything.
 */
void htab_s_lookup_remove(htab_t *t, htab_s_key_t key);

/**
 * Deletes and deallocates all symbolic table items
 */
void htab_s_clear(htab_t *t);

/**
 * Deletes the symbolic table
 */
void htab_s_free(htab_t *t);

/**
 * Adds the item with the given string key to the table, else (if already exists) increments its value by one
 */
void htab_m_lookup_add(htab_t *t, htab_m_key_t key);

/**
 * Prints all hash table items to the given stream (used for measurement output)
 */
void htab_m_print_all(htab_t *t, FILE *output);

/**
 * Deletes and deallocates all measure table items
 */
void htab_m_clear(htab_t *t);

/**
 * Deletes the measure table
 */
void htab_m_free(htab_t *t);

#endif
/* end of "htab.h" */