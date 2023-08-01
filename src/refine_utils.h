#include <sylvan.h>
#include "mtbdd_symb_map.h"
#include "mtbdd_symb_val.h"
#include "symbolic_tree.h"
#include "solver.h"

#ifndef REFINE_UTILS_H
#define REFINE_UTILS_H

/**
 * Type for elements of the update array
 */
typedef stree_t* upd_elem_t;

/**
 * Type for elements of the refined list (old_var -> new_var mapping)
 */
typedef struct ref_elem {
    vars_t old;
    vars_t new;
    struct ref_elem *next;
} ref_elem_t;

/**
 * Type for the refined list
 */
typedef struct ref_list {
    ref_elem_t *first;
    ref_elem_t *cur;
} ref_list_t;

/**
 * Type for encapsulating all data needed during refine
 */
typedef struct rdata {
    upd_elem_t *upd;
    ref_list_t *ref;
    vmap_t *vm;
} rdata_t;

/**
 * Creates and initializes refine data
 */
rdata_t* rdata_create(vmap_t *vm);

/**
 * Disposes refine data except for vmap (as it is used also outside refine)
 */
void rdata_delete(rdata_t *rd);

TASK_DECL_3(MTBDD, mtbdd_symb_refine, MTBDD*, MTBDD*, size_t);
/**
 * Computes refine on the symbolic MTBDD pair
 * 
 * @param p_map pointer to a symbolic map MTBDD
 * 
 * @param p_val pointer to a symbolic value MTBDD
 * 
 * @param rdata ptr to structure cointaining all the data needed for refine (update, refine and map data structures)
 * 
 * @param opid opid needed for the Sylvan's apply
 * 
 */
#define my_mtbdd_symb_refine(p_map, p_val, rdata, opid) \
        mtbdd_applyp(p_map, p_val, (size_t)rdata, TASK(mtbdd_symb_refine), opid)

#endif
/* end of "refine_utils.h" */