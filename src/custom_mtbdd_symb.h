#include <sylvan.h>
#include <gmp.h>
#include "symbolic_tree.h"
#include "leaf_hash.h"
#include "symbolic_tree.h"
#include "error.h"

#ifndef CUSTOM_MTBDD_SYMB_H
#define CUSTOM_MTBDD_SYMB_H

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

/**
 * Global variable for my custom symbolic leaf type id
 */
extern uint32_t ltype_s_id;

/**
 * MTBDD leaf value for symbolic representation
 */
typedef struct lsymb {
    stree_t *a;
    stree_t *b;
    stree_t *c;
    stree_t *d;
} lsymb_t;

/**
 * Type for the coefficient k for symbolic representation
 */
typedef mpz_t coefs_k_t;

/**
 * Complex number coefficient k for symbolic representation
 */
extern coefs_k_t cs_k;

/* SETUP */
/**
 * Function for my custom symbolic leaf setup in Sylvan.
 */
void init_my_leaf_symb();

/* CUSTOM HANDLES */
/**
 * Handle called when my new custom symbolic leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_symb_create(uint64_t *ldata_p_raw);

/**
 * Handle called when my custom symbolic leaf is destroyed during garbage collection.
 */
void my_leaf_symb_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom symbolic leaves.
 */
int my_leaf_symb_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of the symbolic leaf.
 */
char* my_leaf_symb_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating symbolic leaf's hash.
 */
uint64_t my_leaf_symb_hash(const uint64_t ldata_raw, const uint64_t seed);

#endif
/* end of "custom_mtbdd_symb.h" */