/**
 * @file mtbdd.h
 * @brief Custom Sylvan MTBDD type and operations with algebraic complex number representation in leaves
 */

#include <sylvan.h>
#include <gmp.h>
#include <stdbool.h>

#ifndef MTBDD_H
#define MTBDD_H

/// Global variable for my custom leaf type id
extern uint32_t ltype_id;

/// Opid of mtbdd_apply_gate needed for operation caching
extern uint64_t mtbdd_apply_gate_id;

/// Opid of mtbdd_apply_cgate needed for operation caching
extern uint64_t mtbdd_apply_cgate_id;

/// Type of single complex number coefficient
typedef mpz_t coef_t;

/// Complex number in algebraic representation
typedef struct cnum {
    /// a * 1
    coef_t a;
    /// b * ω
    coef_t b;
    /// c * ω²
    coef_t c;
    /// d * ω³
    coef_t d;
}cnum;

/// Type for the probability that a given qubit is 1
typedef double prob_t;

/// Complex number coefficient k
extern coef_t c_k;

/* SETUP */
/**
 * Function for Sylvan and Lace setup.
 */
void init_sylvan();

/**
 * Function for my custom leaf setup in Sylvan.
 * 
 * @param is_prob true if the result MTBDD should hold the probabilities instead
 */
void init_my_leaf(bool is_prob);

/** 
 * Initialize for all qubit values 0.
 * 
 * @param c pointer to the circuit's MTBDD
 * 
 * @param n number of circuit's qubits
 * 
 */
void circuit_init(MTBDD *c, const uint32_t n);

/** 
 * Deletes the circuit.
 * 
 * @param c pointer to the circuit's MTBDD
 * 
 */
void circuit_delete(MTBDD *c);

/**
 * Stops Sylvan and Lace.
 */
void stop_sylvan();

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
 * Handle for creating string representation of the probability represented by some leaf.
 */
char* my_leaf_to_str_prob(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating leaf's hash.
 */
uint64_t my_leaf_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
// Operations needed for gate representation:

TASK_DECL_2(MTBDD, mtbdd_plus, MTBDD*, MTBDD*); // ptrs needed because of the binary apply
/**
 * Computes a + b with cnum MTBDDs
 * 
 * @param p_a pointer to an MTBDD
 * 
 * @param p_b pointer to an MTBDD
 * 
 */
#define my_mtbdd_plus(p_a, p_b) mtbdd_apply(p_a, p_b, TASK(mtbdd_plus))


TASK_DECL_2(MTBDD, mtbdd_minus, MTBDD*, MTBDD*); // ptrs needed because of the binary apply
/**
 * Computes a - b with cnum MTBDDs
 * 
 * @param p_a pointer to an MTBDD
 * 
 * @param p_b pointer to an MTBDD
 * 
 */
#define my_mtbdd_minus(p_a, p_b) mtbdd_apply(p_a, p_b, TASK(mtbdd_minus))


TASK_DECL_2(MTBDD, mtbdd_times, MTBDD*, MTBDD*); // ptrs needed because of the binary apply
/**
 * Computes a * b with cnum MTBDDs
 * 
 * @param p_a pointer to an MTBDD
 * 
 * @param p_b pointer to an MTBDD
 * 
 */
#define my_mtbdd_times(p_a, p_b) mtbdd_apply(p_a, p_b, TASK(mtbdd_times))

TASK_DECL_2(MTBDD, mtbdd_times_c, MTBDD, size_t);
/**
 * Computes c * t for a cnum MTBDD
 * 
 * @param t an MTBDD
 * 
 * @param c unsigned integer (the multiplication coefficient)
 * 
 */
#define my_mtbdd_times_c(t, c) mtbdd_uapply(t, TASK(mtbdd_times_c), c)

TASK_DECL_2(MTBDD, mtbdd_negate, MTBDD, size_t);
/**
 * Computes -t for a cnum MTBDD
 * 
 * @param t an MTBDD
 * 
 */
#define my_mtbdd_neg(t) mtbdd_uapply(t, TASK(mtbdd_negate), 0)


TASK_DECL_2(MTBDD, mtbdd_coef_rot1, MTBDD, size_t);
/**
 * Computes t * ω for a cnum MTBDD (rotate coefficients)
 * 
 * @param t an MTBDD
 * 
 */
#define my_mtbdd_coef_rot1(t) mtbdd_uapply(t, TASK(mtbdd_coef_rot1), 0)


TASK_DECL_2(MTBDD, mtbdd_coef_rot2, MTBDD, size_t);
/**
 * Computes t * ω² for a cnum MTBDD (rotate coefficients twice)
 * 
 * @param t an MTBDD
 * 
 */
#define my_mtbdd_coef_rot2(t) mtbdd_uapply(t, TASK(mtbdd_coef_rot2), 0)


TASK_DECL_2(MTBDD, t_xt_create, MTBDD, uint64_t);
/**
 * Computes projection (Txt) on MTBDD t with target qubit xt (target qubit = 1)
 * 
 * @param t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_t_xt(t, xt) mtbdd_uapply(t, TASK(t_xt_create), xt)


TASK_DECL_2(MTBDD, t_xt_comp_create, MTBDD, uint64_t);
/**
 * Computes projection (Txt_complement) on MTBDD t with target qubit xt (target qubit = 0)
 * 
 * @param t pointer to an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_t_xt_comp(t, xt) mtbdd_uapply(t, TASK(t_xt_comp_create), xt)


TASK_DECL_2(MTBDD, mtbdd_b_xt_mul, MTBDD*, MTBDD*);
MTBDD mtbdd_b_xt_mul_wrapper(MTBDD t, uint32_t xt);
/**
 * Computes restriction (Bxt * T) on a symbolic MTBDD (multiplies target with: low -> 0, high -> 1)
 * 
 * @param t an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_b_xt_mul(t, xt) mtbdd_b_xt_mul_wrapper(t, xt)


MTBDD mtbdd_b_xt_comp_mul_wrapper(MTBDD t, uint32_t xt);
/**
 * Computes restriction (Bxt_complement * T) on a symbolic MTBDD (multiplies target with: low -> 1, high -> 0)
 * 
 * @param t an MTBDD
 * 
 * @param xt target qubit index
 * 
 */
#define my_mtbdd_b_xt_comp_mul(t, xt) mtbdd_b_xt_comp_mul_wrapper(t, xt)

// ==========================================
// Measurement operations:

#define NOT_MEASURED_CHAR 'x'

TASK_DECL_4(prob_t, mtbdd_prob_sum, MTBDD, uint32_t, char*, int);
/**
 * Computes the sum probability that the target qubit will be 1 in the MTBDD
 * (with respect to all currently measured qubits).
 * 
 * @param t cnum MTBDD
 * 
 * @param xt target qubit index
 * 
 * @param curr_state current state vector (determined by previous measurements)
 * 
 * @param n number of qubits in the circuit
 * 
 */
#define my_mtbdd_prob_sum(t, xt, curr_state, n) RUN(mtbdd_prob_sum, t, xt, curr_state, n)

// Can't be private because of the op measure (in gates.c):
/**
 * Computes the skip coefficient value for measure
 * 
 * @param start index of the current node
 * 
 * @param end index of next node present
 * 
 * @param target target qubit index
 * 
 * @param curr_state current state vector (determined by previous measurements)
 * 
 */
long get_coef(uint32_t start, uint32_t end, uint32_t target, char *curr_state);

#endif
/* end of "mtbdd.h" */