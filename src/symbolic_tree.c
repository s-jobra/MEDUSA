#include "symbolic_tree.h"

//FIXME: should be global?
static htab_t *st_table; // Global symbolic hash table


void st_htab_init(size_t size)
{
    st_table = htab_init(size);
}

void st_htab_clear()
{
    htab_st_clear(st_table);
}

void st_htab_delete()
{
    htab_st_free(st_table);
}

void st_init_val(stree_t *new_st, stnode_val_t *new_val, vars_t v) {
    mpz_init_set_ui(new_val->coef, 1);
    new_val->var = v;

    new_st->val = new_val;
    new_st->type = ST_VAL;
    new_st->ls = NULL;
    new_st->rs = NULL;
}

void st_init(stree_t *new, stree_t *src) {
    if (!src) {
        new = NULL;
    }
    else {
        new->type = src->type;
        new->ls = src->ls;
        new->rs = src->rs;
        // if (src->val) {
        //     new->val = my_malloc(sizeof(stnode_val_t)); //FIXME: should be function param just like in st_init_val
        //     new->val->var = src->val->var;
        //     mpz_init_set(new->val->coef, src->val->coef);
        // }
        // else {
        //     new->val = NULL;
        // }
        new->val = src->val; //FIXME: check if soft copy is enough
    }
}

stree_t* st_htab_add(stree_t *t) {
    if (!t) {
        return NULL;
    }
    return htab_st_lookup_add(st_table, t);
}

void st_op(stree_t **res, stnode_val_t *res_val, stree_t *a, stree_t *b, stnode_t op) {
    //FIXME: check more thoroughly?
    if (a == NULL && b == NULL) {
        *res = NULL;
    }
    // Special binary operation case
    else if (a == NULL || b == NULL && op != ST_NEG) { 
        if (op == ST_ADD) {
            *res = (!a)? b : a; //FIXME: does add/remove htab reference?
        }
        else { // ST_SUB
            if (!a) {
                st_op(res, res_val, b, NULL, ST_NEG);
            }
            else {
                *res = a; //FIXME: does add/remove htab reference?
            }
        }
    }
    // Terminal operation case
    else if (a->type == ST_VAL && (op == ST_NEG || \
                                   (b->type == ST_VAL && a->val->var == b->val->var))) {
        mpz_init(res_val->coef);
        res_val->var = a->val->var;

        (*res)->ls = NULL;
        (*res)->rs = NULL;
        (*res)->type = ST_VAL;
        (*res)->val = res_val;

        switch (op) {
            case ST_NEG:
                mpz_neg(res_val->coef, a->val->coef);
                break;
            case ST_ADD:
                mpz_add(res_val->coef, a->val->coef, b->val->coef);
                break;
            case ST_SUB:
                mpz_sub(res_val->coef, a->val->coef, b->val->coef);
                break;
        }
        //FIXME: does add/remove htab reference?

        if (!mpz_cmp_si(res_val->coef, 0)) {
            mpz_clear(res_val->coef);
            *res = NULL;
        }
    }
    // Regular non-terminal operation case
    else {
        (*res)->val = NULL;
        (*res)->type = op;
        (*res)->ls = a;
        (*res)->rs = b;
        //FIXME: does add/remove htab reference?
    }
}

bool st_cmp(stree_t *a, stree_t *b) {
    return (a == b)? true : false;
}

char* st_to_str(stree_t *t) {
    char buf[MAX_ST_TO_STR_LEN] = {0};
    int chars_written;

    if (t == NULL) {
        return "";
    }
    else if (t->type == ST_VAL) {
        chars_written = gmp_snprintf(buf, MAX_ST_TO_STR_LEN, "%Zdv[%ld]", t->val->coef, t->val->var);
    }
    else {
        char *l = NULL;
        if (t->type != ST_NEG) {
            l = st_to_str(t->ls);
        }
        char *r = st_to_str(t->rs);

        if (t->type == ST_ADD) {
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "(%s + %s)", l, r);
        }
        else if (t->type == ST_SUB) {
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "(%s - %s)", l, r);
        }
        else { // ST_NEG
            chars_written = snprintf(buf, MAX_ST_TO_STR_LEN, "-(%s)", r);
        }
        
        if (!l) {
            free(l);
        }
        if (strlen(r)) { // Can be ""
            free(r);
        }
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
        htab_st_lookup_remove(st_table, t);
    }
}

/* end of "symbolic_tree.c" */