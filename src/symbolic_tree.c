#include "symbolic_tree.h"

stree_t* st_create(vars_t v) {
    stree_t *new = my_malloc(sizeof(stree_t));

    new->val = my_malloc(sizeof(stnode_val_t));
    mpz_init_set_ui(new->val->coef, 1);
    new->val->var = v;

    new->type = ST_VAL;
    new->ls = NULL;
    new->rs = NULL;

    return new;
}

stree_t* st_init(stree_t *t) {
    stree_t *new = my_malloc(sizeof(stree_t));

    new->val = my_malloc(sizeof(stnode_val_t));  //TODO: check if should be realloc
    mpz_init_set(new->val->coef, t->val->coef);
    new->val->var = t->val->var;

    new->type = t->type;
    new->ls = st_init(t->ls);  //TODO: check if should be realloc
    new->rs = st_init(t->rs);

    return new;
}

stree_t* st_op(stree_t *a, stree_t *b, stnode_t op) {
    //TODO: check if shouldnt free a,b

    stree_t *res = my_malloc(sizeof(stree_t));
    res->val = NULL;
    res->ls = st_init(a);
    res->rs = st_init(b);
}

void st_coef_mul(stree_t *t, int64_t c) {
    if (t != NULL) {
        if (t->type != ST_VAL) {
            st_coef_mul(t->ls, c);
            st_coef_mul(t->rs, c);
        }
        else {
            mpz_mul_si(t->val->coef, t->val->coef, (long int)c);
        }
    }
}

bool st_cmp(stree_t *a, stree_t *b) {
    if (a == NULL && b == NULL) {
        return true;
    }

    if (a == NULL || b == NULL) {
        return false;
    }

    if (a->type == b->type) {
        if (a->type == ST_VAL) {
            return (mpz_cmp(a->val->coef, b->val->coef) == 0) && (a->val->var == b->val->var);
        }
        return st_cmp(a->ls, b->ls) && st_cmp(a->rs, b->rs);
    }

    return false;
}

void st_delete(stree_t *t) {
    if (t != NULL) {
        st_delete(t->ls);
        st_delete(t->rs);
        free(t->val);
        free(t);
    }
}

/* end of "symbolic_tree.c" */