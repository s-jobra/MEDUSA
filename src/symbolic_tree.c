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

//TODO: FIXME: all functions creating new st: now adding st to htab twice (once here, once in make_leaf)
//              A) add to htab only after leaf has been created -> then here malloc and in mtbdd_symb_val free data
//                                                         (or init local variable in mtbdd_symb_val and set its value here)
//              B) idk -> make_leaf does not interact with htab? (not sure if possible/correct)

stree_t* st_create_val(vars_t v) {
    stree_t *res;
    stree_t new;
    stnode_val_t new_val;

    mpz_init_set_ui(new_val.coef, 1);
    new_val.var = v;

    new.val = &new_val;
    new.type = ST_VAL;
    new.ls = NULL;
    new.rs = NULL;

    res = htab_st_lookup_add(st_table, &new);
    mpz_clear(new_val.coef);
    return res;
}

stree_t* st_init(stree_t *t) {
    if (!t) {
        return NULL;
    }
    return htab_st_lookup_add(st_table, t);
}

stree_t* st_op(stree_t *a, stree_t *b, stnode_t op) {
    stree_t* res;
    stree_t new;
    stnode_val_t new_val;

    //FIXME: check more thoroughly?
    if (a == NULL && b == NULL) {
        return NULL;
    }
    // Special binary operation case
    else if (a == NULL || b == NULL && op != ST_NEG) { 
        if (op == ST_ADD) {
            new = (!a)? *b : *a; //FIXME: does add/remove htab reference?
        }
        else { // ST_SUB
            if (!a) {
                return st_op(b, NULL, ST_NEG);
            }
            new = *a; //FIXME: does add/remove htab reference?
        }
    }
    // Terminal operation case
    else if (a->type == ST_VAL && (op == ST_NEG || \
                                   (b->type == ST_VAL && a->val->var == b->val->var))) {
        mpz_init(new_val.coef);
        new_val.var = a->val->var;

        new.ls = NULL;
        new.rs = NULL;
        new.type = ST_VAL;
        new.val = &new_val;

        switch (op) {
            case ST_NEG:
                mpz_neg(new_val.coef, a->val->coef);
                break;
            case ST_ADD:
                mpz_add(new_val.coef, a->val->coef, b->val->coef);
                break;
            case ST_SUB:
                mpz_sub(new_val.coef, a->val->coef, b->val->coef);
                break;
        }
        //FIXME: does add/remove htab reference?

        if (!mpz_cmp_si(new_val.coef, 0)) {
            mpz_clear(new_val.coef);
            return NULL;
        }
    }
    // Regular non-terminal operation case
    else {
        new.val = NULL;
        new.type = op;
        new.ls = a;
        new.rs = b;
        //FIXME: does add/remove htab reference?
    }

    res = htab_st_lookup_add(st_table, &new);
    if (new.type == ST_VAL) {
        mpz_clear(new_val.coef);
    }
    return res;
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