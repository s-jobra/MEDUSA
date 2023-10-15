#include <sylvan.h>
#include <gmp.h>
#include <sylvan_int.h>
#include "mtbdd.h"
#include "mtbdd_symb_map.h"
#include "hash.h"
#include "symexp.h"
#include "error.h"

#ifndef MTBDD_SYMB_VAL_H
#define MTBDD_SYMB_VAL_H

/**
 * Global variable for my custom symbolic expression mtbdd leaf type id
 */
extern uint32_t ltype_symb_expr_id;

/**
 * MTBDD symbolic leaf value
 */
typedef struct sl_val {
    symexp_list_t *a;  // ptr to a list representing the symbolic expression
    symexp_list_t *b;
    symexp_list_t *c;
    symexp_list_t *d;
} sl_val_t;

/**
 * Type for the coefficient k for symbolic representation
 */
typedef mpz_t coefs_k_t;

/**
 * Complex number coefficient k for symbolic representation
 */
extern coefs_k_t cs_k;

/**
 * Sets symbolic coefficient k's value to 0
 */
void cs_k_reset();

/* SETUP */
/**
 * Function for my custom symbolic expression leaf setup in Sylvan.
 */
void init_my_leaf_symb_val();

/* CUSTOM HANDLES */
/**
 * Handle called when my new custom symbolic expression leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_symb_v_create(uint64_t *ldata_p_raw);

/**
 * Handle called when my custom symbolic expression leaf is destroyed during garbage collection.
 */
void my_leaf_symb_v_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom symbolic expression leaves.
 */
int my_leaf_symb_v_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of the symbolic expression leaf (for debugging purposes).
 */
char* my_leaf_symb_v_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating symbolic expression leaf's hash.
 */
uint64_t my_leaf_symb_v_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
// Basic operations:

TASK_DECL_2(MTBDD, mtbdd_map_to_symb_val, MTBDD, size_t);
/**
 * Converts the given symbolic map MTBDD to a symbolic value MTBDD
 * 
 * @param t a regular MTBDD
 * 
 */
#define my_mtbdd_map_to_symb_val(t) mtbdd_uapply(t, TASK(mtbdd_map_to_symb_val), 0)


VOID_TASK_DECL_4(mtbdd_update_map, MTBDD, MTBDD, coef_t*, coef_t*);
/**
 * Simulates one symbolic iteration (single update of the map values)
 * 
 * @param mtbdd_map a symbolic map MTBDD
 * 
 * @param mtbdd_val a symbolic value MTBDD
 * 
 * @param map array with the variable mapping to their values (complex numbers)
 * 
 * @param new_map array for storing the new variable mapping
 * 
 */
#define my_mtbdd_update_map(mtbdd_map, mtbdd_val, map, new_map) \
        RUN(mtbdd_update_map, mtbdd_map, mtbdd_val, map, new_map)


TASK_DECL_2(MTBDD, mtbdd_from_symb, MTBDD, size_t);
/**
 * Converts the given symbolic MTBDD to a regular MTBDD according to the variable mapping
 * 
 * @param t a symbolic map MTBDD
 * 
 * @param map array with the variable mapping to their values
 * 
 */
#define my_mtbdd_from_symb(t, map) mtbdd_uapply(t, TASK(mtbdd_from_symb), (size_t)map)

// ==========================================
// Operations needed for gate representation:

TASK_DECL_2(MTBDD, mtbdd_symb_plus, MTBDD*, MTBDD*); // ptrs needed because of the binary apply
/**
 * Computes a + b with symbolic MTBDDs
 * 
 * @param p_a pointer to a symbolic value MTBDD
 * 
 * @param p_b pointer to a symbolic value MTBDD
 * 
 */
#define my_mtbdd_symb_plus(p_a, p_b) mtbdd_apply(p_a, p_b, TASK(mtbdd_symb_plus))


TASK_DECL_2(MTBDD, mtbdd_symb_minus, MTBDD*, MTBDD*); // ptrs needed because of the binary apply
/**
 * Computes a - b with symbolic MTBDDs
 * 
 * @param p_a pointer to a symbolic value MTBDD
 * 
 * @param p_b pointer to a symbolic value MTBDD
 * 
 */
#define my_mtbdd_symb_minus(p_a, p_b) mtbdd_apply(p_a, p_b, TASK(mtbdd_symb_minus))


TASK_DECL_2(MTBDD, mtbdd_symb_neg, MTBDD, size_t);
/**
 * Computes -a for a symbolic MTBDD
 * 
 * @param t symbolic value MTBDD
 * 
 */
#define my_mtbdd_symb_neg(t) mtbdd_uapply(t, TASK(mtbdd_symb_neg), 0)


TASK_DECL_2(MTBDD, mtbdd_symb_coef_rot1, MTBDD, size_t);
/**
 * Computes t * ω a symbolic MTBDD (rotate coefficients)
 * 
 * @param t a symbolic value MTBDD
 * 
 */
#define my_mtbdd_symb_coef_rot1(t) mtbdd_uapply(t, TASK(mtbdd_symb_coef_rot1), 0)


TASK_DECL_2(MTBDD, mtbdd_symb_coef_rot2, MTBDD, size_t);
/**
 * Computes t * ω² a symbolic MTBDD (rotate coefficients twice)
 * 
 * @param t a symbolic value MTBDD
 * 
 */
#define my_mtbdd_symb_coef_rot2(t) mtbdd_uapply(t, TASK(mtbdd_symb_coef_rot2), 0)


/**
 * Computes projection (Txt) on MTBDD t with target qubit xt (target qubit = 1)
 * 
 * @param t a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_t_xt(t, xt) mtbdd_uapply(t, TASK(t_xt_create), xt)


/**
 * Computes projection (Txt_complement) on MTBDD t with target qubit xt (target qubit = 0)
 * 
 * @param t a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_t_xt_comp(t, xt) mtbdd_uapply(t, TASK(t_xt_comp_create), xt)


TASK_DECL_2(MTBDD, mtbdd_symb_b_xt_mul, MTBDD*, MTBDD*);
MTBDD mtbdd_symb_b_xt_mul_wrapper(MTBDD t, uint32_t xt);
/**
 * Computes restriction (Bxt * T) on a symbolic MTBDD (multiplies target with: low -> 0, high -> 1)
 * 
 * @param t a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_b_xt_mul(t, xt) mtbdd_symb_b_xt_mul_wrapper(t, xt)


MTBDD mtbdd_symb_b_xt_comp_mul_wrapper(MTBDD t, uint32_t xt);
/**
 * Computes restriction (Bxt_complement * T) on a symbolic MTBDD (multiplies target with: low -> 1, high -> 0)
 * 
 * @param t a symbolic value MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_symb_b_xt_comp_mul(t, xt) mtbdd_symb_b_xt_comp_mul_wrapper(t, xt)

#endif
/* end of "mtbdd_symb_val.h" */