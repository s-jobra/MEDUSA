#include "symbolic_tree.h"

vars_t next_var;

stree_t* st_init(stree_t *t) {
    next_var++;
}

stree_t* st_op(stree_t *a, stree_t *b, stnode_t op);

void st_coef_mul(stree_t *t, int64_t c);

void st_delete(stree_t *t);

/* end of "symbolic_tree.c" */