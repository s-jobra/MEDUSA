#include <stdint.h>
#include <stdbool.h>
#include <gmp.h>
#include "symbolic_tree_types.h"
#include "htab.h"
#include "error.h"

#ifndef SYMBOLIC_TREE_H
#define SYMBOLIC_TREE_H

/**
 * Max. string size when converting expression from tree to string
 */
#define MAX_ST_TO_STR_LEN 2000

/**
 * Initializes the symbolic hash table - must be initialized before any symbolic value MTBDD can be initialized!
 */
void st_htab_init();

/**
 * Clears the symbolic hash table - deletes all items (the array size is not changed)
 */
void st_htab_clear();

/**
 * Deletes the symbolic hash table
 */
void st_htab_delete();

/** FIXME: update comment
 * Creates a new tree for the given complex number coefficient variable
 */
void st_init_val(stree_t *new_st, stnode_val_t *new_val, vars_t v);

/** FIXME: update comment
 * Creates a new tree and initializes it from the data provided
 */
void st_init(stree_t *new, stree_t *src);

//FIXME: comment
stree_t* st_htab_add(stree_t *t);

/** FIXME: update comment
 * Performs the given operation on the two trees (a op b) and returns the result
 */
void st_op(stree_t **res, stnode_val_t *res_val, stree_t *a, stree_t *b, stnode_t op);

/**
 * Returns true if the two trees are equal
 */
bool st_cmp(stree_t *a, stree_t *b);

/**
 * Returns the string representation of the expression represented by tree
 */
char* st_to_str(stree_t *data);

/**
 * Deletes the given tree
 */
void st_delete(stree_t *t);

#endif
/* end of "symbolic_tree.h" */