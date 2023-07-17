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

/**
 * Type for saving and using the symbolic variable to value mapping
 */
typedef struct vmap {
    coef_t *map;        // array for saving the variable mapping to their values (complex numbers)
    size_t msize; 
    vars_t next_var;    // next variable index to be assigned
} vmap_t;

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
 * Handle for creating string representation of the symbolic leaf (for debugging purposes).
 */
char* my_leaf_symb_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating symbolic leaf's hash.
 */
uint64_t my_leaf_symb_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
// Basic operations:

TASK_DECL_2(MTBDD, mtbdd_to_symb, MTBDD, size_t);
/**
 * Converts the given MTBDD to a symbolic MTBDD
 * 
 * @param t a regular MTBDD
 * 
 * @param m pointer to a vmap_t mapping casted to size_t (needed for the TASK implementation)
 * 
 */
#define my_mtbdd_to_symb(t, m) mtbdd_uapply(t, TASK(mtbdd_to_symb), m)


TASK_DECL_3(MTBDD, mtbdd_update_map, MTBDD, coef_t*,  coef_t*);
/**
 * Simulates one symbolic iteration (single update of the map values)
 * 
 * @param t a symbolic MTBDD
 * 
 * @param map array with the variable mapping to their values (complex numbers)
 * 
 * @param new_map array for storing the new variable mapping
 * 
 */
#define my_mtbdd_update_map(t, map, new_map) RUN(mtbdd_update_map, t, map, new_map)


TASK_DECL_2(MTBDD, mtbdd_from_symb, MTBDD, size_t);
/**
 * Converts the given symbolic MTBDD to a regular MTBDD according to the variable mapping
 * 
 * @param t a symbolic MTBDD
 * 
 * @param map array with the variable mapping to their values casted to size_t (needed for the TASK implementation)
 * 
 */
#define my_mtbdd_from_symb(t, map) mtbdd_uapply(t, TASK(mtbdd_from_symb), map)

// ==========================================
// Operations needed for gate representation:

//FIXME: unite MTBDD params t and a

TASK_DECL_2(MTBDD, mtbdd_symb_plus, MTBDD*, MTBDD*);
/**
 * Computes a + b with symbolic MTBDDs
 * 
 * @param a pointer to a symbolic MTBDD
 * 
 * @param b pointer to a symbolic MTBDD
 * 
 */
#define my_mtbdd_symb_plus(a, b) mtbdd_apply(a, b, TASK(mtbdd_symb_plus))


TASK_DECL_2(MTBDD, mtbdd_symb_minus, MTBDD*, MTBDD*);
/**
 * Computes a - b with symbolic MTBDDs
 * 
 * @param a pointer to a symbolic MTBDD
 * 
 * @param b pointer to a symbolic MTBDD
 * 
 */
#define my_mtbdd_symb_minus(a, b) mtbdd_apply(a, b, TASK(mtbdd_symb_minus))


TASK_DECL_2(MTBDD, mtbdd_symb_neg, MTBDD, size_t);
/**
 * Computes -a for a symbolic MTBDD
 * 
 * @param a pointer to an MTBDD
 * 
 */
#define my_mtbdd_symb_neg(a) mtbdd_uapply(a, TASK(mtbdd_symb_neg), 0)


/**
 * Computes projection (Txt) on MTBDD t with target qubit xt (target qubit = 1)
 * 
 * @param t a symbolic MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_t_xt(t, xt) mtbdd_uapply(t, TASK(t_xt_create), xt)


/**
 * Computes projection (Txt_complement) on MTBDD t with target qubit xt (target qubit = 0)
 * 
 * @param t a symbolic MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_t_xt_comp(t, xt) mtbdd_uapply(t, TASK(t_xt_comp_create), xt)


TASK_DECL_2(MTBDD, mtbdd_symb_b_xt_mul, MTBDD, uint64_t);
/**
 * Computes restriction (Bxt * T) on a symbolic MTBDD (multiplies target with: low -> 0, high -> 1)
 * 
 * @param t pointer to a symbolic MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_b_xt_mul(t, xt) mtbdd_uapply(t, TASK(mtbdd_symb_b_xt_mul), xt)


TASK_DECL_2(MTBDD, mtbdd_symb_b_xt_comp_mul, MTBDD, uint64_t);
/**
 * Computes restriction (Bxt_complement * T) on a symbolic MTBDD (multiplies target with: low -> 1, high -> 0)
 * 
 * @param t pointer to a symbolic MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_b_xt_comp_mul(t, xt) mtbdd_uapply(t, TASK(mtbdd_symb_b_xt_comp_mul), xt)

#endif
/* end of "custom_mtbdd_symb.h" */