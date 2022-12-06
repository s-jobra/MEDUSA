#include <sylvan.h>
#include <string.h>

#include <ctype.h> //FIXME: for overflow/underflow detection - will be removed
#include <assert.h>

#include <gmp.h>

#ifndef CUSTOM_MTBDD_H
#define CUSTOM_MTBDD_H

/**
 * Type of single complex number coefficient
 */
typedef mpz_t coef_t;

/**
 * Complex number in algebraic representation
 */
typedef struct cnum {
    coef_t a;
    coef_t b;
    coef_t c;
    coef_t d;
    coef_t k;
}cnum;

/* ERROR HANDLING */
#define ERROR_TEXT "ERROR: " // Beginning of an error message. FIXME: separate header file?
/**
 * Function that handles program exiting in case of error.
 * 
 * @param error Error message.
*/
void error_exit(const char *error);

/* SETUP */
/**
 * Function for sylvan and lace setup.
 */
void init_sylvan();
/**
 * Function for my custom leaf setup.
 */
void init_my_leaf();

/* CUSTOM HANDLES */
/**
 * Handle called when my new custom leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_create(uint64_t* ldata_p_raw);

/**
 * Handle called when my custom leaf is destroyed during garbage collection.
 */
void my_leaf_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom leaves.
 * 
 * @todo projit ??
 */
int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of leaf.
 * 
 * @todo projit ??
 */
char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char* sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating leaf's hash.
 * 
 * @todo hash function source
 */
uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
// Basic operations:
/**
 * Operation plus for my custom MTBDDs.
 */
TASK_DECL_2(MTBDD, my_op_plus, MTBDD*, MTBDD*);

/**
 * Operation minus for my custom MTBDDs.
 */
TASK_DECL_2(MTBDD, my_op_minus, MTBDD*, MTBDD*);

/**
 * Operation times for my custom MTBDDs.
 */
TASK_DECL_2(MTBDD, my_op_times, MTBDD*, MTBDD*);

/**
 * Operation times with constant -1 for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_negate, MTBDD, size_t);

/**
 * Operation times with constant 1/√2 (increment coefficient k) for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_coef_k_incr, MTBDD, size_t);

/**
 * Operation times with constant ω (right rotate coefficients) for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_coef_rot1, MTBDD, size_t);

/**
 * Operation times with constant ω² (right rotate coefficients twice) for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_coef_rot2, MTBDD, size_t);

// Operations needed for gate representation:
/**
 * Function for creating a new MTBDD from a given one with restriction: target qubit = 1.
 */
TASK_DECL_2(MTBDD, t_xt_create, MTBDD, uint64_t);

/**
 * Function for creating a new MTBDD from a given one with restriction: target qubit = 0.
 */
TASK_DECL_2(MTBDD, t_xt_comp_create, MTBDD, uint64_t);

/**
 * Function for creating MTBDD for a target qubit.
 * (low -> 0, high -> 1)
 */
MTBDD b_xt_create(uint32_t xt);

/**
 * Function for creating MTBDD for a target qubit complement.
 * (low -> 1, high -> 0)
 */
MTBDD b_xt_comp_create(uint32_t xt);

// Macros for applying operations:
/**
 * Compute a + b with my custom MTBDDs
 */
#define my_mtbdd_plus(a, b) mtbdd_apply(a, b, TASK(my_op_plus))

/**
 * Compute a * b with my custom MTBDDs
 */
#define my_mtbdd_times(a, b) mtbdd_apply(a, b, TASK(my_op_times))

/**
 * Compute a - b with my custom MTBDDs
 */
#define my_mtbdd_minus(a, b) mtbdd_apply(a, b, TASK(my_op_minus))

/**
 * Compute -a for my custom MTBDD
 */
#define my_mtbdd_neg(a) mtbdd_uapply(a, TASK(my_op_negate), 0)

/**
 * Compute a * (1/√2) for my custom MTBDD (increments coefficient k)
 */
#define my_mtbdd_coef_k_incr(a) mtbdd_uapply(a, TASK(my_op_coef_k_incr), 0)

/**
 * Compute a * ω for my custom MTBDD (rotate coefficients)
 */
#define my_mtbdd_coef_rot1(a) mtbdd_uapply(a, TASK(my_op_coef_rot1), 0)

/**
 * Compute a * ω² for my custom MTBDD (rotate coefficients twice)
 */
#define my_mtbdd_coef_rot2(a) mtbdd_uapply(a, TASK(my_op_coef_rot2), 0)

/**
 * Compute Txt on MTBDD t with target qubit xt
 */
#define create_t_xt(t, xt) mtbdd_uapply(t, TASK(t_xt_create), xt)

/**
 * Compute Txt_complement on MTBDD t with target qubit xt
 */
#define create_t_xt_comp(t, xt) mtbdd_uapply(t, TASK(t_xt_comp_create), xt)

// For unitary interface:
/**
 * Compute Bxt with target qubit xt
 */
#define create_b_xt(xt) b_xt_create(xt)
/**
 * Compute Bxt_complement with target qubit xt
 */
#define create_b_xt_comp(xt) b_xt_comp_create(xt)

#endif
/* end of "custom_mtbdd.h" */