#include <sylvan.h>
#include <gmp.h>
#include "custom_mtbdd.h"
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
    stree_t *a;  // ptr to a tree representing the current expression
    stree_t *b;
    stree_t *c;
    stree_t *d;
    vars_t var_a;   // value of the original variable stored
    vars_t var_b;
    vars_t var_c;
    vars_t var_d;
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

/**
 * Function for converting to a symbolic MTBDD
 */
TASK_DECL_3(MTBDD, mtbdd_to_symb, MTBDD, coef_t*, vars_t*);

/**
 * Function for updating the map values according to the given symbolic MTBDD
 */
TASK_DECL_3(MTBDD, mtbdd_update_map, MTBDD, coef_t*,  coef_t*);

/**
 * Function for converting a symbolic MTBDD to a regular one according to the variable mapping
 */
TASK_DECL_2(MTBDD, mtbdd_from_symb, MTBDD, coef_t*);

//FIXME: fix comments - should be probably just an MTBDD, not a pointer to an MTBDD (also in custom_mtbdd.h)

/**
 * Converts the given MTBDD to a symbolic MTBDD
 * 
 * @param t pointer to an MTBDD
 * 
 * @param map array for saving the variable mapping to their values (complex numbers)
 * 
 * @param next_var pointer to a variable for storing the next var index
 * 
 */
#define my_mtbdd_to_symb(t, map, next_var) RUN(mtbdd_to_symb, t, map, next_var)

/**
 * Simulates one symbolic iteration (single update of the map values)
 * 
 * @param t pointer to a symbolic MTBDD
 * 
 * @param map array with the variable mapping to their values (complex numbers)
 * 
 * @param new_map array for storing the new variable mapping
 * 
 */
#define my_mtbdd_update_map(t, map, new_map) RUN(mtbdd_update_map, t, map, new_map)

/**
 * Converts the given symbolic MTBDD to a regular MTBDD according to the mapping values
 * 
 * @param t pointer to a symbolic MTBDD
 * 
 * @param map array with the variable mapping to their values (complex numbers)
 * 
 */
#define my_mtbdd_from_symb(t, map) RUN(mtbdd_from_symb, t, map)

#endif
/* end of "custom_mtbdd_symb.h" */