#include "symbolic_tree.h"

stree_t* st_create_val(vars_t v) {
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
    new->type = t->type;

    if (new->type == ST_VAL) {
        new->val = my_malloc(sizeof(stnode_val_t));
        new->val->var = t->val->var;
        mpz_init_set(new->val->coef, t->val->coef);
        new->ls = NULL;
        new->rs = NULL;
    }
    else {
        new->val = NULL;
        new->ls = st_init(t->ls);
        new->rs = st_init(t->rs);
    }

    return new;
}

stree_t* st_op(stree_t *a, stree_t *b, stnode_t op) {
    stree_t *res = my_malloc(sizeof(stree_t));
    res->val = NULL;
    res->type = op;
    res->ls = a;
    res->rs = b;
    return res;
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

//TODO: z3?
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

char* st_to_str(stree_t *t) {
    char buf[MAX_ST_TO_STR_LEN] = {0};
    int chars_written;

    if (t->type == ST_VAL) {
        chars_written = gmp_snprintf(buf, MAX_ST_TO_STR_LEN, "%Zdv[%ld]", t->val->coef, t->val->var);
    }
    else {
        char *l = st_to_str(t->ls);
        char *r = st_to_str(t->rs);

        if (t->type == ST_ADD) {
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "(%s + %s)", l, r);
        }
        else if (t->type == ST_SUB) {
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "(%s - %s)", l, r);
        }
        else { // ST_MUL
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "(%s * %s)", l, r);
        }
        free(l);
        free(r);
    }

    // Was string truncated?
    if (chars_written >= MAX_ST_TO_STR_LEN) {
        error_exit("Allocated string length for leaf value output has not been sufficient.\n");
    }
    else if (chars_written < 0) {
        error_exit("An encoding error has occured when producing leaf value output.\n");
    }

    char *new_buf = (char*)my_malloc((chars_written + 1) * sizeof(char));
    memcpy(new_buf, buf, chars_written * sizeof(char));
    new_buf[chars_written] = '\0';
    return new_buf;
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