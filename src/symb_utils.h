/**
 * @file symb_utils.h
 * @brief Symbolic simulation conversion from/to regular simulation and other symbolic sim. utilities
 */

#include <sylvan.h>
#include <stdint.h>
#include "mtbdd_symb_map.h"

#ifndef SYMB_UTILS_H
#define SYMB_UTILS_H

/// Type for encapsulating the mtbdd tuple and map function needed for symbolic representation
typedef struct mtbdd_symb {
    MTBDD map;
    MTBDD val;
    vmap_t *vm;
} mtbdd_symb_t;

// Refine internal:

/**
 * Type for elements of the update array
 */
typedef symexp_list_t* upd_elem_t;

/// Type for elements of the refined list (old_var -> new_var mapping)
typedef struct ref_elem {
    vars_t old;
    vars_t new;
    struct ref_elem *next;
} ref_elem_t;

/// Type for the refined list
typedef struct ref_list {
    ref_elem_t *first;
    ref_elem_t *cur;
} ref_list_t;

/// Type for storing the new variable values (in order to check for conflicts)
typedef struct upd_list{
    upd_elem_t *arr;
    size_t size;
} upd_list_t;

/// Type for encapsulating all data needed during refine
typedef struct rdata {
    upd_list_t *upd;
    ref_list_t *ref;
    vmap_t *vm;
} rdata_t;

/**
 * Function for Sylvan setup for symbolic simulation.
 */
void init_sylvan_symb();

/**
 * Creates an symbolic MTBDD from the given MTBDD and returns the mapping of variables to their values
 */
void symb_init(MTBDD *circ, mtbdd_symb_t *symbc);

/**
 * Creates and initializes refine data
 */
rdata_t* rdata_create(vmap_t *vm);

/**
 * Disposes refine data except for vmap (as it is used also outside refine)
 */
void rdata_delete(rdata_t *rd);

/**
 * Executes refine on the symbolic MTBDDs and returns true if the MTBDDs haven't been changed.
 */
bool symb_refine(mtbdd_symb_t *symbc, rdata_t *rdata);

/**
 * Updates the circuits MTBDD according to the symbolic MTBDD, variable mapping and the number of iterations
 */
void symb_eval(MTBDD *circ,  mtbdd_symb_t *symbc, uint64_t iters, rdata_t *rdata);

#endif
/* end of "symb_utils.h" */