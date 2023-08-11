// Separate to avoid circular dependency with htab

#include <stdint.h>
#include <gmp.h>

#ifndef SYMBOLIC_TREE_TYPES_H
#define SYMBOLIC_TREE_TYPES_H

/**
 * Type for symbolic variables' coefficients
 */
typedef mpz_t coefs_t;

/**
 * Type for symbolic variables
 */
typedef uint64_t vars_t;

/**
 * Type for symbolic tree node type
 */
typedef enum {
    ST_ADD, // binary operations:
    ST_SUB,
    ST_MUL, // unary operation
    ST_VAL  // represents leaf node - just a variable with its coefficient
} stnode_t;

/**
 * Type for symbolic tree node value
 */
typedef struct stnode_val {
    coefs_t coef;
    vars_t var;
} stnode_val_t;

/**
 * Type for symbolic tree node
 */
typedef struct stree {
    stnode_val_t *val;
    stnode_t type;
    struct stree *ls; //FIXME: maybe change to union? (coef/ls, var/rs)
    struct stree *rs;
} stree_t;

#endif
/* end of "symbolic_tree_types.h" */