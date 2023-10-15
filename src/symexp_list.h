// Separate file to avoid circular dependency with htab

#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>
#include "error.h"

#ifndef SYMEXP_LIST_H
#define SYMEXP_LIST_H

/**
 * Type for symbolic variables' coefficients
 */
typedef mpz_t coefs_t;

/**
 * Type for symbolic variables
 */
typedef uint64_t vars_t;

//TODO:FIXME:TODO: comments

typedef enum {
    SYMEXP_ADD,
    SYMEXP_SUB,
} symexp_op_t;

/**
 * 
 */
typedef struct symexp_val {
    coefs_t coef;
    vars_t var;
} symexp_val_t;

/**
 * 
 */
typedef struct symexp_el {
    symexp_val_t *data;
    struct symexp_el *next;
} symexp_el_t;

/**
 * 
 */
typedef struct symexp_list {
    symexp_el_t *first;
    symexp_el_t *active;
} symexp_list_t;

symexp_list_t* symexp_list_create();

void symexp_list_first(symexp_list_t *l);

void symexp_list_next(symexp_list_t *l);

void symexp_list_insert_first(symexp_list_t *l, symexp_val_t *val);

void symexp_list_insert_after(symexp_list_t *l, symexp_val_t *val);

void symexp_list_neg(symexp_list_t *l);

symexp_list_t* symexp_list_mkcpy(symexp_list_t *l);

void symexp_list_del(symexp_list_t *l);


#endif
/* end of "symexp_list.h" */