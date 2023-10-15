#include <stdint.h>
#include <stdbool.h>
#include <gmp.h>
#include "symexp_list.h"
#include "htab.h"
#include "error.h"

#ifndef SYMEXP_H
#define SYMEXP_H

//TODO:FIXME:TODO: all comments

/**
 * Max. string size when converting expression from tree to string
 */
#define MAX_ST_TO_STR_LEN 2000

/**
 * Initializes the symbolic hash table - must be initialized before any symbolic value MTBDD can be initialized!
 */
void symexp_htab_init();

/**
 * Clears the symbolic hash table - deletes all items (the array size is not changed)
 */
void symexp_htab_clear();

/**
 * Deletes the symbolic hash table
 */
void symexp_htab_delete();

/**
 * Creates a new tree for the given complex number coefficient variable
 */
symexp_list_t* symexp_init(vars_t v);

// //FIXME: comment, maybe rename, maybe remove completely if refs wont be used
// symexp_list_t* st_htab_add(symexp_list_t *t);

/**
 * Performs the given operation on the two trees (a op b) and returns the result
 */
symexp_list_t* symexp_op(symexp_list_t *a, symexp_list_t *b, symexp_op_t op);

/**
 * Returns true if the two trees are equal
 */
bool symexp_cmp(symexp_list_t *a, symexp_list_t *b);

/**
 * Returns the string representation of the expression represented by tree
 */
char* symexp_to_str(symexp_list_t *l);

/**
 * Deletes the given tree
 */
void symexp_ref_dec(symexp_list_t *l);

#endif
/* end of "symexp.h" */