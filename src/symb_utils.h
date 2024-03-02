#include <sylvan.h>
#include "sylvan_int.h" // for cache_next_opid()
#include "mtbdd.h"
#include "mtbdd_symb_map.h"
#include "mtbdd_symb_val.h"

#ifndef SYMB_UTILS_H
#define SYMB_UTILS_H

/**
 * Type for encapsulating the mtbdd tuple and map function needed for symbolic representation
 */
typedef struct mtbdd_symb {
    MTBDD map;
    MTBDD val;
    vmap_t *vm;
} mtbdd_symb_t;

/**
 * Function for Sylvan setup for symbolic simulation.
 */
void init_sylvan_symb();

/**
 * Creates an symbolic MTBDD from the given MTBDD and returns the mapping of variables to their values
 */
void symb_init(MTBDD *circ, mtbdd_symb_t *symbc);

/**
 * Executes refine on the symbolic MTBDDs and returns true if the MTBDDs haven't been changed.
 */
bool symb_refine(mtbdd_symb_t *symbc);

/**
 * Updates the circuits MTBDD according to the symbolic MTBDD, variable mapping and the number of iterations
 */
void symb_eval(MTBDD *circ,  mtbdd_symb_t *symbc, uint64_t iters);

#endif
/* end of "symb_utils.h" */