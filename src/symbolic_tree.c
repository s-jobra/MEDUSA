#include "symbolic_tree.h"

//FIXME: should be global?
static htab_t *st_table; // Global symbolic hash table


void st_htab_init(size_t size)
{
    st_table = htab_init(size);
}

void st_htab_clear()
{
    htab_clear(st_table);
}

void st_htab_delete()
{
    htab_free(st_table);
}

stree_t* st_create_val(vars_t v) {
    stree_t new;
    stnode_val_t new_val;

    mpz_init_set_ui(new_val.coef, 1);
    new_val.var = v;

    new.val = &new_val;
    new.type = ST_VAL;
    new.ls = NULL;
    new.rs = NULL;

    return htab_st_lookup_add(st_table, &new);
}

stree_t* st_init(stree_t *t) {
    stree_t new;
    new.type = t->type; //FIXME: check if can't be NULL

    if (new.type == ST_VAL) {
        stnode_val_t new_val;
        new_val.var = t->val->var;
        mpz_init_set(new_val.coef, t->val->coef);
        new.val = &new_val;
        new.ls = NULL;
        new.rs = NULL;
    }
    else {
        new.val = NULL;
        new.ls = st_init(t->ls);
        new.rs = st_init(t->rs);
    }

    return htab_st_lookup_add(st_table, &new);
}

stree_t* st_op(stree_t *a, stree_t *b, stnode_t op) {
    stree_t res;

    //FIXME: make prettier
    //FIXME: check more thoroughly?
    if (a == NULL && b == NULL) {
        return NULL;
    }
    else if (a == NULL || b == NULL) {
        switch (op) {
            case ST_ADD:
                if (a == NULL) {
                    res = *b; //FIXME: does add & remove htab reference?
                }
                else {
                    res = *a;
                }
                break;
            case ST_SUB:
                if (a == NULL) {
                    res = *b;
                    st_coef_mul(&res, -1); //FIXME: wrong
                }
                else {
                    res = *a;
                }
                break;
        }
        return htab_st_lookup_add(st_table, &res); //TODO: FIXME: use st_init or this??
    }
    else if (a->type == ST_VAL && b->type == ST_VAL) {
        if (a->val->var == b->val->var) {
            stnode_val_t temp;
            mpz_init(temp.coef);
            temp.var = a->val->var;
            switch (op) {
                case ST_ADD:
                    mpz_add(temp.coef, a->val->coef, b->val->coef);
                    break;
                case ST_SUB:
                    mpz_sub(temp.coef, a->val->coef, b->val->coef);
                    break;
                case ST_MUL:
                    mpz_mul(temp.coef, a->val->coef, b->val->coef);
                    break;
                default:
                    break;
            }

            if (!mpz_cmp_si(temp.coef, 0)) {
                return NULL;
            }

            //res = st_init(a);
            res.ls = NULL;
            res.rs = NULL;
            res.type = ST_VAL;
            res.val = &temp;

            //mpz_clear(temp); FIXME: where clear? + add clear to other st alloc functions
            return htab_st_lookup_add(st_table, &res); //TODO: FIXME: use st_init or this??
        }
    }

    //res = my_malloc(sizeof(stree_t));
    res.val = NULL;
    res.type = op;
    res.ls = a;
    res.rs = b;

    return htab_st_lookup_add(st_table, &res); //TODO: FIXME: use st_init or this??
}

void st_coef_mul(stree_t *t, int64_t c) {
    //TODO: FIXME: change so that minus is put in front of the expression
    //         ... add NULL as a possible rs value ?
    
    if (t != NULL) {
        if (t->type != ST_VAL) {
            st_coef_mul(t->ls, c);
            st_coef_mul(t->rs, c);
        }
        else {
            if (htab_st_get_val(st_table, t) == 1) {
                mpz_mul_si(t->val->coef, t->val->coef, (long int)c);
            }
            else {
                //FIXME: now doesn't work properly (just a copy)
                stree_t copy;
                copy.ls = t->ls;
                copy.rs = t->rs;
                copy.val = t->val; // not ST_VAL
                copy.type = t->type;
                htab_st_lookup_add(st_table, &copy);
            }
        }
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
        htab_st_lookup_remove(st_table, t);
    }
}

/* end of "symbolic_tree.c" */