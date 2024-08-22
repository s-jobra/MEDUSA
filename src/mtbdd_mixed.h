/**
 * @file mtbdd_mixed.h
 * @brief Custom Sylvan mixed value MTBDD type and operations for verfication purposes
 *        (combination of specific values and symbolic expressions in the leaves)
 */

#include <stdint.h>
#include "mtbdd.h"
#include "symexp_list.h"

#ifndef MTBDD_COMB_H
#define MTBDD_COMB_H

/// Number of coefficients (without the factor k) used for complex number representation
#define N_COEFS 4

/// Global variable for my mixed value mtbdd leaf type id
extern uint32_t ltype_mixed_id;

/// Type for the coefficient k for mixed value mtbdds
typedef mpz_t coefm_k_t;

/// Complex number coefficient k for the verification mtbdd
extern coefm_k_t cm_k;

/// Type for mixed value coefficient
typedef struct leaf_mix_coef {
    bool is_symb;        /// True if coefficient is symbolic - num is undefined, else symb is undefined
    coef_t num;          /// Specific value of the coefficient
    symexp_list_t *symb; /// Ptr to a list representing the symbolic expression for the first variable
} leaf_mix_coef_t;

/// Type for mixed value leaf (4 tuple (a,b,c,d), each coefficient is either a specific value or symbolic)
typedef struct leaf_mix {
    leaf_mix_coef_t coefs[N_COEFS];
} leaf_mix_t;

/* SETUP */
/**
 * Function for my mixed value leaf setup in Sylvan
 */
void init_my_leaf_mixed();

/* CUSTOM HANDLES */
/**
 * Handle called when my new mixed value leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_mixed_create(uint64_t *ldata_p_raw);

/**
 * Handle called when my new mixed value leaf is destroyed during garbage collection.
 */
void my_leaf_mixed_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom mixed value leaves.
 */
int my_leaf_mixed_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of the mixed value leaf (for debugging purposes).
 */
char* my_leaf_mixed_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating mixed value leaf's hash.
 */
uint64_t my_leaf_mixed_hash(const uint64_t ldata_raw, const uint64_t seed);

#endif
/* end of "mtbdd_mixed.h" */
