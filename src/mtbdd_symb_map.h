#include <sylvan.h>
#include "mtbdd.h"
#include "hash.h"
#include "symexp_list.h"

#ifndef MTBDD_SYMB_MAP_H
#define MTBDD_SYMB_MAP_H

/**
 * Global variable for my custom symbolic map mtbdd leaf type id
 */
extern uint32_t ltype_symb_map_id;

/**
 * MTBDD leaf value with the variable mapping for symbolic representation
 */
typedef struct sl_map {
    vars_t va;
    vars_t vb;
    vars_t vc;
    vars_t vd;
} sl_map_t;

/**
 * Type for saving and using the symbolic variable to value mapping
 */
typedef struct vmap {
    coef_t *map;        // array for saving the variable mapping to their values (complex numbers)
    size_t msize; 
    vars_t next_var;    // next variable index to be assigned
} vmap_t;

/**
 * Allocates and initializes the variable mapping structure (the map itself only allocated)
 */
void vmap_init(vmap_t **vm, size_t size);

/**
 * Appends the existing map with the next_var (with the same value as the 'old' variable)
 */
void vmap_add(vmap_t *vm, vars_t old);

/**
 * Clears and deallocates the map in the given mapping structure
 */
void vmap_clear(vmap_t *vm);

/**
 * Deallocates the given mapping structure
 */
void vmap_delete(vmap_t *vm);

/* SETUP */
/**
 * Function for my custom symbolic map leaf setup in Sylvan.
 */
void init_my_leaf_symb_map();

/* CUSTOM HANDLES */
/**
 * Handle called when my new custom symbolic map leaf is created and is not found in the internal table.
 * It allocates and initializes custom leaf data from the given data pointer.
 */
void my_leaf_symb_m_create(uint64_t *ldata_p_raw);

/**
 * Handle called when my custom symbolic map leaf is destroyed during garbage collection.
 */
void my_leaf_symb_m_destroy(uint64_t ldata);

/**
 * Handle called when comparing two custom symbolic map leaves.
 */
int my_leaf_symb_m_equals(const uint64_t ldata_a_raw, const uint64_t ldata_b_raw);

/**
 * Handle for creating string representation of the symbolic map  leaf (for debugging purposes).
 */
char* my_leaf_symb_m_to_str(int complemented, uint64_t ldata_raw, char *sylvan_buf, size_t sylvan_bufsize);

/**
 * Hashing function for calculating symbolic map leaf's hash.
 */
uint64_t my_leaf_symb_m_hash(const uint64_t ldata_raw, const uint64_t seed);

/* CUSTOM MTBDD OPERATIONS */
// Basic operations:

TASK_DECL_2(MTBDD, mtbdd_to_symb_map, MTBDD, size_t);
/**
 * Converts the given MTBDD to a symbolic map MTBDD
 * 
 * @param t a regular MTBDD
 * 
 * @param m pointer to a vmap_t mapping
 * 
 */
#define my_mtbdd_to_symb_map(t, m) mtbdd_uapply(t, TASK(mtbdd_to_symb_map), (size_t)m)

#endif
/* end of "mtbdd_symb_map.h" */