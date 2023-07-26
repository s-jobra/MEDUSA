#include <sylvan.h>
#include <gmp.h>
#include <math.h>
#include "leaf_hash.h"
#include "error.h"

#ifndef MTBDD_H
#define MTBDD_H

/*
 * Custom leaf implementation is taken from: https://github.com/MichalHe/sylvan-custom-leaf-example
 */

/**
 * Global variable for my custom leaf type id
 */
extern uint32_t ltype_id;

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
}cnum;

/**
 * Type for the probability that a given qubit is 1
 */
typedef double prob_t;

/**
 * Complex number coefficient k
 */
extern coef_t c_k;

/* SETUP */
/**
 * Function for Sylvan and Lace setup.
 */
void init_sylvan();

/**
 * Function for my custom leaf setup in Sylvan.
 */
void init_my_leaf();

/* CUSTOM HANDLES */
/**
 * Handle called when my new custom leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_create(uint64_t *ldata_p_raw);

/**
 * Handle called when my custom leaf is destroyed during garbage collection.
 */
void my_leaf_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom leaves.
 */
int my_leaf_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of leaf.
 */
char* my_leaf_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating leaf's hash.
 */
uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
//FIXME: check if binary ops should be ptr and unary should not be ptr
//FIXME: fix naming conventions: every task vs macro (mtbdd_... vs my_mtbdd_...)
//FIXME: fix comments: task + define together, change to "Computes ..." etc
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
 * Operation times with constant ω (right rotate coefficients) for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_coef_rot1, MTBDD, size_t);

/**
 * Operation times with constant ω² (right rotate coefficients twice) for my custom MTBDD.
 */
TASK_DECL_2(MTBDD, my_op_coef_rot2, MTBDD, size_t);

// ==========================================
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
 * Function for calculating the sum of all leafs in a given MTBDD where the target qubit is 1 
 * (with respect to all currently measured qubits).
 */
TASK_DECL_4(prob_t, mtbdd_prob_sum, MTBDD, uint32_t, char*, int);

/**
 * Function for calculating the probability from a given complex number
 * 
 * @param p pointer to a complex number
 * 
 */
static inline prob_t calculate_prob(cnum *prob);

/**
 * Function for creating auxiliary MTBDD for a target qubit.
 * (low -> 0, high -> 1)
 * 
 * @param xt target qubit index
 * 
 */
MTBDD b_xt_create(uint32_t xt);

/**
 * Function for creating auxiliary MTBDD for a target qubit complement.
 * (low -> 1, high -> 0)
 * 
 * @param xt target qubit index
 * 
 */
MTBDD b_xt_comp_create(uint32_t xt);

// ==========================================
//FIXME: fix comments (probably just an MTBDD, not a pointer to an MTBDD) and t vs a,b naming
// Macros for applying operations:
/**
 * Compute a + b with my custom MTBDDs
 * 
 * @param a pointer to an MTBDD
 * 
 * @param b pointer to an MTBDD
 * 
 */
#define my_mtbdd_plus(a, b) mtbdd_apply(a, b, TASK(my_op_plus))

/**
 * Compute a * b with my custom MTBDDs
 * 
 * @param a pointer to an MTBDD
 * 
 * @param b pointer to an MTBDD
 * 
 */
#define my_mtbdd_times(a, b) mtbdd_apply(a, b, TASK(my_op_times))

/**
 * Compute a - b with my custom MTBDDs
 * 
 * @param a pointer to an MTBDD
 * 
 * @param b pointer to an MTBDD
 * 
 */
#define my_mtbdd_minus(a, b) mtbdd_apply(a, b, TASK(my_op_minus))

/**
 * Compute -a for my custom MTBDD
 * 
 * @param a pointer to an MTBDD
 * 
 */
#define my_mtbdd_neg(a) mtbdd_uapply(a, TASK(my_op_negate), 0)

/**
 * Compute a * ω for my custom MTBDD (rotate coefficients)
 * 
 * @param a pointer to an MTBDD
 * 
 */
#define my_mtbdd_coef_rot1(a) mtbdd_uapply(a, TASK(my_op_coef_rot1), 0)

/**
 * Compute a * ω² for my custom MTBDD (rotate coefficients twice)
 * 
 * @param a pointer to an MTBDD
 * 
 */
#define my_mtbdd_coef_rot2(a) mtbdd_uapply(a, TASK(my_op_coef_rot2), 0)

/**
 * Compute Txt on MTBDD t with target qubit xt
 * 
 * @param t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define create_t_xt(t, xt) mtbdd_uapply(t, TASK(t_xt_create), xt)

/**
 * Compute Txt_complement on MTBDD t with target qubit xt
 * 
 * @param t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define create_t_xt_comp(t, xt) mtbdd_uapply(t, TASK(t_xt_comp_create), xt)

// For unitary interface:
/**
 * Compute Bxt with target qubit xt
 * 
 * @param xt target qubit index
 * 
 */
#define create_b_xt(xt) b_xt_create(xt)
/**
 * Compute Bxt_complement with target qubit xt
 * 
 * @param xt target qubit index
 * 
 */
#define create_b_xt_comp(xt) b_xt_comp_create(xt)

/**
 * Computes the sum probability that the target qubit will be 1 in the MTBDD.
 * 
 * @param a pointer an MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param curr_state current state vector (determined by previous measurements)
 * 
 * @param n number of qubits in the circuit
 * 
 */
#define my_mtbdd_prob_sum(a, xt, curr_state, n) RUN(mtbdd_prob_sum, a, xt, curr_state, n)

/**
 * Computes the skip coefficient value for measure
 * 
 * @param start index of the current node
 * 
 * @param end index of next node present
 * 
 * @param target target qubit index
 * 
 */
#define get_coef(start, end, target) \
        (end > target && start < target)? (1 << (end - start - 2)) : (1 << (end - start -1))

#endif
/* end of "mtbdd.h" */